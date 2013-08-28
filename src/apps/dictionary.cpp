/* Test for the exception dictionary API.
 *
 * Copyright (C) 2013 Reece H. Dunn
 *
 * This file is part of cainteoir-engine.
 *
 * cainteoir-engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cainteoir-engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with cainteoir-engine.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include "i18n.h"
#include "options.hpp"

#include <ucd/ucd.h>
#include <cainteoir/document.hpp>
#include <cainteoir/engines.hpp>
#include <cainteoir/text.hpp>
#include <cainteoir/unicode.hpp>
#include <cainteoir/stopwatch.hpp>
#include <stdexcept>

namespace rdf = cainteoir::rdf;
namespace tts = cainteoir::tts;

enum class mode_type
{
	from_document,
	list_entries,
	pronounce_entries,
	compare_entries,
	mismatched_entries,
};

static bool matches(const std::list<tts::phoneme> &a, const std::list<tts::phoneme> &b)
{
	auto first1 = a.begin(), last1 = a.end();
	auto first2 = b.begin(), last2 = b.end();
	while (first1 != last1 && first2 != last2)
	{
		if (*first1 != *first2) return false;
		++first1;
		++first2;
	}
	return first1 == last1 && first2 == last2;
}

static void list_entries(const tts::dictionary &dict,
                         std::shared_ptr<tts::phoneme_writer> writer,
                         const char *phonemeset,
                         bool as_dictionary)
{
	writer->reset(stdout);
	for (auto &entry : dict)
	{
		if (as_dictionary)
		{
			int n = fprintf(stdout, "%s", entry.first->str().c_str());
			if (n < 8) fprintf(stdout, "\t");

			if (entry.second.type == tts::dictionary::say_as)
			{
				fprintf(stdout, "\t%s\n",
				        entry.second.text->str().c_str());
			}
			else
			{
				fprintf(stdout, "\t/");
				for (auto p : entry.second.phonemes)
					writer->write(p);
				fprintf(stdout, "/\n");
			}
		}
		else if (entry.second.type == tts::dictionary::say_as)
		{
			ucd::codepoint_t cp = 0;
			cainteoir::utf8::read(entry.second.text->begin(), cp);

			fprintf(stdout, "\"%s\" => \"%s\"@%s [say-as]\n",
			        entry.first->str().c_str(),
			        entry.second.text->str().c_str(),
			        ucd::get_script_string(ucd::lookup_script(cp)));
		}
		else
		{
			fprintf(stdout, "\"%s\" => /",
			        entry.first->str().c_str());
			for (auto p : entry.second.phonemes)
				writer->write(p);
			fprintf(stdout, "/ [%s]\n", phonemeset);
		}
	}
}

static bool pronounce(const tts::dictionary &dict,
                      const std::shared_ptr<cainteoir::buffer> &word,
                      const tts::dictionary::entry &pronunciation,
                      std::shared_ptr<tts::phoneme_reader> &rules,
                      std::shared_ptr<tts::phoneme_writer> &writer,
                      const char *phonemeset,
                      bool as_dictionary,
                      mode_type mode,
                      int depth = 0)
{
	if (pronunciation.type == tts::dictionary::say_as && mode != mode_type::pronounce_entries)
	{
		if (depth == 5)
		{
			fprintf(stderr, "error: too much recursion for entry '%s'.\n", word->str().c_str());
			return false;
		}

		auto entry = dict.lookup(pronunciation.text);
		if (entry.type == tts::dictionary::no_match)
		{
			fprintf(stderr, "error: cannot find '%s' in the dictionary.\n", pronunciation.text->str().c_str());
			return false;
		}

		return pronounce(dict, word, entry,
		                 rules, writer, phonemeset, as_dictionary, mode, depth + 1);
	}

	std::list<tts::phoneme> pronounced;
	rules->reset(word);
	while (rules->read())
		pronounced.push_back(*rules);

	bool match = matches(pronounced, pronunciation.phonemes);
	if (mode == mode_type::mismatched_entries && match)
		return true;

	if (as_dictionary)
	{
		int n = fprintf(stdout, "%s", word->str().c_str());
		if (n < 8) fprintf(stdout, "\t");
		fprintf(stdout, "\t/");
	}
	else
		fprintf(stdout, "\"%s\" => /", word->str().c_str());

	if (mode == mode_type::compare_entries)
	{
		for (auto p : pronunciation.phonemes)
			writer->write(p);
		fprintf(stdout, "/ ... ");
		if (match)
		{
			fprintf(stdout, "matched\n");
		}
		else
		{
			fprintf(stdout, "mismatched; got /");
			for (auto p : pronounced)
				writer->write(p);
			fprintf(stdout, "/\n");
		}
	}
	else
	{
		if (mode == mode_type::mismatched_entries)
		{
			for (auto p : pronunciation.phonemes)
				writer->write(p);
		}
		else
		{
			for (auto p : pronounced)
				writer->write(p);
		}
		if (as_dictionary)
			fprintf(stdout, "/\n");
		else
			fprintf(stdout, "/ [%s]\n", phonemeset);
	}

	return match;
}

static void pronounce(const tts::dictionary &dict,
                      std::shared_ptr<tts::phoneme_reader> rules,
                      std::shared_ptr<tts::phoneme_writer> writer,
                      const char *phonemeset,
                      bool as_dictionary,
                      mode_type mode)
{
	writer->reset(stdout);

	int matched = 0;
	int entries = 0;

	for (auto &entry : dict)
	{
		if (pronounce(dict, entry.first, entry.second,
		              rules, writer, phonemeset, as_dictionary, mode))
			++matched;
		++entries;
	}

	fflush(stdout);

	if (mode == mode_type::compare_entries)
	{
		fprintf(stderr, "... matched: %d (%.0f%%)\n", matched, (float(matched) / entries * 100.0f));
		fprintf(stderr, "... entries: %d\n", entries);
	}
}

static uint32_t from_document(const tts::dictionary &base_dict,
                              tts::dictionary &dict,
                              const char *filename,
                              bool silent)
{
	if (!silent)
		fprintf(stdout, "reading %s\n", (filename == nullptr) ? "<stdin>" : filename);

	rdf::graph metadata;
	auto reader = cainteoir::createDocumentReader(filename, metadata, std::string());
	if (!reader)
	{
		fprintf(stderr, "unsupported document format for file \"%s\"\n",
		        (filename == nullptr) ? "<stdin>" : filename);
		return 0;
	}

	uint32_t words = 0;
	tts::text_reader text(reader);
	while (text.read()) switch (text.event().type)
	{
	case tts::word_uppercase:
	case tts::word_lowercase:
	case tts::word_capitalized:
	case tts::word_mixedcase:
	case tts::word_script:
		if (base_dict.lookup(text.event().text).type == tts::dictionary::no_match)
		{
			dict.add_entry(text.event().text,
			               tts::dictionary::say_as,
			               text.event().text);
			++words;
		}
		break;
	}

	return words;
}

int main(int argc, char ** argv)
{
	try
	{
		mode_type mode = mode_type::from_document;
		bool time = false;
		bool as_dictionary = false;
		bool new_words = false;
		const char *voicename = nullptr;
		const char *language = nullptr;
		const char *ruleset = nullptr;
		const char *dictionary = nullptr;
		const char *phonemeset = "ipa";

		const option_group general_options = { nullptr, {
			{ 'L', "list", no_argument, nullptr,
			  i18n("List the entries in the dictionary"),
			  [&mode](const char *) { mode = mode_type::list_entries; }},
			{ 'D', "as-dictionary", no_argument, nullptr,
			  i18n("List the entries as a Cainteoir Dictionary"),
			  [&as_dictionary](const char *) { as_dictionary = true; }},
			{ 't', "time", no_argument, nullptr,
			  i18n("Time how long it takes to complete the action"),
			  [&time](const char *) { time = true; }},
			{ 'd', "dictionary", required_argument, "DICTIONARY",
			  i18n("Use the words in DICTIONARY"),
			  [&dictionary](const char *arg) { dictionary = arg; }},
			{ 'n', "new-words", no_argument, nullptr,
			  i18n("Only use words not in the loaded dictionary"),
			  [&new_words](const char *) { new_words = true; }},
			{ 'P', "phonemeset", required_argument, "PHONEMESET",
			  i18n("Use PHONEMESET to transcribe phoneme entries (default: ipa)"),
			  [&phonemeset](const char *arg) { phonemeset = arg; }},
		}};

		const option_group pronunciation_options = { i18n("Pronunciation:"), {
			{ 'p', "pronounce", no_argument, nullptr,
			  i18n("Pronounce the dictionary items using the ruleset/engine"),
			  [&mode](const char *) { mode = mode_type::pronounce_entries; }},
			{ 'c', "compare", no_argument, nullptr,
			  i18n("Compare dictionary and ruleset/engine pronunciations"),
			  [&mode](const char *) { mode = mode_type::compare_entries; }},
			{ 'm', "mismatched", no_argument, nullptr,
			  i18n("Only display mismatched pronunciations (implies --compare)"),
			  [&mode](const char *) { mode = mode_type::mismatched_entries; }},
			//{ 'r', "ruleset", required_argument, "RULESET",
			//  i18n("Use the RULESET pronunciation rule file"),
			//  [&ruleset](const char *arg) { ruleset = arg; }},
			{ 'v', "voice", required_argument, "VOICE",
			  i18n("Use the TTS voice named VOICE"),
			  [&voicename](const char *arg) { voicename = arg; }},
			{ 'l', "language", required_argument, "LANG",
			  i18n("Use a TTS voice that speaks the language LANG"),
			  [&language](const char *arg) { language = arg; }},
		}};

		const std::initializer_list<const char *> usage = {
			i18n("dictionary [OPTION..] DOCUMENT.."),
		};

		if (!parse_command_line({ general_options, pronunciation_options }, usage, argc, argv))
			return 0;

		auto writer = tts::createPhonemeWriter(phonemeset);
		cainteoir::stopwatch timer;

		tts::dictionary base_dict;
		tts::dictionary dict;

		uint32_t words = 0;
		if (dictionary != nullptr)
		{
			if (!base_dict.add_entries(cainteoir::path(dictionary)))
			{
				fprintf(stderr, "cannot load dictionary \"%s\"\n", dictionary);
				return 0;
			}
			if (!new_words)
			{
				dict  = base_dict;
				words = dict.size();
			}
		}

		if (dictionary == nullptr || new_words)
		{
			if (argc == 0)
				words += from_document(base_dict, dict, nullptr, as_dictionary);
			else for (int i = 0; i != argc; ++i)
				words += from_document(base_dict, dict, argv[i], as_dictionary);
		}

		switch (mode)
		{
		case mode_type::list_entries:
			list_entries(dict, writer, phonemeset, as_dictionary);
			break;
		case mode_type::pronounce_entries:
		case mode_type::compare_entries:
		case mode_type::mismatched_entries:
			if (ruleset != nullptr)
			{
				auto rules = tts::createPronunciationRules(ruleset);
				if (!rules.get())
				{
					fprintf(stderr, "cannot load letter-to-phoneme rule file \"%s\"\n", argv[1]);
					return 0;
				}
				pronounce(dict, rules, writer, phonemeset, as_dictionary, mode);
			}
			else
			{
				rdf::graph metadata;
				tts::engines engine(metadata);
				if (voicename)
				{
					const rdf::uri *ref = tts::get_voice_uri(metadata, rdf::tts("name"), voicename);
					if (ref)
						engine.select_voice(metadata, *ref);
				}
				else if (language)
				{
					const rdf::uri *ref = tts::get_voice_uri(metadata, rdf::dc("language"), language);
					if (ref)
						engine.select_voice(metadata, *ref);
				}
				pronounce(dict, engine.pronunciation(), writer, phonemeset, as_dictionary, mode);
			}
			break;
		case mode_type::from_document:
			fflush(stdout);
			fprintf(stderr, "... words:   %d\n", words);
			fprintf(stderr, "... indexed: %d\n", dict.size());
			break;
		}

		if (time)
			fprintf(stderr, "... time:    %G\n", timer.elapsed());
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}

/* Cainteoir Engine.
 *
 * Copyright (C) 2010-2012 Reece H. Dunn
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

#include <cainteoir/languages.hpp>
#include <cainteoir/document.hpp>
#include <algorithm>

namespace rdf  = cainteoir::rdf;
namespace rql  = cainteoir::rdf::query;
namespace lang = cainteoir::language;

static std::string to_upper(std::string s)
{
	std::transform(s.begin(), s.end(), s.begin(), ::toupper);
	return s;
}

static std::string to_lower(std::string s)
{
	std::transform(s.begin(), s.end(), s.begin(), ::tolower);
	return s;
}

static std::string capitalize(std::string s)
{
	int len = s.length();
	if (len > 0)
	{
		s[0] = toupper(s[0]);
		std::transform(s.begin()+1, s.end(), s.begin()+1, ::tolower);
	}
	return s;
}

std::initializer_list<std::pair<std::string, lang::tag>> alias2tag = {
	{ "art-lojban",  { "jbo" } },
	{ "cel-gaulish", { "cel-gaulish" } }, // parent=cel, children=[xtg, xcg, xlp, xga]
	{ "en-sc",       { "en", "", "", "scotland" } },
	{ "en-uk",       { "en", "", "GB" } },
	{ "en-wi",       { "en", "", "029" } }, // Caribbean
	{ "es-la",       { "es", "", "419" } }, // Latin America & Caribbean
	{ "hy-west",     { "hy" } },
	{ "i-ami",       { "ami" } },
	{ "i-bnn",       { "bnn" } },
	{ "i-default",   { "und" } },
	{ "i-enochian",  { "i-enochian" } }, // no corresponding preferred tag
	{ "i-hak",       { "hak" } },
	{ "i-klingon",   { "tlh" } },
	{ "i-lux",       { "lb" } },
	{ "i-mingo",     { "i-mingo" } }, // no corresponding preferred tag
	{ "i-navajo",    { "nv" } },
	{ "i-pwn",       { "pwn" } },
	{ "i-tao",       { "tao" } },
	{ "i-tay",       { "tay" } },
	{ "i-tsu",       { "tsu" } },
	{ "no-bok",      { "nb" } },
	{ "no-nyn",      { "nn" } },
	{ "zh-cmn",      { "cmn" } },
	{ "zh-guoyu",    { "cmn" } },
	{ "zh-hak",      { "hak" } },
	{ "zh-hakka",    { "hak" } },
	{ "zh-min",      { "nan" } },
	{ "zh-nan",      { "nan" } },
	{ "zh-xiang",    { "hsn" } },
	{ "zh-yue",      { "yue" } },
};

std::initializer_list<std::pair<std::string, lang::tag>> alias3tag = {
	{ "en-gb-oed",   { "en", "", "GB" } },
	{ "en-uk-north", { "en", "", "GB" } },
	{ "en-uk-rp",    { "en", "", "GB" } },
	{ "en-uk-wmids", { "en", "", "GB" } },
	{ "sgn-be-fr",   { "sfb" } },
	{ "sgn-be-nl",   { "vgt" } },
	{ "sgn-ch-de",   { "sgg" } },
	{ "zh-min-nan",  { "nan" } },
};

lang::tag lang::make_lang(const std::string &lang)
{
	std::string::size_type a = lang.find('-');
	if (a == std::string::npos)
		return { to_lower(lang), "", "" };

	std::string::size_type b = lang.find('-', a+1);
	if (b == std::string::npos)
	{
		std::string subtag = lang.substr(a+1);

		std::string langid = to_lower(lang);
		for (auto id = alias2tag.begin(), last = alias2tag.end(); id != last; ++id)
		{
			if (id->first == langid)
				return id->second;
		}

		if (subtag.length() == 4)
			return { to_lower(lang.substr(0, a)), capitalize(subtag), "" };

		return { to_lower(lang.substr(0, a)), "", to_upper(subtag) };
	}

	std::string langid = to_lower(lang);
	for (auto id = alias3tag.begin(), last = alias3tag.end(); id != last; ++id)
	{
		if (id->first == langid)
			return id->second;
	}

	return { to_lower(lang.substr(0, a)), capitalize(lang.substr(a+1, b-a-1)), to_upper(lang.substr(b+1)) };
}

cainteoir::languages::languages(const char * locale)
{
	cainteoir::document_events events;
	rdf::graph data;
	try
	{
		const char * filename = DATADIR "/" PACKAGE "/languages.rdf";
		printf("loading language data from %s\n", filename);

		cainteoir::parseDocument(filename, events, data);
	}
	catch (const std::exception & e)
	{
		printf("error: %s\n", e.what());
	}

	// This should construct the m_subtags map directly from the RDF metadata.
	// However, with the current list-based implementation lookup of sublists
	// (e.g. all statements with a specific subject) is slow when lookup is
	// chained, especially for large statement sets like the subtag registry.
	//
	// This implementation performs a single pass over the data so the performance
	// is O(n) where n is the number of statements, not O(n^2) or O(n^3).

	std::map<std::string, std::pair<std::string, std::string>> mapping;
	foreach_iter(lang, data)
	{
		if (rql::predicate(*lang) == rdf::rdf("value"))
			mapping[rql::subject(*lang).as<rdf::uri>()->str()].first = rql::value(*lang);
		else if (rql::predicate(*lang) == rdf::dcterms("title"))
			mapping[rql::subject(*lang).as<rdf::uri>()->str()].second = rql::value(*lang);
	}

	foreach_iter(lang, mapping)
	{
		const auto &entry = lang->second;
		if (!entry.first.empty() && !entry.second.empty())
			m_subtags[entry.first] = entry.second;
	}
}

std::string cainteoir::languages::operator()(const std::string & langid)
{
	lang::tag lang = lang::make_lang(langid);

	std::ostringstream name;
	name << m_subtags[lang.lang];
	if (!lang.region.empty())
		name << " (" << m_subtags[lang.region] << ")";
	else if (!lang.variant.empty())
		name << " (" << m_subtags[lang.variant] << ")";

	return name.str();
}

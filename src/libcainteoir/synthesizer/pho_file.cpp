/* MBROLA pho file.
 *
 * Copyright (C) 2014 Reece H. Dunn
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
#include "compatibility.hpp"

#include <cainteoir/prosody.hpp>

namespace tts = cainteoir::tts;
namespace ipa = cainteoir::ipa;
namespace css = cainteoir::css;

float parse_number(const char * &current, const char *end)
{
	int value = 0; // the value of the number
	int divisor = 1; // the number to divide by to convert value to a fraction
	bool is_fraction = false;
	while (current != end) switch (*current)
	{
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		value *= 10;
		value += (*current - '0');
		if (is_fraction)
			divisor *= 10;
		++current;
		break;
	case '.':
		if (is_fraction)
			return float(value) / divisor;
		is_fraction = true;
		++current;
		break;
	default:
		return float(value) / divisor;
	}
	return float(value) / divisor;
}

struct pho_reader : public tts::prosody_reader
{
	pho_reader(const std::shared_ptr<tts::phoneme_parser> &aPhonemeSet);

	void reset(const std::shared_ptr<cainteoir::buffer> &aBuffer);

	bool read();
private:
	std::shared_ptr<tts::phoneme_parser> mPhonemeSet;

	std::shared_ptr<cainteoir::buffer> mBuffer;
	const char *mCurrent;
	const char *mEnd;
};

pho_reader::pho_reader(const std::shared_ptr<tts::phoneme_parser> &aPhonemeSet)
	: mPhonemeSet(aPhonemeSet)
	, mCurrent(nullptr)
	, mEnd(nullptr)
{
}

void pho_reader::reset(const std::shared_ptr<cainteoir::buffer> &aBuffer)
{
	mBuffer = aBuffer;
	if (mBuffer.get())
	{
		mCurrent = mBuffer->begin();
		mEnd = mBuffer->end();
	}
	else
		mCurrent = mEnd = nullptr;
}

bool pho_reader::read()
{
	while (mCurrent != mEnd) switch (*mCurrent)
	{
	case ';':
		while (mCurrent != mEnd && *mCurrent != '\n') ++mCurrent;
		break;
	case '\r': case '\n':
		++mCurrent;
		break;
	default:
		if (!mPhonemeSet->parse(mCurrent, mEnd, phoneme1))
			return false;
		if (*mCurrent != ' ')
		{
			if (!mPhonemeSet->parse(mCurrent, mEnd, phoneme2))
				phoneme2 = ipa::unspecified;
		}
		else
			phoneme2 = ipa::unspecified;

		if (*mCurrent != ' ') throw tts::phoneme_error("expected whitespace after the phoneme");
		++mCurrent;

		duration = css::time(parse_number(mCurrent, mEnd), css::time::milliseconds);
		envelope.clear();

		while (*mCurrent == ' ')
		{
			++mCurrent;

			float offset = parse_number(mCurrent, mEnd);

			if (*mCurrent != ' ') throw tts::phoneme_error("expected whitespace after the offset");
			++mCurrent;

			float pitch = parse_number(mCurrent, mEnd);

			envelope.push_back({ int(offset), { pitch, css::frequency::hertz }});
		}

		return true;
	}
	return false;
}

std::shared_ptr<tts::prosody_reader>
tts::createPhoReader(const std::shared_ptr<phoneme_parser> &aPhonemeSet)
{
	return std::make_shared<pho_reader>(aPhonemeSet);
}

struct pho_writer : public tts::prosody_writer
{
	pho_writer(const std::shared_ptr<tts::phoneme_writer> &aPhonemeSet);

	void reset(FILE *aOutput);

	bool write(const tts::prosody &aProsody);
private:
	std::shared_ptr<tts::phoneme_writer> mPhonemeSet;
	FILE *mOutput;
};

pho_writer::pho_writer(const std::shared_ptr<tts::phoneme_writer> &aPhonemeSet)
	: mPhonemeSet(aPhonemeSet)
	, mOutput(nullptr)
{
}

void pho_writer::reset(FILE *aOutput)
{
	mPhonemeSet->reset(aOutput);
	mOutput = aOutput;
}

bool pho_writer::write(const tts::prosody &aProsody)
{
	if (!mOutput) return false;

	if (!mPhonemeSet->write(aProsody.phoneme1.get(ipa::main | ipa::diacritics | ipa::length)))
		return false;

	if (aProsody.phoneme2 != ipa::unspecified)
	{
		if (!mPhonemeSet->write(aProsody.phoneme2.get(ipa::main | ipa::diacritics | ipa::length)))
			return false;
	}

	fprintf(mOutput, " %G", aProsody.duration.as(css::time::milliseconds).value());
	for (auto &entry : aProsody.envelope)
		fprintf(mOutput, " %d %G", entry.offset, entry.pitch.as(css::frequency::hertz).value());
	fprintf(mOutput, "\n");
	return true;
}

std::shared_ptr<tts::prosody_writer>
tts::createPhoWriter(const std::shared_ptr<phoneme_writer> &aPhonemeSet)
{
	return std::make_shared<pho_writer>(aPhonemeSet);
}
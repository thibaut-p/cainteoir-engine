/* Phoneme Model API.
 *
 * Copyright (C) 2013-2014 Reece H. Dunn
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

#ifndef CAINTEOIR_ENGINE_PHONEME_HPP
#define CAINTEOIR_ENGINE_PHONEME_HPP

#include "buffer.hpp"

#include <stdexcept>
#include <cstdint>

namespace cainteoir { namespace ipa
{
	// features

	// main ---------------------------------------------- 00000000001FFFFF
	constexpr uint64_t phoneme_type           = UINT64_C(0x000000000000000F);
	// ... consonants ------------------------------------ 0000000000003FF0
	constexpr uint64_t place_of_articulation  = UINT64_C(0x00000000000000F0);
	constexpr uint64_t manner_of_articulation = UINT64_C(0x0000000000000F00);
	constexpr uint64_t voiced                 = UINT64_C(0x0000000000001000);
	constexpr uint64_t lateral                = UINT64_C(0x0000000000002000);
	// ... vowels ---------------------------------------- 00000000001FC000
	constexpr uint64_t high                   = UINT64_C(0x0000000000004000);
	constexpr uint64_t mid                    = UINT64_C(0x0000000000008000);
	constexpr uint64_t low                    = UINT64_C(0x0000000000010000);
	constexpr uint64_t lax                    = UINT64_C(0x0000000000020000);
	constexpr uint64_t front                  = UINT64_C(0x0000000000040000);
	constexpr uint64_t back                   = UINT64_C(0x0000000000080000);
	constexpr uint64_t rounded                = UINT64_C(0x0000000000100000);
	// diacritics ---------------------------------------- 000000007FE00000
	constexpr uint64_t syllabic               = UINT64_C(0x0000000000200000);
	constexpr uint64_t release                = UINT64_C(0x0000000000C00000);
	constexpr uint64_t murmured               = UINT64_C(0x0000000001000000);
	constexpr uint64_t velarized              = UINT64_C(0x0000000002000000);
	constexpr uint64_t labialized             = UINT64_C(0x0000000004000000);
	constexpr uint64_t palatalized            = UINT64_C(0x0000000008000000);
	constexpr uint64_t rhoticized             = UINT64_C(0x0000000010000000);
	constexpr uint64_t nasalized              = UINT64_C(0x0000000020000000);
	constexpr uint64_t pharyngealized         = UINT64_C(0x0000000040000000);
	// reserved (unassigned) ----------------------------- 0FFFFFFF80000000
	// suprasegmentals ----------------------------------- F000000000000000
	constexpr uint64_t stress                 = UINT64_C(0x3000000000000000);
	constexpr uint64_t length                 = UINT64_C(0xC000000000000000);

	// phoneme type

	constexpr uint64_t consonant        = UINT64_C(0x0000000000000000);
	constexpr uint64_t vowel            = UINT64_C(0x0000000000000001);
	constexpr uint64_t pause            = UINT64_C(0x0000000000000002);
	constexpr uint64_t syllable_break   = UINT64_C(0x0000000000000003);
	constexpr uint64_t linking          = UINT64_C(0x0000000000000004);
	constexpr uint64_t foot_break       = UINT64_C(0x0000000000000005);
	constexpr uint64_t intonation_break = UINT64_C(0x0000000000000006);

	// place of articulation

	constexpr uint64_t bilabial        = UINT64_C(0x0000000000000000);
	constexpr uint64_t labio_dental    = UINT64_C(0x0000000000000010);
	constexpr uint64_t dental          = UINT64_C(0x0000000000000020);
	constexpr uint64_t alveolar        = UINT64_C(0x0000000000000030);
	constexpr uint64_t palato_alveolar = UINT64_C(0x0000000000000040);
	constexpr uint64_t retroflex       = UINT64_C(0x0000000000000050);
	constexpr uint64_t alveolo_palatal = UINT64_C(0x0000000000000060);
	constexpr uint64_t palatal         = UINT64_C(0x0000000000000070);
	constexpr uint64_t labio_palatal   = UINT64_C(0x0000000000000080);
	constexpr uint64_t velar           = UINT64_C(0x0000000000000090);
	constexpr uint64_t labio_velar     = UINT64_C(0x00000000000000A0);
	constexpr uint64_t uvular          = UINT64_C(0x00000000000000B0);
	constexpr uint64_t pharyngeal      = UINT64_C(0x00000000000000C0);
	constexpr uint64_t epiglottal      = UINT64_C(0x00000000000000D0);
	constexpr uint64_t glottal         = UINT64_C(0x00000000000000E0);

	// manner of articulation

	constexpr uint64_t plosive     = UINT64_C(0x0000000000000000);
	constexpr uint64_t fricative   = UINT64_C(0x0000000000000100);
	constexpr uint64_t nasal       = UINT64_C(0x0000000000000200);
	constexpr uint64_t approximant = UINT64_C(0x0000000000000300);
	constexpr uint64_t trill       = UINT64_C(0x0000000000000400);
	constexpr uint64_t flap        = UINT64_C(0x0000000000000500);
	constexpr uint64_t click       = UINT64_C(0x0000000000000600);
	constexpr uint64_t ejective    = UINT64_C(0x0000000000000700);
	constexpr uint64_t implosive   = UINT64_C(0x0000000000000800);

	// vowel height

	constexpr uint64_t vowel_height = high | mid | low | lax;
	//                              = high
	constexpr uint64_t semi_high    = high | lax;
	constexpr uint64_t upper_mid    = high | mid;
	//                              = mid
	constexpr uint64_t lower_mid    = low  | mid;
	constexpr uint64_t semi_low     = low  | lax;
	//                              = low

	// vowel backness

	constexpr uint64_t vowel_backness = front | back;
	//                                = front
	constexpr uint64_t center         = front | back;
	//                                = back

	// diacritics -- release

	constexpr uint64_t aspirated       = UINT64_C(0x0000000000000000);
	constexpr uint64_t unexploded      = UINT64_C(0x0000000000400000);
	constexpr uint64_t nasal_release   = UINT64_C(0x0000000000800000);
	constexpr uint64_t lateral_release = UINT64_C(0x0000000000C00000);

	// suprasegmentals -- stress

	constexpr uint64_t unstressed       = UINT64_C(0x0000000000000000);
	constexpr uint64_t primary_stress   = UINT64_C(0x1000000000000000);
	constexpr uint64_t secondary_stress = UINT64_C(0x2000000000000000);
	constexpr uint64_t extra_stress     = UINT64_C(0x3000000000000000);

	// suprasegmentals -- length

	constexpr uint64_t extra_short = UINT64_C(0x4000000000000000);
	constexpr uint64_t short_      = UINT64_C(0x0000000000000000);
	constexpr uint64_t half_long   = UINT64_C(0xC000000000000000);
	constexpr uint64_t long_       = UINT64_C(0x8000000000000000);

	struct phoneme
	{
		typedef uint64_t value_type;

		phoneme(value_type value = 0)
			: mValue(value)
		{
		}

		value_type get(value_type mask) const { return mValue & mask; }

		phoneme &set(value_type value)
		{
			mValue |= value;
			return *this;
		}

		phoneme &set(value_type value, value_type mask)
		{
			mValue = (mValue & ~mask) | (value & mask);
			return *this;
		}

		phoneme &set(const char *feature);

		phoneme &clear(value_type value)
		{
			mValue &= ~value;
			return *this;
		}

		bool operator==(const phoneme &rhs) const { return mValue == rhs.mValue; }
		bool operator!=(const phoneme &rhs) const { return mValue != rhs.mValue; }
	private:
		value_type mValue;
	};
}}

namespace cainteoir { namespace tts
{
	enum class feature : uint8_t
	{
		unspecified,

		vowel,

		// phonation

		voiced,
		voiceless,
		murmured,

		// place of articulation

		bilabial,
		labio_dental,
		dental,
		alveolar,
		retroflex,
		palato_alveolar,
		palatal,
		velar,
		uvular,
		pharyngeal,
		epiglottal,
		glottal,

		// manner of articulation

		lateral,
		sibilant,

		plosive,
		fricative,
		nasal,
		approximant,
		trill,
		flap,
		click,
		ejective,
		implosive,

		// vowel height

		high,
		semi_high,
		upper_mid,
		mid,
		lower_mid,
		semi_low,
		low,

		// vowel backness

		front,
		center,
		back,

		// vowel rounding

		unrounded,
		rounded,

		// stress

		primary_stress,
		secondary_stress,

		// prosody

		syllable_break,
		silent_pause,

		// length

		extra_short,
		half_long,
		long_,

		// consonant release

		aspirated,
		unreleased,

		// syllabicity

		syllabic,
		non_syllabic,

		// co-articulation

		velarized,
		labialized,
		palatalized,
		rhoticized,
		nasalized,
		pharyngealized,
	};

	class phoneme
	{
		feature features[8];
	public:
		phoneme();

		phoneme(const feature a,
		        const feature b,
		        const feature c,
		        const feature d = feature::unspecified,
		        const feature e = feature::unspecified,
		        const feature f = feature::unspecified,
		        const feature g = feature::unspecified,
		        const feature h = feature::unspecified);

		bool contains(const feature f) const;

		bool remove(const feature f);

		bool add(const feature f);

		bool operator==(const phoneme &rhs) const;

		bool operator!=(const phoneme &rhs) const
		{
			return !operator==(rhs);
		}

		const feature *begin() const { return features; }
		const feature *end()   const { return features + 8; }
	};

	std::pair<bool, feature> get_feature_id(const char *abbreviation);

	const char *get_feature_abbreviation(const feature f);

	const char *get_feature_name(const feature f);

	struct phoneme_error : public std::runtime_error
	{
		phoneme_error(const std::string &msg) : std::runtime_error(msg)
		{
		}
	};

	struct phoneme_reader : public phoneme
	{
		virtual void reset(const std::shared_ptr<buffer> &aBuffer) = 0;

		virtual bool read() = 0;

		virtual ~phoneme_reader() {}
	};

	std::shared_ptr<phoneme_reader> createExplicitFeaturePhonemeReader();

	std::shared_ptr<phoneme_reader> createPhonemeReader(const char *aPhonemeSet);

	struct phoneme_writer
	{
		virtual void reset(FILE *aOutput) = 0;

		virtual bool write(const phoneme &aPhoneme) = 0;

		virtual const char *name() const = 0;

		virtual ~phoneme_writer() {}
	};

	std::shared_ptr<phoneme_writer> createExplicitFeaturePhonemeWriter();

	std::shared_ptr<phoneme_writer> createPhonemeWriter(const char *aPhonemeSet);

	enum class stress_type
	{
		as_transcribed,
		vowel,
		syllable,
	};

	void make_stressed(std::list<phoneme> &aPhonemes, stress_type aType);
}}

#endif

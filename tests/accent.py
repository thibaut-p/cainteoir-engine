#!/usr/bin/python

# Copyright (C) 2014 Reece H. Dunn
#
# This file is part of cainteoir-engine.
#
# cainteoir-engine is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# cainteoir-engine is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with cainteoir-engine.  If not, see <http://www.gnu.org/licenses/>.

import harness
import sys

if __name__ == '__main__':
	test = harness.TestSuite('accent', sys.argv)
	test.run({'name': 'Accents', 'groups': [
		{'name': 'Phoneme-to-Phoneme', 'type': 'phonemeset', 'tests': [
			{'test': 'accent/match-at-end.ipa', 'result': 'accent/match-at-end.cmu', 'from': 'ipa', 'to': 'ipa', 'accent': 'cmu'},
		]},
		{'name': 'CMU Pronunciation Dictionary (General American)', 'type': 'phonemeset', 'tests': [
			{'test': 'accent/english/consonants.ipa', 'result': 'accent/english/consonants.en', 'from': 'ipa', 'to': 'ipa', 'accent': 'cmu'},
			{'test': 'accent/english/phonemes.ipa', 'result': 'accent/english/phonemes.cmu', 'from': 'ipa', 'to': 'ipa', 'accent': 'cmu'},
		]},
		{'name': 'English (Archaic Received Pronunciation)', 'type': 'phonemeset', 'tests': [
			{'test': 'accent/english/consonants.ipa', 'result': 'accent/english/consonants.en', 'from': 'ipa', 'to': 'ipa', 'accent': 'en-GB-x-rp'},
			{'test': 'accent/english/phonemes.ipa', 'result': 'accent/english/phonemes.en-GB-x-rp', 'from': 'ipa', 'to': 'ipa', 'accent': 'en-GB-x-rp'},
		]},
		{'name': 'English (General American)', 'type': 'phonemeset', 'tests': [
			{'test': 'accent/english/consonants.ipa', 'result': 'accent/english/consonants.en', 'from': 'ipa', 'to': 'ipa', 'accent': 'en-US'},
			{'test': 'accent/english/phonemes.ipa', 'result': 'accent/english/phonemes.en-US', 'from': 'ipa', 'to': 'ipa', 'accent': 'en-US'},
		]},
		{'name': 'English (Scottish English)', 'type': 'phonemeset', 'tests': [
			{'test': 'accent/english/consonants.ipa', 'result': 'accent/english/consonants.ipa', 'from': 'ipa', 'to': 'ipa', 'accent': 'en-GB-scotland'},
			{'test': 'accent/english/phonemes.ipa', 'result': 'accent/english/phonemes.en-GB-scotland', 'from': 'ipa', 'to': 'ipa', 'accent': 'en-GB-scotland'},
		]},
		{'name': 'English (Lancastrian, Northern Lancashire)', 'type': 'phonemeset', 'tests': [
			{'test': 'accent/english/consonants.ipa', 'result': 'accent/english/consonants.en', 'from': 'ipa', 'to': 'ipa', 'accent': 'en-GB-x-lancastr'},
			{'test': 'accent/english/phonemes.ipa', 'result': 'accent/english/phonemes.en-GB-x-lancastr', 'from': 'ipa', 'to': 'ipa', 'accent': 'en-GB-x-lancastr'},
		]},
		{'name': 'English (Birmingham, West Midlands)', 'type': 'phonemeset', 'tests': [
			{'test': 'accent/english/consonants.ipa', 'result': 'accent/english/consonants.en', 'from': 'ipa', 'to': 'ipa', 'accent': 'en-GB-x-brummie'},
			{'test': 'accent/english/phonemes.ipa', 'result': 'accent/english/phonemes.en-GB-x-brummie', 'from': 'ipa', 'to': 'ipa', 'accent': 'en-GB-x-brummie'},
		]},
		{'name': 'English (Caribbean)', 'type': 'phonemeset', 'tests': [
			{'test': 'accent/english/consonants.ipa', 'result': 'accent/english/consonants.en-029', 'from': 'ipa', 'to': 'ipa', 'accent': 'en-029'},
			{'test': 'accent/english/phonemes.ipa', 'result': 'accent/english/phonemes.en-029', 'from': 'ipa', 'to': 'ipa', 'accent': 'en-029'},
		]},
	]})
	test.summary()

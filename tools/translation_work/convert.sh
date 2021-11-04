#!/bin/bash
dos2unix *.strings
./remove_comments.sh MIDI2LR_Chinese_simplified.strings | sed -e '/^$/d' -e 's/" = "/=/' -e 's/";/"/' > TranslatedStrings_zh_cn.txt
./remove_comments.sh MIDI2LR_Chinese_TW.strings | sed -e '/^$/d' -e 's/" = "/=/' -e 's/";/"/' > TranslatedStrings_zh_tw.txt
./remove_comments.sh MIDI2LR_Dutch.strings | sed -e '/^$/d' -e 's/" = "/=/' -e 's/";/"/' > TranslatedStrings_nl.txt
./remove_comments.sh MIDI2LR_English.strings | sed -e '/^$/d' -e 's/" = "/=/' -e 's/";/"/' > TranslatedStrings_en.txt
./remove_comments.sh MIDI2LR_French.strings | sed -e '/^$/d' -e 's/" = "/=/' -e 's/";/"/' > TranslatedStrings_fr.txt
./remove_comments.sh MIDI2LR_German.strings | sed -e '/^$/d' -e 's/" = "/=/' -e 's/";/"/' > TranslatedStrings_de.txt
./remove_comments.sh MIDI2LR_Italian.strings | sed -e '/^$/d' -e 's/" = "/=/' -e 's/";/"/' > TranslatedStrings_it.txt
./remove_comments.sh MIDI2LR_Japanese.strings | sed -e '/^$/d' -e 's/" = "/=/' -e 's/";/"/' > TranslatedStrings_ja.txt
./remove_comments.sh MIDI2LR_Korean.strings | sed -e '/^$/d' -e 's/" = "/=/' -e 's/";/"/' > TranslatedStrings_ko.txt
./remove_comments.sh MIDI2LR_Portuguese_BR.strings | sed -e '/^$/d' -e 's/" = "/=/' -e 's/";/"/' > TranslatedStrings_pt.txt
./remove_comments.sh MIDI2LR_Russian.strings | sed -e '/^$/d' -e 's/" = "/=/' -e 's/";/"/' > TranslatedStrings_ru.txt
./remove_comments.sh MIDI2LR_Spanish.strings | sed -e '/^$/d' -e 's/" = "/=/' -e 's/";/"/' > TranslatedStrings_es.txt
./remove_comments.sh MIDI2LR_Swedish.strings | sed -e '/^$/d' -e 's/" = "/=/' -e 's/";/"/' > TranslatedStrings_sv.txt
./remove_comments.sh MIDI2LR_Thai.strings | sed -e '/^$/d' -e 's/" = "/=/' -e 's/";/"/' > TranslatedStrings_th.txt
unix2dos -m *.txt
#note: use dos2unix -b, then unix2dos
#make another batch file that does dos2unix -b *.strings, then calls this file with ./convert XXX.strings > YYY.txt for each language, then calls unix2dos *.txt

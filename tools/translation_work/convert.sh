#!/bin/bash
cd POE\ plugin
dos2unix *.po
po2csv MIDI2LR_Chinese_simplified.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' > TranslatedStrings_zh_cn.txt
po2csv MIDI2LR_Chinese_TW.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' > TranslatedStrings_zh_tw.txt
po2csv MIDI2LR_Dutch.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' > TranslatedStrings_nl.txt
po2csv MIDI2LR_English.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' > TranslatedStrings_en.txt
po2csv MIDI2LR_French.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' > TranslatedStrings_fr.txt
po2csv MIDI2LR_German.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' > TranslatedStrings_de.txt
po2csv MIDI2LR_Hindi.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' > TranslatedStrings_hi.txt
po2csv MIDI2LR_Italian.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' > TranslatedStrings_it.txt
po2csv MIDI2LR_Japanese.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' > TranslatedStrings_ja.txt
po2csv MIDI2LR_Korean.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' > TranslatedStrings_ko.txt
po2csv MIDI2LR_Norwegian_Bokm_l.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' > TranslatedStrings_nb.txt
po2csv MIDI2LR_Polish.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' > TranslatedStrings_pl.txt
po2csv MIDI2LR_Portuguese_BR.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' > TranslatedStrings_pt.txt
po2csv MIDI2LR_Russian.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' > TranslatedStrings_ru.txt
po2csv MIDI2LR_Spanish.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' > TranslatedStrings_es.txt
po2csv MIDI2LR_Swedish.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' > TranslatedStrings_sv.txt
po2csv MIDI2LR_Thai.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' > TranslatedStrings_th.txt
unix2dos -m *.txt
cd ..
#!/bin/bash
cd POE\ installer
dos2unix *.po
po2csv MIDI2LR_Chinese_simplified.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' -e 's/..$//' > custom-zh_CN.lng
po2csv MIDI2LR_Chinese_TW.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' -e 's/..$//' > custom-zh_TW.lng
po2csv MIDI2LR_Dutch.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' -e 's/..$//' > custom-nl.lng
po2csv MIDI2LR_English.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' -e 's/..$//' > custom-en.lng
po2csv MIDI2LR_French.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' -e 's/..$//' > custom-fr.lng
po2csv MIDI2LR_German.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' -e 's/..$//' > custom-de.lng
po2csv MIDI2LR_Hindi.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' -e 's/..$//' > custom-hi.lng
po2csv MIDI2LR_Italian.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' -e 's/..$//' > custom-it.lng
po2csv MIDI2LR_Japanese.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' -e 's/..$//' > custom-ja.lng
po2csv MIDI2LR_Korean.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' -e 's/..$//' > custom-ko.lng
po2csv MIDI2LR_Norwegian_Bokm_l.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' -e 's/..$//' > custom-nb.lng
po2csv MIDI2LR_Polish.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' -e 's/..$//' > custom-pl.lng
po2csv MIDI2LR_Portuguese_BR.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' -e 's/..$//' > custom-pt.lng
po2csv MIDI2LR_Russian.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' -e 's/..$//' > custom-ru.lng
po2csv MIDI2LR_Spanish.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' -e 's/..$//' > custom-es.lng
po2csv MIDI2LR_Swedish.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' -e 's/..$//' > custom-sv.lng
po2csv MIDI2LR_Thai.po | sed -e '1d' -e 's/^"","//' -e 's/^"Installer","//' -e 's/","/=/' -e 's/..$//' > custom-th.lng
cp custom-pt.lng custom-pt_BR.lng
unix2dos -m *.lng
cd ..
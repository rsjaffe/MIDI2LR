#!/bin/bash
dos2unix *.strings
./remove_comments.sh MIDI2LR_Chinese_simplified.strings | sed -e '/^$/d' -e 's/^"//' -e 's/" = "/=/' -e 's/";//' > custom-zh_CN.lng
./remove_comments.sh MIDI2LR_Chinese_TW.strings | sed -e '/^$/d' -e 's/^"//' -e 's/" = "/=/' -e 's/";//' > custom-zh_TW.lng
./remove_comments.sh MIDI2LR_Dutch.strings | sed -e '/^$/d' -e 's/^"//' -e 's/" = "/=/' -e 's/";//' > custom-nl.lng
./remove_comments.sh MIDI2LR_English.strings | sed -e '/^$/d' -e 's/^"//' -e 's/" = "/=/' -e 's/";//' > custom-en.lng
./remove_comments.sh MIDI2LR_French.strings | sed -e '/^$/d' -e 's/^"//' -e 's/" = "/=/' -e 's/";//' > custom-fr.lng
./remove_comments.sh MIDI2LR_German.strings | sed -e '/^$/d' -e 's/^"//' -e 's/" = "/=/' -e 's/";//' > custom-de.lng
./remove_comments.sh MIDI2LR_Italian.strings | sed -e '/^$/d' -e 's/^"//' -e 's/" = "/=/' -e 's/";//' > custom-it.lng
./remove_comments.sh MIDI2LR_Japanese.strings | sed -e '/^$/d' -e 's/^"//' -e 's/" = "/=/' -e 's/";//' > custom-ja.lng
./remove_comments.sh MIDI2LR_Korean.strings | sed -e '/^$/d' -e 's/^"//' -e 's/" = "/=/' -e 's/";//' > custom-ko.lng
./remove_comments.sh MIDI2LR_Portuguese_BR.strings | sed -e '/^$/d' -e 's/^"//' -e 's/" = "/=/' -e 's/";//' > custom-pt.lng
./remove_comments.sh MIDI2LR_Russian.strings | sed -e '/^$/d' -e 's/^"//' -e 's/" = "/=/' -e 's/";//' > custom-ru.lng
./remove_comments.sh MIDI2LR_Spanish.strings | sed -e '/^$/d' -e 's/^"//' -e 's/" = "/=/' -e 's/";//' > custom-es.lng
./remove_comments.sh MIDI2LR_Swedish.strings | sed -e '/^$/d' -e 's/^"//' -e 's/" = "/=/' -e 's/";//' > custom-sv.lng
./remove_comments.sh MIDI2LR_Thai.strings | sed -e '/^$/d' -e 's/^"//' -e 's/" = "/=/' -e 's/";//' > custom-th.lng
cp custom-pt.lng custom-pt_BR.lng
unix2dos -m *.lng

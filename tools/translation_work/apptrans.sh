#!/bin/bash
dos2unix *.strings
echo '/* Saved in UTF-8 encoding codepage 65001
 *
 * See https://poeditor.com/projects/view?id=359585 for translation sources and notes
 * To help in translations, sign up at https://poeditor.com/join/project?hash=v6U0MvufAn
 * Using Microsoft language portal to review the translations: https://www.microsoft.com/en-us/language/
 * For authorization for events instructions, used Apple Translation Resources, UniversalAccessPref.lg, Position: instructions.content.events
 *
 */
namespace rsj
{
   const auto de = u8R"===(language: German
countries: de at li
' > Translate.txt
./remove_comments.sh MIDI2LR_German.strings | sed -e '/^$/d' -e 's/";/"/' >> Translate.txt
echo ')===";
   const auto es = u8R"===(language: Spanish
countries: es ar cl co cr cu do ec sv gt hn mx ni pa py pe uy ve
' >> Translate.txt
./remove_comments.sh MIDI2LR_Spanish.strings | sed -e '/^$/d' -e 's/";/"/' >> Translate.txt
echo ')===";
   const auto fr = u8R"===(language: French
countries: fr bj bf cf cg cd ga gn ci ml mc ne sn tg ht bi mg
' >> Translate.txt
./remove_comments.sh MIDI2LR_French.strings | sed -e '/^$/d' -e 's/";/"/' >> Translate.txt
echo ')===";
   const auto it = u8R"===(language: Italian
countries: it mt sm va
' >> Translate.txt
./remove_comments.sh MIDI2LR_Italian.strings | sed -e '/^$/d' -e 's/";/"/' >> Translate.txt
echo ')===";
   const auto ja = u8R"===(language: Japanese
countries: jp
' >> Translate.txt
./remove_comments.sh MIDI2LR_Japanese.strings | sed -e '/^$/d' -e 's/";/"/' >> Translate.txt
echo ')===";
   const auto ko = u8R"===(language: Korean
countries: kr kp
' >> Translate.txt
./remove_comments.sh MIDI2LR_Korean.strings | sed -e '/^$/d' -e 's/";/"/' >> Translate.txt
echo ')===";
   const auto nl = u8R"===(language: Dutch
countries: nl sr
' >> Translate.txt
./remove_comments.sh MIDI2LR_Dutch.strings | sed -e '/^$/d' -e 's/";/"/' >> Translate.txt
echo ')===";
   const auto pt = u8R"===(language: Portuguese
countries: pt br ao mz cv gw st
' >> Translate.txt
./remove_comments.sh MIDI2LR_Portuguese_BR.strings | sed -e '/^$/d' -e 's/";/"/' >> Translate.txt
echo ')===";
   const auto ru = u8R"===(language: Russian
countries: ru
' >> Translate.txt
./remove_comments.sh MIDI2LR_Russian.strings | sed -e '/^$/d' -e 's/";/"/' >> Translate.txt
echo ')===";
   const auto sv = u8R"===(language: Swedish
countries: se
' >> Translate.txt
./remove_comments.sh MIDI2LR_Swedish.strings | sed -e '/^$/d' -e 's/";/"/' >> Translate.txt
echo ')===";
   const auto th = u8R"===(language: Thai
countries: th
' >> Translate.txt
./remove_comments.sh MIDI2LR_Thai.strings | sed -e '/^$/d' -e 's/";/"/' >> Translate.txt
echo ')===";
   const auto zh_cn = u8R"===(language: Chinese simplified
countries: cn sg
' >> Translate.txt
./remove_comments.sh MIDI2LR_Chinese_simplified.strings | sed -e '/^$/d' -e 's/";/"/' >> Translate.txt
echo ')===";
   const auto zh_tw = u8R"===(language: Chinese traditional
countries: tw
' >> Translate.txt
./remove_comments.sh MIDI2LR_Chinese_TW.strings | sed -e '/^$/d' -e 's/";/"/' >> Translate.txt
echo ')===";
} // namespace rsj' >> Translate.txt

unix2dos -m Translate.txt
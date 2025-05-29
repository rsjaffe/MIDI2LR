#!/bin/bash
cd POE\ application
dos2unix *.po
echo '/* Saved in UTF-8 encoding codepage 65001
 *
 * Using Microsoft language portal to review the translations: https://www.microsoft.com/en-us/language/
 * For authorization for events instructions, used Apple Translation Resources, UniversalAccessPref.lg, Position: instructions.content.events
 *
 */
namespace rsj
{
   const auto de = u8R"===(language: German
countries: de at li
' > Translate.txt
po2csv MIDI2LR_German.po | sed -e '1d' -e 's/^"","/"/' -e 's/","/" = "/' >> Translate.txt
echo ')===";
   const auto es = u8R"===(language: Spanish
countries: es ar cl co cr cu do ec sv gt hn mx ni pa py pe uy ve
' >> Translate.txt
po2csv MIDI2LR_Spanish.po | sed -e '1d' -e 's/^"","/"/' -e 's/","/" = "/' >> Translate.txt
echo ')===";
   const auto fr = u8R"===(language: French
countries: fr bj bf cf cg cd ga gn ci ml mc ne sn tg ht bi mg
' >> Translate.txt
po2csv MIDI2LR_French.po | sed -e '1d' -e 's/^"","/"/' -e 's/","/" = "/' >> Translate.txt
echo ')===";
   const auto it = u8R"===(language: Italian
countries: it mt sm va
' >> Translate.txt
po2csv MIDI2LR_Italian.po | sed -e '1d' -e 's/^"","/"/' -e 's/","/" = "/' >> Translate.txt
echo ')===";
   const auto hi = u8R"===(language: Hindi
countries: in
' >> Translate.txt
po2csv MIDI2LR_Hindi.po | sed -e '1d' -e 's/^"","/"/' -e 's/","/" = "/' >> Translate.txt
echo ')===";
   const auto ja = u8R"===(language: Japanese
countries: jp
' >> Translate.txt
po2csv MIDI2LR_Japanese.po | sed -e '1d' -e 's/^"","/"/' -e 's/","/" = "/' >> Translate.txt
echo ')===";
   const auto ko = u8R"===(language: Korean
countries: kr kp
' >> Translate.txt
po2csv MIDI2LR_Korean.po | sed -e '1d' -e 's/^"","/"/' -e 's/","/" = "/' >> Translate.txt
echo ')===";
   const auto nl = u8R"===(language: Dutch
countries: nl sr
' >> Translate.txt
po2csv MIDI2LR_Dutch.po | sed -e '1d' -e 's/^"","/"/' -e 's/","/" = "/' >> Translate.txt
echo ')===";
   const auto nb = u8R"===(language: Norwegian
countries: no
' >> Translate.txt
po2csv MIDI2LR_Norwegian_Bokm_l.po | sed -e '1d' -e 's/^"","/"/' -e 's/","/" = "/' >> Translate.txt
echo ')===";
   const auto pl = u8R"===(language: Polish
countries: pl
' >> Translate.txt
po2csv MIDI2LR_Polish.po | sed -e '1d' -e 's/^"","/"/' -e 's/","/" = "/' >> Translate.txt
echo ')===";
   const auto pt = u8R"===(language: Portuguese
countries: pt br ao mz cv gw st
' >> Translate.txt
po2csv MIDI2LR_Portuguese_BR.po | sed -e '1d' -e 's/^"","/"/' -e 's/","/" = "/' >> Translate.txt
echo ')===";
   const auto ru = u8R"===(language: Russian
countries: ru
' >> Translate.txt
po2csv MIDI2LR_Russian.po | sed -e '1d' -e 's/^"","/"/' -e 's/","/" = "/' >> Translate.txt
echo ')===";
   const auto sv = u8R"===(language: Swedish
countries: se
' >> Translate.txt
po2csv MIDI2LR_Swedish.po | sed -e '1d' -e 's/^"","/"/' -e 's/","/" = "/' >> Translate.txt
echo ')===";
   const auto th = u8R"===(language: Thai
countries: th
' >> Translate.txt
po2csv MIDI2LR_Thai.po | sed -e '1d' -e 's/^"","/"/' -e 's/","/" = "/' >> Translate.txt
echo ')===";
   const auto zh_cn = u8R"===(language: Chinese simplified
countries: cn sg
' >> Translate.txt
po2csv MIDI2LR_Chinese_simplified.po | sed -e '1d' -e 's/^"","/"/' -e 's/","/" = "/' >> Translate.txt
echo ')===";
   const auto zh_tw = u8R"===(language: Chinese traditional
countries: tw
' >> Translate.txt
po2csv MIDI2LR_Chinese_TW.po | sed -e '1d' -e 's/^"","/"/' -e 's/","/" = "/' >> Translate.txt
echo ')===";
} // namespace rsj' >> Translate.txt

unix2dos -m Translate.txt
cd
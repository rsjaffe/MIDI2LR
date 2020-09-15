/*
 * This file is part of MIDI2LR. Copyright (C) 2015 by Rory Jaffe.
 *
 * MIDI2LR is free software: you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with MIDI2LR.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */
#include "DebugInfo.h"

#include <exception>

#include <ww898/utf_converters.hpp>

#include <JuceLibraryCode/JuceHeader.h>

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <array>
#include <exception>
#include <string>
#include <unordered_map>

#include <Windows.h>
#include <fmt/format.h>
#include <wil/result.h>

#include "Misc.h"

/* SEE:https://docs.microsoft.com/en-us/windows-hardware/manufacture/desktop/windows-language-pack-default-values
 */
namespace {
#pragma warning(suppress : 26426)
   const std::unordered_map<unsigned long int, std::string> kKeyboardNames {//-V126
       {0x0000041c, "Albanian"}, {0x00000401, "Arabic (101)"}, {0x00010401, "Arabic (102)"},
       {0x00020401, "Arabic (102) AZERTY"}, {0x0000042b, "Armenian Eastern"},
       {0x0002042b, "Armenian Phonetic"}, {0x0003042b, "Armenian Typewriter"},
       {0x0001042b, "Armenian Western"}, {0x0000044d, "Assamese - Inscript"},
       {0x0001042c, "Azerbaijani (Standard)"}, {0x0000082c, "Azerbaijani Cyrillic"},
       {0x0000042c, "Azerbaijani Latin"}, {0x0000046d, "Bashkir"}, {0x00000423, "Belarusian"},
       {0x0001080c, "Belgian (Comma)"}, {0x00000813, "Belgian (Period)"},
       {0x0000080c, "Belgian French"}, {0x00000445, "Bangla (Bangladesh)"},
       {0x00020445, "Bangla (India)"}, {0x00010445, "Bangla (India - Legacy)"},
       {0x0000201a, "Bosnian (Cyrillic)"}, {0x000b0c00, "Buginese"}, {0x00030402, "Bulgarian"},
       {0x00010402, "Bulgarian (Latin)"}, {0x00020402, "Bulgarian (phonetic layout)"},
       {0x00040402, "Bulgarian (phonetic traditional)"}, {0x00000402, "Bulgarian (Typewriter)"},
       {0x00001009, "Canadian French"}, {0x00000c0c, "Canadian French (Legacy)"},
       {0x00011009, "Canadian Multilingual Standard"}, {0x0000085f, "Central Atlas Tamazight"},
       {0x0000045c, "Cherokee Nation"}, {0x0001045c, "Cherokee Nation Phonetic"},
       {0x00000804, "Chinese (Simplified) - US Keyboard"},
       {0x00000404, "Chinese (Traditional) - US Keyboard"},
       {0x00000c04, "Chinese (Traditional, Hong Kong S.A.R.)"},
       {0x00001404, "Chinese (Traditional Macao S.A.R.) US Keyboard"},
       {0x00001004, "Chinese (Simplified, Singapore) - US keyboard"}, {0x0000041a, "Croatian"},
       {0x00000405, "Czech"}, {0x00010405, "Czech (QWERTY)"}, {0x00020405, "Czech Programmers"},
       {0x00000406, "Danish"}, {0x00000439, "Devanagari-INSCRIPT"}, {0x00000465, "Divehi Phonetic"},
       {0x00010465, "Divehi Typewriter"}, {0x00000413, "Dutch"}, {0x00000C51, "Dzongkha"},
       {0x00000425, "Estonian"}, {0x00000438, "Faeroese"}, {0x0000040b, "Finnish"},
       {0x0001083b, "Finnish with Sami"}, {0x0000040c, "French"}, {0x00120c00, "Futhark"},
       {0x00000437, "Georgian"}, {0x00020437, "Georgian (Ergonomic)"},
       {0x00010437, "Georgian (QWERTY)"},
       {0x00030437, "Georgian Ministry of Education and Science Schools"},
       {0x00040437, "Georgian (Old Alphabets)"}, {0x00000407, "German"},
       {0x00010407, "German (IBM)"}, {0x000c0c00, "Gothic"}, {0x00000408, "Greek"},
       {0x00010408, "Greek (220) or Uyghur"}, /* Microsoft assigns both to same KLID */
       {0x00030408, "Greek (220) Latin"}, {0x00020408, "Greek (319)"},
       {0x00040408, "Greek (319) Latin"}, {0x00050408, "Greek Latin"},
       {0x00060408, "Greek Polytonic"}, {0x0000046f, "Greenlandic"}, {0x00000474, "Guarani"},
       {0x00000447, "Gujarati"}, {0x00000468, "Hausa"}, {0x0000040d, "Hebrew"},
       {0x00010439, "Hindi Traditional"}, {0x0000040e, "Hungarian"},
       {0x0001040e, "Hungarian 101-key"}, {0x0000040f, "Icelandic"}, {0x00000470, "Igbo"},
       {0x00004009, "India"}, {0x0000085d, "Inuktitut - Latin"},
       {0x0001045d, "Inuktitut - Naqittaut"}, {0x00001809, "Irish"}, {0x00000410, "Italian"},
       {0x00010410, "Italian (142)"}, {0x00000411, "Japanese"}, {0x00110c00, "Javanese"},
       {0x0000044b, "Kannada"}, {0x0000043f, "Kazakh"}, {0x00000453, "Khmer"},
       {0x00010453, "Khmer (NIDA)"}, {0x00000412, "Korean"}, {0x00000440, "Kyrgyz Cyrillic"},
       {0x00000454, "Lao"}, {0x0000080a, "Latin American"}, {0x00020426, "Latvian (Standard)"},
       {0x00010426, "Latvian (Legacy)"}, {0x00070c00, "Lisu (Basic)"},
       {0x00080c00, "Lisu (Standard)"}, {0x00010427, "Lithuanian"}, {0x00000427, "Lithuanian IBM"},
       {0x00020427, "Lithuanian Standard"}, {0x0000046e, "Luxembourgish"},
       {0x0000042f, "Macedonia (FYROM)"}, {0x0001042f, "Macedonia (FYROM) - Standard"},
       {0x0000044c, "Malayalam"}, {0x0000043a, "Maltese 47-Key"}, {0x0001043a, "Maltese 48-key"},
       {0x00000481, "Maori"}, {0x0000044e, "Marathi"},
       {0x00000850, "Mongolian (Mongolian Script - Legacy)"},
       {0x00020850, "Mongolian (Mongolian Script - Standard)"}, {0x00000450, "Mongolian Cyrillic"},
       {0x00010c00, "Myanmar"}, {0x00090c00, "N'ko"}, {0x00000461, "Nepali"},
       {0x00020c00, "New Tai Lue"}, {0x00000414, "Norwegian"}, {0x0000043b, "Norwegian with Sami"},
       {0x00000448, "Odia"}, {0x000d0c00, "Ol Chiki"}, {0x000f0c00, "Old Italic"},
       {0x000e0c00, "Osmanya"}, {0x00000463, "Pashto (Afghanistan)"},
       {0x00000429, "Persian or Central Kurdish"}, /* Microsoft assigns both to same KLID */
       {0x00050429, "Persian (Standard)"}, {0x000a0c00, "Phags-pa"}, {0x00010415, "Polish (214)"},
       {0x00000415, "Polish (Programmers)"}, {0x00000816, "Portuguese"},
       {0x00000416, "Portuguese (Brazilian ABNT)"}, {0x00010416, "Portuguese (Brazilian ABNT2)"},
       {0x00000446, "Punjabi"}, {0x00000418, "Romanian (Legacy)"},
       {0x00020418, "Romanian (Programmers)"}, {0x00010418, "Romanian (Standard)"},
       {0x00000419, "Russian"}, {0x00020419, "Russian - Mnemonic"},
       {0x00010419, "Russian (Typewriter)"},
       {0x00000485, "Sakha or Yakut"}, /* Microsoft assigns both to same KLID */
       {0x0002083b, "Sami Extended Finland-Sweden"}, {0x0001043b, "Sami Extended Norway"},
       {0x00011809, "Scottish Gaelic"}, {0x00000c1a, "Serbian (Cyrillic)"},
       {0x0000081a, "Serbian (Latin)"}, {0x0000046c, "Sesotho sa Leboa"}, {0x00000432, "Setswana"},
       {0x0000045b, "Sinhala"}, {0x0001045b, "Sinhala - wij 9"}, {0x0000041b, "Slovak"},
       {0x0001041b, "Slovak (QWERTY)"}, {0x00000424, "Slovenian"}, {0x00100c00, "Sora"},
       {0x0001042e, "Sorbian Extended"}, {0x0002042e, "Sorbian Standard"},
       {0x0000042e, "Sorbian Standard (Legacy)"}, {0x0000040a, "Spanish"},
       {0x0001040a, "Spanish Variation"}, {0x0000041d, "Swedish"},
       {0x0000083b, "Swedish with Sami"}, {0x0000100c, "Swiss French"},
       {0x00000807, "Swiss German"}, {0x0000045a, "Syriac"}, {0x0001045a, "Syriac Phonetic"},
       {0x00030c00, "Tai Le"}, {0x00000428, "Tajik"}, {0x00000449, "Tamil"}, {0x00010444, "Tatar"},
       {0x00000444, "Tatar (Legacy)"}, {0x0000044a, "Telugu"}, {0x0000041e, "Thai Kedmanee"},
       {0x0002041e, "Thai Kedmanee (non-ShiftLock)"}, {0x0001041e, "Thai Pattachote"},
       {0x0003041e, "Thai Pattachote (non-ShiftLock)"}, {0x00010451, "Tibetan (PRC - Standard)"},
       {0x00000451, "Tibetan (PRC - Legacy)"}, {0x00050c00, "Tifinagh (Basic)"},
       {0x00060c00, "Tifinagh (Full)"}, {0x0001041f, "Turkish F"}, {0x0000041f, "Turkish Q"},
       {0x00000442, "Turkmen"}, {0x00000480, "Uyghur (Legacy)"}, {0x00000422, "Ukrainian"},
       {0x00020422, "Ukrainian (Enhanced)"}, {0x00000809, "United Kingdom"},
       {0x00000452, "United Kingdom Extended"}, {0x00010409, "United States - Dvorak"},
       {0x00020409, "United States - International"},
       {0x00030409, "United States-Dvorak for left hand"},
       {0x00040409, "United States-Dvorak for right hand"}, {0x00000409, "United States - English"},
       {0x00000420, "Urdu"}, {0x00010480, "Uyghur (Legacy)"}, {0x00000843, "Uzbek Cyrillic"},
       {0x0000042a, "Vietnamese"}, {0x00000488, "Wolof"}, {0x0000046a, "Yoruba"}};

   std::string GetKeyboardLayout()
   {
      try {
         static_assert(sizeof(CHAR) == sizeof(char), "Windows CHAR and char different sizes.");
         std::array<CHAR, KL_NAMELENGTH> klid_ascii {};
         if (LOG_IF_WIN32_BOOL_FALSE(GetKeyboardLayoutNameA(klid_ascii.data()))) {
            try {
               const auto klid {std::stoul(std::string(klid_ascii.data()), nullptr, 16)};
               if (const auto f = kKeyboardNames.find(klid); f != kKeyboardNames.end())
                  return f->second;
               return fmt::format("KLID not in keyboard_names: 0x{}.", klid_ascii.data());
            }
            catch (...) {
               auto msg {
                   fmt::format("Exception when finding KLID name. KLID: 0x{}.", klid_ascii.data())};
               rsj::Log(msg);
               return msg;
            }
         }
         return "Unable to get KLID.";
      }
      catch (const std::exception& e) {
         MIDI2LR_E_RESPONSE_F;
         throw;
      }
   }
} // namespace
#endif

#pragma warning(push)
#pragma warning(disable : 26447) /* all exceptions suppressed by catch blocks */
DebugInfo::DebugInfo(const juce::String& profile_directory) noexcept
{
   try {
      LogAndSave(fmt::format("Application: System language {}.",
          juce::SystemStats::getDisplayLanguage().toStdString()));
      // ReSharper disable CppUnreachableCode
      if constexpr (kNdebug) {
         LogAndSave(
             fmt::format("Application: Application version {}.", ProjectInfo::versionString));
      }
      else {
         LogAndSave(
             fmt::format("Application: Application version {}-debug.", ProjectInfo::versionString));
      }
      // ReSharper restore CppUnreachableCode
      LogAndSave(fmt::format("Application: Application path {}.",
          juce::File::getSpecialLocation(juce::File::currentApplicationFile)
              .getFullPathName()
              .toStdString()));
      LogAndSave(
          fmt::format("Application: Profile directory {}.", profile_directory.toStdString()));
      LogAndSave(fmt::format(
          "Application: Log file directory {}.", ww898::utf::conv<char>(rsj::AppLogFilePath(""))));
      LogAndSave(fmt::format("Application: Settings file directory {}.",
          ww898::utf::conv<char>(rsj::AppDataFilePath(""))));
#ifdef _WIN32
      /* MacOS defers keyboard layout information until first keystroke sent */
      LogAndSave(fmt::format("Application: Keyboard type {}.", GetKeyboardLayout()));
#endif
   }
   catch (...) {
      try {
         static constexpr auto kErr {"Failed to obtain app info. Exception."};
         info_.emplace_back(kErr);
         rsj::Log(kErr);
      }
      catch (...) { //-V565
      }
   }
}
#pragma warning(pop)
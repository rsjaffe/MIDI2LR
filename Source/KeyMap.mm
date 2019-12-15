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

 /* the following is adapted from https://github.com/microsoft/node-native-keymap/blob/master/src/keyboard_mac.mm */
#include "Ocpp.h"
#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>
#include <cctype>
#include <optional>
#include <JuceLibraryCode/JuceHeader.h>

namespace {
   std::optional<std::pair<bool, UniChar>> ConvertKeyCodeToText(
       const UCKeyboardLayout* keyboardLayout, UInt16 virtual_key_code, UInt32 modifiers)
   {
      /* Note that modifiers are the eventrecord modifiers >> 8 & 0xFF. So shift is 0x2.*/
      UInt32 dead_key_state = 0;
      UniCharCount char_count = 0;
      UniChar character = 0;
      OSStatus status = UCKeyTranslate(keyboardLayout, virtual_key_code, kUCKeyActionDown,
          modifiers, LMGetKbdLast(), kUCKeyTranslateNoDeadKeysBit, &dead_key_state, 1, &char_count,
          &character);

      bool isDeadKey = false;
      if (status == noErr && char_count == 0 && dead_key_state != 0) {
         isDeadKey = true;
         status = UCKeyTranslate(keyboardLayout, virtual_key_code, kUCKeyActionDown, modifiers,
             LMGetKbdLast(), kUCKeyTranslateNoDeadKeysBit, &dead_key_state, 1, &char_count,
             &character);
      }

      if (status == noErr && char_count == 1 && !std::iscntrl(character))
         return std::make_pair(isDeadKey, character);
      return {};
   }
} // namespace

std::unordered_map<UniChar, std::pair<size_t, bool>> rsj::GetKeyMap()
{
   std::unordered_map<UniChar, std::pair<size_t, bool>> KeyMap;

   const juce::MessageManagerLock mmLock;

   TISInputSourceRef source = TISCopyCurrentKeyboardInputSource();
   CFDataRef layout_data = static_cast<CFDataRef>(
       (TISGetInputSourceProperty(source, kTISPropertyUnicodeKeyLayoutData)));
   if (!layout_data) {
      /* TISGetInputSourceProperty returns null with  Japanese keyboard layout.
       * Using TISCopyCurrentKeyboardLayoutInputSource to fix NULL return. */
      source = TISCopyCurrentKeyboardLayoutInputSource();
      layout_data = static_cast<CFDataRef>(
          (TISGetInputSourceProperty(source, kTISPropertyUnicodeKeyLayoutData)));
      if (!layout_data) {
         // https://developer.apple.com/library/mac/documentation/TextFonts/Reference/TextInputSourcesReference/#//apple_ref/c/func/TISGetInputSourceProperty
         return KeyMap;
      }
   }

   const UCKeyboardLayout* keyboardLayout =
       reinterpret_cast<const UCKeyboardLayout*>(CFDataGetBytePtr(layout_data));

   for (UInt16 native_keycode = 0; native_keycode <= 0xFF; ++native_keycode) {
      if (auto value = ConvertKeyCodeToText(keyboardLayout, native_keycode, 0))
         KeyMap.try_emplace(value->second, std::make_pair(native_keycode, false));
      if (auto value = ConvertKeyCodeToText(keyboardLayout, native_keycode, 2))
         KeyMap.try_emplace(value->second, std::make_pair(native_keycode, true));
   }
   return KeyMap;
}

std::string rsj::GetKeyboardLayout()
{
   std::string result;
   const juce::MessageManagerLock mmLock;

   TISInputSourceRef source = TISCopyCurrentKeyboardInputSource();
   CFStringRef sourceId = (CFStringRef)TISGetInputSourceProperty(source, kTISPropertyInputSourceID);
   if (sourceId)
      result = "Id = " + std::string(((NSString*)sourceId).UTF8String);

   TISInputSourceRef nameSource = TISCopyCurrentKeyboardInputSource();
   CFStringRef localizedName =
       (CFStringRef)TISGetInputSourceProperty(nameSource, kTISPropertyLocalizedName);
   if (localizedName)
      result += ". Localized name = " + std::string(((NSString*)localizedName).UTF8String);

   NSArray* languages =
       (NSArray*)TISGetInputSourceProperty(source, kTISPropertyInputSourceLanguages);
   if (languages && languages.count > 0) {
      NSString* lang = languages[0];
      if (lang)
         result += ". Language = " + std::string(lang.UTF8String);
   }
   return result + ".";
}

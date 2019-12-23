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
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <JuceLibraryCode/JuceHeader.h>
#include "Misc.h"

/* note that, in MacOS Catalina, TISGetInputSourceProperty() must execute on the main thread, or crash */

namespace {
   std::optional<std::pair<bool, UniChar>> ConvertKeyCodeToText(
       const UCKeyboardLayout* keyboard_layout, UInt16 virtual_key_code, UInt32 modifiers)
   {
      /* Note that modifiers are the eventrecord modifiers >> 8 & 0xFF. So shift is 0x2.*/
      UInt32 dead_key_state {0};
      UniCharCount char_count {0};
      UniChar character {0};
      OSStatus status {UCKeyTranslate(keyboard_layout, virtual_key_code, kUCKeyActionDown,
          modifiers, LMGetKbdLast(), kUCKeyTranslateNoDeadKeysBit, &dead_key_state, 1, &char_count,
          &character)};
      bool dead_key {false};
      if (status == noErr && char_count == 0 && dead_key_state != 0) {
         dead_key = true;
         status = UCKeyTranslate(keyboard_layout, virtual_key_code, kUCKeyActionDown, modifiers,
             LMGetKbdLast(), kUCKeyTranslateNoDeadKeysBit, &dead_key_state, 1, &char_count,
             &character);
      }
      if (status == noErr && char_count == 1 && !std::iscntrl(character))
         return std::make_pair(dead_key, character);
      return {};
   }

   std::shared_mutex mtx; /* since following are filled in in Message Loop asynchronously */
   std::string source_id_string {};
   std::string localized_name_string {};
   std::string lang_string {};
   std::unordered_map<UniChar, std::pair<size_t, bool>> key_map {};
} // namespace

void rsj::FillInMessageLoop()
{
   std::unique_lock lock {mtx};
   TISInputSourceRef source {TISCopyCurrentKeyboardInputSource()};
   if (!source)
      source = TISCopyCurrentKeyboardLayoutInputSource();
   if (!source) {
      rsj::LogAndAlertError(juce::translate("Invalid keyboard layout handle."),
          "Invalid keyboard layout handle. No source.");
      return;
   }
   CFStringRef source_id {
       (CFStringRef)TISGetInputSourceProperty(source, kTISPropertyInputSourceID)};
   if (source_id)
      source_id_string = std::string(((NSString*)source_id).UTF8String);

   CFStringRef localized_name {
       (CFStringRef)TISGetInputSourceProperty(source, kTISPropertyLocalizedName)};
   if (localized_name)
      localized_name_string = std::string(((NSString*)localized_name).UTF8String);

   NSArray* languages {
       (NSArray*)TISGetInputSourceProperty(source, kTISPropertyInputSourceLanguages)};
   if (languages && languages.count > 0) {
      NSString* lang {languages[0]};
      if (lang)
         lang_string = std::string(lang.UTF8String);
   }

   /* get layout data here since it's before use is needed and it crashes if done on another thread
    * since MacOS Catalina. it must happen on the main message thread */
   CFDataRef layout_data {static_cast<CFDataRef>(
       (TISGetInputSourceProperty(source, kTISPropertyUnicodeKeyLayoutData)))};
   if (!layout_data) {
      // TISGetInputSourceProperty returns null with  Japanese keyboard layout.
      // Using TISCopyCurrentKeyboardLayoutInputSource to fix NULL return.
      TISInputSourceRef source_a {TISCopyCurrentKeyboardLayoutInputSource()};
      if (source_a) {
         layout_data = static_cast<CFDataRef>(
             (TISGetInputSourceProperty(source_a, kTISPropertyUnicodeKeyLayoutData)));
         CFRelease(source_a);
      }
      if (!layout_data) {
         rsj::LogAndAlertError(juce::translate("Invalid keyboard layout handle."),
             "Invalid keyboard layout handle. No layout_data.");
         CFRelease(source);
         return;
      }
   }
   const UCKeyboardLayout* keyboard_layout {
       reinterpret_cast<const UCKeyboardLayout*>(CFDataGetBytePtr(layout_data))};
   if (!keyboard_layout)
      rsj::LogAndAlertError(juce::translate("Invalid keyboard layout handle."),
          "Invalid keyboard layout handle. No keyboard_layout.");
   else
      for (UInt16 native_keycode {0}; native_keycode <= 0xFF; ++native_keycode) {
         if (auto value {ConvertKeyCodeToText(keyboard_layout, native_keycode, 0)})
            key_map.try_emplace(value->second, std::make_pair(native_keycode, false));
         if (auto value {ConvertKeyCodeToText(keyboard_layout, native_keycode, 2)})
            key_map.try_emplace(value->second, std::make_pair(native_keycode, true));
      }
   CFRelease(source);
}

std::unordered_map<UniChar, std::pair<size_t, bool>> rsj::GetKeyMap()
{
   std::shared_lock lock {mtx};
   return key_map;
}

std::string rsj::GetKeyboardLayout()
{
   std::shared_lock lock {mtx};
   std::string result {};
   if (!source_id_string.empty())
      result = "Id " + source_id_string;
   if (!localized_name_string.empty())
      result += ". Localized name = " + localized_name_string;
   if (!lang_string.empty())
      result += ". Language = " + lang_string;
   return result;
}
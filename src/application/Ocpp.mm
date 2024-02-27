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
#include "Ocpp.h"

#include <cstring>

#import <Cocoa/Cocoa.h>
#include <fmt/format.h>

#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "Misc.h"

std::string rsj::AppDataMac()
{
   const auto result {(@"~/Library/Application Support").stringByExpandingTildeInPath};
   return result.UTF8String;
}

std::string rsj::AppLogMac()
{
   const auto result {(@"~/Library/Logs").stringByExpandingTildeInPath};
   return result.UTF8String;
}

void rsj::CheckPermission(pid_t pid)
{
   AEAddressDesc addressDesc;
   const auto bundleIdentifier {
       [NSRunningApplication runningApplicationWithProcessIdentifier:pid].bundleIdentifier};
   const auto bundleIdentifierCString {
       [bundleIdentifier cStringUsingEncoding:NSUTF8StringEncoding]};
   const auto aeresult {AECreateDesc(typeApplicationBundleID, bundleIdentifierCString,
       std::strlen(bundleIdentifierCString), &addressDesc)};
   if (aeresult == noErr) {
      const auto status {
          AEDeterminePermissionToAutomateTarget(&addressDesc, typeWildCard, typeWildCard, true)};
      AEDisposeDesc(&addressDesc);
      switch (status) {
      case errAEEventWouldRequireUserConsent:
         rsj::Log(fmt::format(FMT_STRING("Automation permission pending for {}."),
                      bundleIdentifierCString),
             std::source_location::current());
         break;
      case noErr:
         rsj::Log(fmt::format(FMT_STRING("Automation permission granted for {}."),
                      bundleIdentifierCString),
             std::source_location::current());
         break;
      case errAEEventNotPermitted:
         {
            rsj::Log(fmt::format(FMT_STRING("Automation permission denied for {}."),
                         bundleIdentifierCString),
                std::source_location::current());
            const auto title {juce::translate("MIDI2LR needs your authorization to send keystrokes "
                                              "to Lightroom")};
            const auto message {juce::translate(
                "To authorize MIDI2LR to send keystrokes to Lightroom, please follow these "
                "steps:\r\n1) Open System Preferences\r\n2) Open Accessibility preferences \r\n3) "
                "Select \"Accessibility Apps\"\r\n4) Add this application to the approval list")};
            juce::NativeMessageBox::showMessageBox(juce::AlertWindow::WarningIcon, title, message);
            break;
         }
      case procNotFound:
         rsj::Log(fmt::format(FMT_STRING("Application not found. Automation permission unknown for "
                                         "{}."),
                      bundleIdentifierCString),
             std::source_location::current());
         break;
      default:
         rsj::Log(fmt::format(FMT_STRING("Unexpected return value when checking automation "
                                         "permission for {}."),
                      bundleIdentifierCString),
             std::source_location::current());
         break;
      }
   }
   else {
      rsj::Log(fmt::format(FMT_STRING("AECreateDesc returned error {}."), aeresult),
          std::source_location::current());
   }
}
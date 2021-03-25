#ifndef MIDI2LR_DEVICES_H
#define MIDI2LR_DEVICES_H
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
#include <map>
#include <memory>
#include <utility>
#include <version>

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_core/juce_core.h>

#ifdef __cpp_lib_three_way_comparison
#include <compare>
#endif

class Devices {
 public:
   Devices();
   ~Devices();
   Devices(const Devices& other) = delete;
   Devices(Devices&& other) noexcept = default;
   Devices& operator=(const Devices& other) = delete;
   Devices& operator=(Devices&& other) noexcept = default;
   bool Add(const juce::MidiDeviceInfo& info, const juce::String& io);
   [[nodiscard]] bool Enabled(const juce::MidiDeviceInfo& info, juce::String io) const;
   [[nodiscard]] bool EnabledOrNew(const juce::MidiDeviceInfo& info, const juce::String& io);

 private:
   struct DevInfo {
#ifdef __cpp_lib_three_way_comparison
      std::strong_ordering operator<=>(const DevInfo& rhs) const noexcept
      {
         if (const auto a = name.compare(rhs.name)) { return a <=> 0; }
         if (const auto a = identifier.compare(rhs.identifier)) { return a <=> 0; }
         const auto a = i_o.compare(rhs.i_o);
         return a <=> 0;
      }
#else
      friend bool operator==(const DevInfo& lhs, const DevInfo& rhs) noexcept
      {
         return lhs.name == rhs.name && lhs.identifier == rhs.identifier && lhs.i_o == rhs.i_o;
      }
      friend bool operator!=(const DevInfo& lhs, const DevInfo& rhs) noexcept
      {
         return !(lhs == rhs);
      }
      friend bool operator<(const DevInfo& lhs, const DevInfo& rhs) noexcept
      {
         if (lhs.name < rhs.name) { return true; }
         if (rhs.name < lhs.name) { return false; }
         if (lhs.identifier < rhs.identifier) { return true; }
         if (rhs.identifier < lhs.identifier) { return false; }
         return lhs.i_o < rhs.i_o;
      }
      friend bool operator<=(const DevInfo& lhs, const DevInfo& rhs) noexcept
      {
         return !(rhs < lhs);
      }
      friend bool operator>(const DevInfo& lhs, const DevInfo& rhs) noexcept { return rhs < lhs; }
      friend bool operator>=(const DevInfo& lhs, const DevInfo& rhs) noexcept
      {
         return !(lhs < rhs);
      }
#endif

      DevInfo(const juce::MidiDeviceInfo& info, juce::String io) noexcept
          : name {info.name}, identifier {info.identifier}, i_o {std::move(io)}
      {
      }
      DevInfo(juce::String n, juce::String i, juce::String io) noexcept
          : name {std::move(n)}, identifier {std::move(i)}, i_o {std::move(io)}
      {
      }
      juce::String name;
      juce::String identifier;
      juce::String i_o;
   };
   std::map<DevInfo, bool> device_listing_;
   std::unique_ptr<juce::XmlElement> device_xml_;
   juce::XmlElement* column_list_ {nullptr};
   juce::XmlElement* data_list_ {nullptr};
   int num_rows_ {0};
};
#endif

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

#include "Devices.h"

#include <algorithm>
#include <exception>

#include "Misc.h"

Devices::Devices()
{
   try {
      /* open file with xml list of devices */
#ifdef _WIN32
      const juce::File source {
          juce::CharPointer_UTF16(rsj::AppDataFilePath("DisabledControllers.xml").data())};
#else
      const juce::File source {
          juce::CharPointer_UTF8(rsj::AppDataFilePath("DisabledControllers.xml").data())};
#endif

      if (source.exists())
         device_xml_ = juce::parseXML(source);
   }
   catch (const std::exception& e) { /* log and carry on */
      MIDI2LR_E_RESPONSE;
      device_xml_.reset();
   }
   if (!device_xml_)
      device_xml_ = juce::parseXML(R"===(<?xml version="1.0" encoding="UTF-8"?>
<table_data>
  <heading>
    <column columnId="1" name="devicename" label="device name" width="200"/>
    <column columnId="2" name="systemid" label="system id" width="200"/>
    <column columnId="3" name="inputoutput" label="input/output" width="50"/>
    <column columnId="9" name="active" label="active" width="50"/>
  </heading>
  <data>
  </data>
</table_data>)===");
   column_list_ = device_xml_->getChildByName("heading");
   data_list_ = device_xml_->getChildByName("data");

   if (data_list_) {
      num_rows_ = data_list_->getNumChildElements();
      forEachXmlChildElement(*data_list_, data_element)
          device_listing_.emplace(DevInfo {data_element->getStringAttribute("devicename"),
                                      data_element->getStringAttribute("systemid"),
                                      data_element->getStringAttribute("inputoutput")},
              data_element->getIntAttribute("active"));
   }
   else
      data_list_ = device_xml_->createNewChildElement("data");
}

Devices::~Devices()
{
   try {
      /* open file with xml list of devices */
#ifdef _WIN32
      const juce::File source {
          juce::CharPointer_UTF16(rsj::AppDataFilePath("DisabledControllers.xml").data())};
#else
      const juce::File source {
          juce::CharPointer_UTF8(rsj::AppDataFilePath("DisabledControllers.xml").data())};
#endif
      // ReSharper disable once CppExpressionWithoutSideEffects
      device_xml_->writeTo(source);
   }
   catch (const std::exception& e) {
      rsj::LogAndAlertError(juce::String("Exception in ~Devices: ") + e.what());
   }
   catch (...) {
      rsj::LogAndAlertError("Non-standard exception in ~Devices.");
   }
}

bool Devices::Add(const juce::MidiDeviceInfo& info, const juce::String& io)
{
   try {
      const auto [it, success] {device_listing_.try_emplace({info, io}, true)};
      if (success) {
         auto new_element {data_list_->createNewChildElement("item")};
         if (new_element) {
            new_element->setAttribute("devicename", info.name);
            new_element->setAttribute("systemid", info.identifier);
            new_element->setAttribute("inputoutput", io);
            new_element->setAttribute("active", "1");
         }
         else {
            rsj::Log("Failed to create new element in Devices::Add");
         }
      }
      return success;
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

bool Devices::Enabled(const juce::MidiDeviceInfo& info, juce::String io) const
{
   try {
      const auto it {device_listing_.find({info, std::move(io)})};
      if (it == device_listing_.end())
         return true;
      return it->second;
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

bool Devices::EnabledOrNew(const juce::MidiDeviceInfo& info, const juce::String& io)
{
   try {
      if (Add(info, io))
         return true;
      return Enabled(info, io);
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}
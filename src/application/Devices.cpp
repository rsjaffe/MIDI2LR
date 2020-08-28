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
#include <vector>

#include "Misc.h"

Devices::Devices()
{
   // open file with xml list of devices
#ifdef _WIN32
   const juce::File source {
       juce::CharPointer_UTF16(rsj::AppDataFilePath("DisabledControllers.xml").data())};
#else
   const juce::File source {
       juce::CharPointer_UTF8(rsj::AppDataFilePath("DisabledControllers.xml").data())};
#endif

   if (source.exists())
      device_xml_ = juce::XmlDocument::parse(source);
   else
      device_xml_ = juce::XmlDocument::parse(R"===(<?xml version="1.0" encoding="UTF-8"?>
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
      forEachXmlChildElement (*data_list_, data_element)
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
   // open file with xml list of devices
#ifdef _WIN32
   const juce::File source {
       juce::CharPointer_UTF16(rsj::AppDataFilePath("DisabledControllers.xml").data())};
#else
   const juce::File source {
       juce::CharPointer_UTF8(rsj::AppDataFilePath("DisabledControllers.xml").data())};
#endif
   device_xml_->writeTo(source);
}

void Devices::Add(const juce::MidiDeviceInfo& info, juce::String io)
{
   const auto [it, success] {device_listing_.try_emplace({info, io}, true)};
   if (success) {
      auto new_element {data_list_->createNewChildElement("item")};
      new_element->setAttribute("devicename", info.name);
      new_element->setAttribute("systemid", info.identifier);
      new_element->setAttribute("inputoutput", io);
      new_element->setAttribute("active", "1");
   }
}

bool Devices::Enabled(const juce::MidiDeviceInfo& info, juce::String io) const
{
   const auto it {device_listing_.find({info, std::move(io)})};
   if (it == device_listing_.end())
      return true;
   return it->second;
}

bool Devices::EnabledOrNew(const juce::MidiDeviceInfo& info, juce::String io)
{
   const auto [it, success] {device_listing_.try_emplace({info, io}, true)};
   if (success) {
      auto new_element {data_list_->createNewChildElement("item")};
      new_element->setAttribute("devicename", info.name);
      new_element->setAttribute("systemid", info.identifier);
      new_element->setAttribute("inputoutput", io);
      new_element->setAttribute("active", "1");
      return true;
   }
   const auto found {device_listing_.find({info, io})};
   if (found == device_listing_.end()) {
      rsj::Log(fmt::format("EnabledOrNew unable to emplace, but can't find device {} {} {}.",
          info.name.toStdString(), info.identifier.toStdString(), io.toStdString()));
      return true;
   }
   return found->second;
}
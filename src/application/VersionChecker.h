#ifndef MIDI2LR_VERSIONCHECKER_H_INCLUDED
#define MIDI2LR_VERSIONCHECKER_H_INCLUDED
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
#include <atomic>
#include <future>

#include <juce_events/juce_events.h>

class SettingsManager;

class VersionChecker final : juce::AsyncUpdater {
 public:
   explicit VersionChecker(SettingsManager& settings_manager) noexcept;
   ~VersionChecker() = default; // NOLINT(modernize-use-override)
   VersionChecker(const VersionChecker& other) = delete;
   VersionChecker(VersionChecker&& other) = delete;
   VersionChecker& operator=(const VersionChecker& other) = delete;
   VersionChecker& operator=(VersionChecker&& other) = delete;
   void Start();
   void Stop() noexcept;

 private:
   void handleAsyncUpdate() override;
   void Run();

   int new_version_ {0};
   SettingsManager& settings_manager_;
   std::atomic<bool> thread_should_exit_ {false};
   std::future<void> run_future_;
};

#endif

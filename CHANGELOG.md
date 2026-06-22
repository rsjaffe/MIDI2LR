# Changelog — DanyelAndre/MIDI2LR

This document describes changes relative to the upstream project [rsjaffe/MIDI2LR](https://github.com/rsjaffe/MIDI2LR).

---

## [Unreleased]

### Fixed

#### White Balance presets revert to "Custom" after 1–2 seconds
**File:** `src/plugin/Client.lua`  
**PR:** [rsjaffe/MIDI2LR#872](https://github.com/rsjaffe/MIDI2LR/pull/872)  
**Issue:** [rsjaffe/MIDI2LR#871](https://github.com/rsjaffe/MIDI2LR/issues/871)

**Problem:**  
Triggering a White Balance preset (e.g. Cloudy) via MIDI2LR initially applied the
correct setting, but it reverted to "Custom" with slightly altered temperature values
after 1–2 seconds. Applying the same preset through a native Lightroom Develop Preset
worked correctly.

**Root cause:**  
After a WB preset is applied, the `AdjustmentChangeObserver` detects the resulting
`Temperature` and `Tint` changes and sends those numeric values back to the MIDI
controller. In setups using an IAC Driver (e.g. Stream Deck + MIDI Button plugin),
these outgoing MIDI messages loop back as incoming messages. MIDI2LR then receives
`Temperature = <value>` and calls `setValue('Temperature', ...)`, which causes
Lightroom to reset `WhiteBalance` to "Custom".

**Fix:**  
All WhiteBalance actions now record a timestamp (`whiteBalancePresetTime`) after
executing. Both `UpdateParamPickup` and `UpdateParamNoPickup` ignore incoming
`Temperature` and `Tint` values for 1 second after that timestamp, breaking the
feedback loop without affecting normal Temperature/Tint control.

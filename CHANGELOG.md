# Changelog — DanyelAndre/MIDI2LR

Dieses Dokument beschreibt Änderungen gegenüber dem upstream-Projekt [rsjaffe/MIDI2LR](https://github.com/rsjaffe/MIDI2LR).

---

## [Unreleased]

### Behoben

#### White Balance Preset kehrt nach 1–2 Sekunden zu „Custom" zurück
**Datei:** `src/plugin/Client.lua`  
**PR:** [rsjaffe/MIDI2LR#872](https://github.com/rsjaffe/MIDI2LR/pull/872)  
**Issue:** [rsjaffe/MIDI2LR#871](https://github.com/rsjaffe/MIDI2LR/issues/871)

**Problem:**  
Beim Auslösen eines White-Balance-Presets (z. B. „Bewölkt") über MIDI2LR wurde der Wert
nach 1–2 Sekunden automatisch auf „Benutzerdefiniert" zurückgesetzt, obwohl derselbe
Preset über ein natives Lightroom-Develop-Preset stabil blieb.

**Ursache:**  
Der `AdjustmentChangeObserver` sendet nach dem Setzen eines WB-Presets die neuen
`Temperature`- und `Tint`-Werte an den MIDI-Controller zurück. In Setups mit IAC-Driver
(z. B. Stream Deck + MIDI-Button-Plugin) gelangen diese Werte als eingehende MIDI-Nachrichten
zurück zu MIDI2LR. Dieses ruft dann `setValue('Temperature', …)` auf, was Lightroom dazu
veranlasst, `WhiteBalance` auf „Custom" zu setzen.

**Lösung:**  
Alle WhiteBalance-Aktionen setzen nach ihrer Ausführung einen Zeitstempel
(`whiteBalancePresetTime`). In `UpdateParamPickup` und `UpdateParamNoPickup` werden
eingehende `Temperature`- und `Tint`-Werte für 1 Sekunde danach ignoriert. Das unterbricht
den Feedback-Loop, ohne den normalen Betrieb von Temperature/Tint-Reglern zu beeinflussen.

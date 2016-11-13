iCon QCon Pro USB DAW Controller
https://www.icondigitalusa.com/daw-controllers/

Midi out messages recorded using MIDI-OX.

=================================================
Nuendo mode, Cubase mode, Reason Mode, Bitwig
=================================================

Encoders

| TIMESTAMP | IN | PORT | STATUS | DATA1 | DATA2 | CHAN | NOTE | EVENT |
| --------- | --:| ----:| ------:| -----:| -----:| ----:|:----:|:-----:|
| 0001B339  |  1 |    5 |     B0 |    17 |    41 |    1 |  --- | CC: 23 (E-MU) |

CW: 1 - 9
CCW: 41 - 49

Buttons

| TIMESTAMP | IN | PORT | STATUS | DATA1 | DATA2 | CHAN | NOTE | EVENT |
| --------- | --:| ----:| ------:| -----:| -----:| ----:|:----:|:-----:|
|  00035052 |  1 |    6 |     90 |    0F |    7F |    1 | Eb 0 | Note On |
|  000350AF |  1 |    5 |     90 |    0F |    00 |    1 | Eb 0 | Note Off |

Faders

| TIMESTAMP | IN | PORT | STATUS | DATA1 | DATA2 | CHAN | NOTE | EVENT |
| --------- | --:| ----:| ------:| -----:| -----:| ----:|:----:|:-----:|
|  000AAA3E |  1 |    5 |     E7 |    40 |    7D |    8 |  --- | Pitch Bend |
|  000AAA3E |  1 |    5 |     E7 |    00 |    7E |    8 |  --- | Pitch Bend |
|  000AAA3E |  1 |    5 |     E7 |    40 |    7E |    8 |  --- | Pitch Bend |
|  000AAA3E |  1 |    5 |     E7 |    00 |    7F |    8 |  --- | Pitch Bend |
|  000AAA3E |  1 |    5 |     E7 |    40 |    7F |    8 |  --- | Pitch Bend |
|  000AAA51 |  1 |    5 |     E7 |    71 |    7F |    8 |  --- | Pitch Bend (max value) |
|  0008EB5A |  1 |    5 |     90 |    6F |    7F |    1 | Eb 8 | Note On |
|  0008EC30 |  1 |    6 |     90 |    6F |    00 |    1 | Eb 8 | Note Off |

Jog

| TIMESTAMP | IN | PORT | STATUS | DATA1 | DATA2 | CHAN | NOTE | EVENT |
| --------- | --:| ----:| ------:| -----:| -----:| ----:|:----:|:-----:|
|  000D3BE9 |  1 |    6 |     B0 |    3C |    45 |    1 |  --- | Control Change |
|  000D41C1 |  1 |    5 |     B0 |    3C |    01 |    1 |  --- | Control Change |

CW: 1 - 9
CCW: 41 - 49


=================================================
Logic Pro mode, Samplitude mode - seems to have higher fader resolution
=================================================

Encoders, Buttons and Jog see above

Faders

| TIMESTAMP | IN | PORT | STATUS | DATA1 | DATA2 | CHAN | NOTE | EVENT |
| --------- | --:| ----:| ------:| -----:| -----:| ----:|:----:|:-----:|
|  00015A14 |  1 |    5 |     E7 |    5E |    71 |    8 |  --- | Pitch Bend |
|  00015A91 |  1 |    5 |     E7 |    18 |    72 |    8 |  --- | Pitch Bend |
|  00015A91 |  1 |    5 |     E7 |    52 |    72 |    8 |  --- | Pitch Bend |
|  00015C12 |  1 |    5 |     E7 |    0C |    73 |    8 |  --- | Pitch Bend
|  00015D62 |  1 |    6 |     E7 |    7B |    73 |    8 |  --- | Pitch Bend (max value logic pro) |
|  00012216 |  1 |    6 |     E7 |    44 |    77 |    8 |  --- | Pitch Bend (max value samplitude) |


=================================================
Live mode
=================================================

Encoders and Buttons see above

Faders

| TIMESTAMP | IN | PORT | STATUS | DATA1 | DATA2 | CHAN | NOTE | EVENT |
| --------- | --:| ----:| ------:| -----:| -----:| ----:|:----:|:-----:|
|  0000DF60 |  1 |    6 |     E7 |    7F |    7F |    8 |  --- | Pitch Bend (max value Live mode) |

Jog (only ONE step CW and CCW)

| TIMESTAMP | IN | PORT | STATUS | DATA1 | DATA2 | CHAN | NOTE | EVENT |
| --------- | --:| ----:| ------:| -----:| -----:| ----:|:----:|:-----:|
|  00017205 |  1 |    6 |     B0 |    3C |    08 |    1 |  --- | Control Change |
|  00017762 |  1 |    5 |     B0 |    3C |    48 |    1 |  --- | Control Change |


=================================================
Pro Tools
=================================================

Encoders

| TIMESTAMP | IN | PORT | STATUS | DATA1 | DATA2 | CHAN | NOTE | EVENT |
| --------- | --:| ----:| ------:| -----:| -----:| ----:|:----:|:-----:|
|  0000270B |  1 |    6 |     B0 |    47 |    01 |    1 |  --- | CC: Harmonic Content |
|  00002AB1 |  1 |    5 |     B0 |    47 |    41 |    1 |  --- | CC: Harmonic Content |

CW: 1 - 9
CCW: 41 - 49

Buttons

| TIMESTAMP | IN | PORT | STATUS | DATA1 | DATA2 | CHAN | NOTE | EVENT |
| --------- | --:| ----:| ------:| -----:| -----:| ----:|:----:|:-----:|
|  0002B23C |  1 |    5 |     B0 |    0F |    07 |    1 |  --- | Control Change |
|  0002B240 |  1 |    6 |     B0 |    2F |    47 |    1 |  --- | Control Change |
|  0002B244 |  1 |    6 |     B0 |    0F |    07 |    1 |  --- | Control Change |
|  0002B248 |  1 |    6 |     B0 |    2F |    07 |    1 |  --- | Control Change |

Faders 7-bit 0-127

| TIMESTAMP | IN | PORT | STATUS | DATA1 | DATA2 | CHAN | NOTE | EVENT |
| --------- | --:| ----:| ------:| -----:| -----:| ----:|:----:|:-----:|
|  00044B72 |  1 |    6 |     B0 |    07 |    00 |    1 |  --- | CC: | Volume (min value Pro Tools mode) |
|  000501C4 |  1 |    6 |     B0 |    07 |    7C |    1 |  --- | CC: | Volume |
|  00050256 |  1 |    6 |     B0 |    07 |    7D |    1 |  --- | CC: | Volume |
|  0005029C |  1 |    6 |     B0 |    07 |    7E |    1 |  --- | CC: | Volume |
|  000502B0 |  1 |    5 |     B0 |    07 |    7F |    1 |  --- | CC: | Volume (max value Pro Tools mode) |

Jog

| TIMESTAMP | IN | PORT | STATUS | DATA1 | DATA2 | CHAN | NOTE | EVENT |
| --------- | --:| ----:| ------:| -----:| -----:| ----:|:----:|:-----:|
|  0006E4FC |  1 |    6 |     B0 |    0D |    01 |    1 |  --- | Control Change |
|  0006F291 |  1 |    5 |     B0 |    0D |    41 |    1 |  --- | Control Change |

CW: 1 - 9
CCW: 41 - 49


=================================================
Reaper Mode, Studio one mode
=================================================

Encoders (only ONE step CW and CCW)

| TIMESTAMP | IN | PORT | STATUS | DATA1 | DATA2 | CHAN | NOTE | EVENT |
| --------- | --:| ----:| ------:| -----:| -----:| ----:|:----:|:-----:|
|  00003B13 |  1 |    6 |     B0 |    17 |    01 |    1 |  --- | CC: 23 (E-MU) |
|  00003B94 |  1 |    5 |     B0 |    17 |    41 |    1 |  --- | CC: 23 (E-MU) |

Buttons and Faders see Cubase

Jog (only ONE step CW and CCW)

| TIMESTAMP | IN | PORT | STATUS | DATA1 | DATA2 | CHAN | NOTE | EVENT |
| --------- | --:| ----:| ------:| -----:| -----:| ----:|:----:|:-----:|
|  00032D8B |  1 |    6 |     B0 |    3C |    01 |    1 |  --- | Control Change |
|  00032E42 |  1 |    5 |     B0 |    3C |    41 |    1 |  --- | Control Change |


=================================================
MIDI Learn mode - all controllers work in ABSOLUTE MODE
=================================================

Encoders 7-bit 0-127

| TIMESTAMP | IN | PORT | STATUS | DATA1 | DATA2 | CHAN | NOTE | EVENT |
| --------- | --:| ----:| ------:| -----:| -----:| ----:|:----:|:-----:|
|  0000A1F8 |  1 |    6 |     B0 |    17 |    00 |    1 |  --- | CC: 23 (E-MU) |
|  0000A1F8 |  1 |    6 |     B0 |    17 |    7F |    1 |  --- | CC: 23 (E-MU) |

Buttons

| TIMESTAMP | IN | PORT | STATUS | DATA1 | DATA2 | CHAN | NOTE | EVENT |
| --------- | --:| ----:| ------:| -----:| -----:| ----:|:----:|:-----:|
|  00029831 |  1 |    6 |     90 |    07 |    7F |    1 | G -1 | Note On |
|  0002987F |  1 |    5 |     90 |    07 |    00 |    1 | G -1 | Note Off |

Faders 7-bit 0-127

| TIMESTAMP | IN | PORT | STATUS | DATA1 | DATA2 | CHAN | NOTE | EVENT |
| --------- | --:| ----:| ------:| -----:| -----:| ----:|:----:|:-----:|
|  0001B50A |  1 |    6 |     E7 |    00 |    00 |    8 |  --- | Pitch Bend |
|  0001B50A |  1 |    6 |     E7 |    00 |    7F |    8 |  --- | Pitch Bend |

Jog 7-bit 0-127

| TIMESTAMP | IN | PORT | STATUS | DATA1 | DATA2 | CHAN | NOTE | EVENT |
| --------- | --:| ----:| ------:| -----:| -----:| ----:|:----:|:-----:|
|  00038B66 |  1 |    6 |     B0 |    3C |    00 |    1 |  --- | Control Change |
|  00038B66 |  1 |    6 |     B0 |    3C |    7F |    1 |  --- | Control Change |


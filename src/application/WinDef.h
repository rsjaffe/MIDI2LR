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
#ifndef WIN32_LEAN_AND_MEAN
// ReSharper disable IdentifierTypo
#define NOATOM -Atom Manager routines
#define NOCLIPBOARD -Clipboard routines
#define NOCOLOR -Screen colors
#define NOCOMM -COMM driver routines
#define NOCTLMGR -Control and Dialog routines
#define NODEFERWINDOWPOS -DeferWindowPos routines
#define NODRAWTEXT -DrawText() and DT_*
#define NOGDI -All GDI defines and routines
#define NOGDICAPMASKS -CC_*, LC_*, PC_*, CP_*, TC_*, RC_
#define NOHELP -Help engine interface.
#define NOICONS -IDI_*
#define NOKANJI -Kanji support stuff.
#define NOKERNEL -All KERNEL defines and routines
#define NOKEYSTATES -MK_*
#define NOMB -MB_*and MessageBox()
#define NOMCX -Modem Configuration Extensions
#define NOMEMMGR -GMEM_*, LMEM_*, GHND, LHND, associated routines
#define NOMENUS -MF_*
#define NOMETAFILE -typedef METAFILEPICT
#define NOMINMAX -Macros min(a, b) and max(a, b)
#define NOMSG -typedef MSG and associated routines
#define NONLS -All NLS defines and routines
#define NOOPENFILE -OpenFile(), OemToAnsi, AnsiToOem, and OF_*
#define NOPROFILER -Profiler interface.
#define NORASTEROPS -Binary and Tertiary raster ops
#define NOSCROLL -SB_*and scrolling routines
#define NOSERVICE -All Service Controller routines, SERVICE_ equates, etc.
#define NOSHOWWINDOW -SW_*
#define NOSOUND -Sound driver routines
#define NOSYSCOMMANDS -SC_*
#define NOSYSMETRICS -SM_*
#define NOTEXTMETRIC -typedef TEXTMETRIC and associated routines
#define NOUSER -All USER defines and routines
#define NOVIRTUALKEYCODES -VK_*
#define NOWH -SetWindowsHook and WH_*
#define NOWINMESSAGES -WM_*, EM_*, LB_*, CB_*
#define NOWINOFFSETS -GWL_*, GCL_*, associated routines
#define NOWINSTYLES -WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
#define OEMRESOURCE -OEM Resource values
#define WIN32_LEAN_AND_MEAN
// ReSharper restore IdentifierTypo
#endif

Crispy Doom for AmigaOS 3.x
===========================

Crispy Doom 7.2 (upstream https://github.com/fabiangreffrath/crispy-doom,
commit fdeabe4c23a833534b70aa84a22f1685aae7125d, tag crispy-doom-7.2,
GPL-2.0-or-later) for AmigaOS 3.2 on a 68040 or 68060 with an FPU.

Status: builds and links; not yet run on an Amiga.


What you need
-------------

- AmigaOS 3.2 (or OS 3.2.3 with GFOpenUp), a 68040/68060 with an FPU, an
  RTG or AGA screen that SDL 2 can open (640x480 is used full screen).
- OpenGPU's runtime, which OpenUp installs: LIBS:OpenGPU/SDL2.module and
  LIBS:OpenGPU/SDL2_mixer.module (and opengpu.library). Without them the
  program says which file it needs and stops.
- A Doom IWAD: none comes with the game. See IWAD-ReadMe in the drawer.


Installing
----------

Unpack the archive anywhere (on AmiCradle: SYS:Utilities/OpenCompress x
crispydoom-...lha RAM:), then double-click Install-CrispyDoom beside the
CrispyDoom drawer: it checks for a 68040 with FPU and for SDL2.module and
SDL2_mixer.module (in LIBS:GFOpenGPU/, or copied there from
LIBS:OpenGPU/), asks where to put the game (Work:Games by default) and
updates an earlier install in place: IWADs put in the drawer and the User
drawer are left as they are.


Using it
--------

Put an IWAD in the CrispyDoom drawer and open crispy-doom's icon, or from
a Shell:

    cd Work:Games/CrispyDoom
    crispy-doom                    ; finds doom1.wad, doom2.wad, ... here
    crispy-doom -iwad Work:WADs/doom2.wad -file mymap.wad

An ARGS tool type on the program's icon (ARGS=-warp 1 1) is read as the
Shell's arguments. Configuration (crispy-doom.cfg, default.cfg), saved
games (User/savegames/IWADNAME/), music packs and autoload files go in the
User drawer beside the program, made on the first run. DOOMWADDIR and
DOOMWADPATH (directories separated by ';') are read from ENV:.

On the Amiga, Crispy Doom starts at the original 320x200 (4:3): a 68040
draws four times as many pixels at 640x400. Hires and widescreen are in the
Crispness menu (Options, Crispness), as on other systems.


How it is built: the route
--------------------------

The same as the Chocolate Doom port (DalsinAI/amigachocolatedoom), which
Crispy shares its i_video, i_sound, i_input and i_joystick layer with:
Crispy's own SDL 2 code, unchanged, linked with OpenGPU's SDL 2 developer
kit. libSDL2.a and libSDL2_mixer.a are the program's halves; the work is
done by SDL2.module and SDL2_mixer.module (screen, keyboard, mouse, pads
through GFOpenInput, AHI sound, and SDL_mixer for OPL and music packs).
The Chocolate port plays at Doom's 35 frames a second on AC090 with sound
and OPL music on that SDL 2, so no new video, sound or input code is
written for Crispy. (ACGame was the alternative: it would need a new
i_video, i_sound/music, i_input and timer layer, and the same OPL work.)

Build (GFOpenAmigaGCC 16.2.0-open4 or later):

    make -f amiga/Makefile.amigaos \
         STOVE=$HOME/CradleLab/toolchain/openamigagcc-16.2.0-open4-x86_64-linux \
         NDK=$HOME/CradleLab/gfopensdk/OpenNDK/Include_H \
         SDL2KIT=/mnt/amigachrome/team2/deps/opengpu-sdk \
         ZLIB=$HOME/CradleLab/zlib-1.3.1

Output: build/amigaos/CrispyDoom/ (crispy-doom, ReadMe, IWAD-ReadMe,
COPYING.txt and their icons) and build/amigaos/CrispyDoom.info.
amiga/config.h replaces the config.h CMake would make: 8-bit paletted (no
CRISPY_TRUECOLOR), SDL_net off (no network game), no FluidSynth,
libsamplerate or libpng (screenshots are PCX), zlib's inflate built in for
compressed ZDBSP nodes. Flags: -noixemul -m68040 -m68881 -O2 and GCC 16's
-fno-tree-loop-distribute-patterns; the objects pass openrtg's
fpcr_check.py.

Icons: amiga/icons/icons.json, from Crispy's own data/doom.png:

    python3 AMICRADLE/scripts/port_icons.py amiga/icons/icons.json --out amiga/icons


Changes to upstream (all under #ifdef __amigaos__)
--------------------------------------------------

Taken from the Chocolate Doom port's patches 0001, 0003 and 0004, which
applied to Crispy unchanged:

- src/i_main.c: a 512 KB stack (libnix swaps to it), a console window that
  opens only when something is printed, and a Workbench start's arguments
  (the icons' ARGS tool types; a WAD's project icon gives -iwad).
- src/m_misc.c, m_misc.h: AmigaDOS paths for every file the game opens
  through M_fopen, M_stat, M_remove, M_rename and M_MakeDirectory
  ("PROGDIR:/x" is the parent, "./" isn't the current directory); T: for
  temporary files; M_DirName knows "Vol:name".
- src/m_argv.c, src/d_iwad.c: PROGDIR: is the program's drawer; no Unix
  XDG/Steam directories.
- src/m_config.c: PROGDIR:User/ in place of SDL's preference path.
- src/i_joystick.c, src/m_controls.c: pads on by default, in SDL's game
  controller layout as GFOpenInput gives a CD32 pad.

Crispy's own:

- src/crispy.c: hires and widescreen off by default (above).
- src/doomtype.h: PATH_SEPARATOR is ';' (':' is part of AmigaDOS names).
- src/w_merge.c: -mergedump returned the byte-swapped lump count on a
  big-endian machine (it printed nonsense); the file was right.


Byte order
----------

The 68k is big-endian; WADs, MUS, DeHackEd binary data and saved demos are
little-endian. Crispy reads them through SHORT() and LONG() (src/i_swap.h,
SDL_SwapLE16/32). SDL_BYTEORDER is SDL_BIG_ENDIAN in the kit's
SDL_config_amigaos.h, so SYS_BIG_ENDIAN is defined and the macros swap
(sha1.c's own byte order check uses it too). Checked by reading the code
Crispy adds over Chocolate: extended nodes (DeePBSP, ZDBSP, XNOD/ZNOD in
p_extnodes.c, every field through SHORT/LONG), the blockmap and reject
padding, the WAV/DMX sound lump parser (bytes put together by hand), the
Ogg/FLAC music pack loop tags (LONG), the demo footer PWAD and -mergedump
(both written with LONG). Saved games are written a byte at a time. No
other unswapped reads were found; the w_merge.c fix above was the only
big-endian bug.


Not done / later
----------------

- Music: OPL (Doom's default "SB" music) is Crispy's software Nuked OPL3
  through SDL_mixer, as in the Chocolate port: it costs CPU on a real
  68040 (set snd_musicdevice 0 in User/default.cfg, or the music
  volume to 0, if the game slows). opl3.library (the real OPL3 on our
  hardware) is a later step. No FluidSynth / General MIDI.
- No network game (SDL_net off), no setup program (crispy-setup): the
  options are in the game's menus and in User/default.cfg and
  User/crispy-doom.cfg.
- Truecolor (CRISPY_TRUECOLOR) is off: Doom's 8-bit palette only.
- Heretic, Hexen and Strife are not built.


What to check first
-------------------

1. The program starts from a Shell with doom1.wad in the drawer: title,
   demo, menu; then from its icon.
2. E1M1 plays at 35 frames a second (type the cheat SHOWFPS or IDRATE in
   the game) at 320x200; then with hires on (Crispness menu).
3. Sound effects and OPL music through AHI; then with music off.
4. Save, quit, restart, load: User/savegames/doom1.wad/ appears.
5. A PWAD with ZDBSP compressed nodes (zlib) and one with DeePBSP nodes.

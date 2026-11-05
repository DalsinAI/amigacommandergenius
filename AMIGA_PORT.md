Commander Genius for AmigaOS 3.x
================================

Commander Genius 3.6.4 (the Commander Keen engine: Keen 1-6 and Keen
Dreams) on AmigaOS 3.x, 68040 or 68060 with an FPU, an RTG screen, and
OpenGPU's SDL 2 (LIBS:OpenGPU/SDL2.module with SDL2_image.module and
SDL2_mixer.module, which OpenUp installs).

Upstream: https://gitlab.com/Dringgstein/Commander-Genius, tag v3.6.4
(fb66a50d7b1fb3d2305f4fc727eaff27edb9f87b). GPL 2 (COPYRIGHT, here
COPYING.txt).


No game comes with it
---------------------

Commander Genius plays the Keen games' own DOS files, and none of them is
in this drawer: they are not ours to pass on. Games/ is empty; Games/ReadMe
says what to put there (one drawer per game, the DOS files as they are).
The repository has no free game or fan-made data of its own to bundle
(vfsroot/scripts holds only Lua/Python scripts for two mods, which need the
mods' own data, so they are left out).

With no game in Games/, the program starts, shows its launcher with an
empty game list and a message naming the drawer, and quits from there.


What is in the drawer (dist/CommanderGenius)
--------------------------------------------

  CommanderGenius      the program (512 KB stack in its icon and in the
                       program itself)
  global/              Commander Genius's own files (vfsroot/global: its
                       fonts, menu pictures, help text, trophy sound; GPL 2
                       with the program)
  Games/               empty, for the Keen games; Games/ReadMe
  ReadMe               this file
  README-CG.md         upstream's README
  COPYING.txt          the GPL 2 (upstream's COPYRIGHT)
  changelog.txt        upstream's change log

Settings (cgenius.cfg), the log (CGLog.html) and saves are written into the
drawer (PROGDIR:).


How to play
-----------

Put a game into Games/ (see Games/ReadMe), start CommanderGenius from its
icon or from a Shell, choose the game in the list and start it. Arrows move,
Ctrl jumps, Alt is the pogo, Esc is the menu (global/HELPTEXT.CKP, shown by
the game's help, has the rest); the keys can be set in the menu.

It opens a 640x400 window on the Workbench; the display settings in the
menu switch to fullscreen. The sound is 22 kHz mono (Settings, Audio, for
more: the AdLib music and the sounds are emulated on the 68k).


What was done
-------------

- amiga/Makefile.amigaos builds it without CMake: the sources upstream's
  CMakeLists.txt files list, with their per-directory definitions (RefKeen's
  REFKEEN_VER_KDREAMS..., unzip's IOAPI_NO_64), and Lua as C89.
  gamelauncher.menu is turned into a header with upstream's own xxd.c,
  built for the host.
- Cut: the downloader / game catalogue (DOWNLOADER, curl), OpenGL
  (USE_OPENGL, COpenGL.cpp), the on-screen virtual gamepad
  (USE_VIRTUALPAD), the crash handler (USE_CRASHHANDLER), the Cosmo engine
  (BUILD_COSMOS, a submodule) and the GP2X code. There is no networking in
  Commander Genius 3.6 besides the downloader, and no Python: its mods are
  scripted in Lua, which stays.
- Paths (amiga/amiga_paths.c): upstream joins its names the Unix way
  ("PROGDIR:" + "/" + "games", "./x", "a//b"); on AmigaDOS "PROGDIR:/x"
  is the drawer above. The program is linked with --wrap for fopen, open,
  stat, opendir, mkdir, remove, rename, chdir, access ... and SDL's
  SDL_RWFromFile, IMG_Load, TTF_OpenFont and Mix_LoadMUS, so that every name
  goes through amiga_path() first ("PROGDIR:games", "x", "a/b"; ".." as
  AmigaDOS's "/").
- GsKit/base/interface/FindFile: the one search path is PROGDIR:; home is
  PROGDIR:, the temporary drawer T:; GetExactFileName() needs no case
  search (AmigaDOS names are case-insensitive) and gives the AmigaDOS name.
- Big-endian (Keen's files are little-endian):
  - GsKit/base/TypeDefinitions.cpp: GETWORD/GETLONGWORD read the bytes as
    big-endian on a big-endian machine; they read little-endian now on all.
    Most of the game's readers (tiles, physics, sounds, maps) use them.
  - CExeFile: the MZ header, the music pointers (AUDIOHED inside the EXE
    or outside it) and the IMF sizes; Cunlzexe: the headers it reads and
    writes; crc.cpp: the EXE checksum as a PC computes it (the game checks
    it against PC values); CHuffman: the dictionary; CCarmack: its output
    words (CRLE then reads them byte by byte); CIMFPlayer: IMF sizes and
    delays; CPatcher: numbers patched into the EXE.
  - Keen 1-3: EGAHEAD (CEGAGraphicsVort), the sprite and bitmap tables
    (CEGASprit, CEGALatch); Keen 4-6: EGAHEAD offsets, chunk lengths, the
    bitmap, sprite and font headers, the "misc" pictures (CEGAGraphicsGalaxy,
    EGAStructs.h), AUDIOHED and chunk sizes (CAudioGalaxy), the action
    table (ActionFormat), level texts (CMessages), the song table
    (GalaxyEngine); Keen Dreams: MAPHEAD (dreamsengine; RefKeen itself is
    big-endian aware).
  - Single bytes read from the EXE with memcpy() into an int (CNospike,
    CMadMushroom, CCouncilMember, CTank, CPlayerWM): read as bytes.
  - The PC speaker sounds (CAudioResources): samples written as 8- or
    16-bit values, not as the first bytes of an int.
  - 16-bit sound: SDL's AUDIO_S16/AUDIO_U16 are the little-endian formats,
    but the mixers here make native samples: amiga/include/amiga_prelude.h
    makes them AUDIO_S16SYS/AUDIO_U16SYS.
- CGameLauncher: the "no games" message names the drawer as "the Games
  drawer beside CommanderGenius" (upstream printed the search path joined
  the Unix way, "PROGDIR:/games"; no "+ More" button), and starting with
  an empty list does nothing (upstream threw std::out_of_range).
- CGameLauncher: a game whose EXE version Commander Genius doesn't know
  (Keen 1 v1.3 shareware: KEEN1.EXE unpacks to 99998 bytes; the program
  knows 100274, 100484 and 100132, v1.10, v1.31 and v1.34) is refused at
  Start with a message naming the versions that work. Upstream started it
  anyway: with no version there is no tile-table offset, the tile
  properties stay empty and the Vorticon engine died on
  TileProperties.at(0) (std::out_of_range, "__n (which is 0) >= size 0").
  The data texts, sounds and patches are version-specific too, so v1.3 is
  not just one offset away.
- CEGAGraphicsGalaxy: the EGAHEAD and EGAGRAPH sizes are taken by
  seeking to the end, not one byte past it (upstream's seekg(1, end):
  libnix cannot seek past the end, and Keen 4 stopped with "the file
  EGAGRAPH.CK4 is empty").
- CGameLauncher: Esc quits the launcher wherever the focus is (upstream
  gives Back only to the game list, and the launcher starts with Start
  focused, so Esc did nothing); on a message, Esc closes it as Ok does.
- Exit (GsApp::deinit): the engine a quit leaves is freed (upstream
  released the unique_ptr, so the launcher or game was never destroyed),
  then the menus, the effect, every tile, font, sprite and bitmap
  (GsGraphics::freeAll), the music and SDL_mixer's samples, and the audio
  device is closed (Audio::destroy(), with Mix_CloseAudio(), was never
  called), all before SDL_Quit(). On AmigaOS those surfaces and samples
  are SDL2.module's and SDL2_mixer.module's memory, which the end of the
  program does not give back (about 4 MB was lost per run).
- CInput, amiga/amiga_pointer.c: full screen, the mouse events' places are
  taken from Intuition's pointer (the active window's MouseX/MouseY) and
  turned into the renderer's logical units as SDL lays the picture out
  (output stretched over the window, logical size scaled in and centred,
  whole steps with integer scaling). With SDL2.module on a 1920x1200 RTG
  screen, SDL's own numbers put the launcher's buttons away from the
  pointer. The first such event writes the window, renderer and logical
  sizes, and SDL's and Intuition's positions, to CGLog.html. A window on
  the Workbench keeps SDL's numbers.
- CVidConfig: a 640x400 window by default (upstream: 1920x1080); CSettings:
  22050 Hz mono by default.
- CVideoDriver: amiga/amiga_screens.c (from the Seven Kingdoms port) closes
  the empty "SDL" screens SDL2.module 0.6 leaves after a fullscreen window.
- CGeniusEntry: SDL threads get a 512 KB stack (the loaders run in them).
- Debug.cpp: no gdb core dumps. amiga/pthread_stubs.c: what libstdc++ asks
  of POSIX threads (one thread as libstdc++ sees it; SDL's threads are
  SDL2.module's).


Build
-----

    make -f amiga/Makefile.amigaos -j2

with GFOpenAmigaGCC 16.2.0-open4 (STOVE=), the OpenNDK (NDK=), the OpenGPU
developer kit (SDL2KIT=) and the Team's static zlib (DEPS=; built by
amiga/deps/build-m68k-libs.sh zlib). It gives build/amigaos/CommanderGenius
and copies it to dist/CommanderGenius (+ .info). The icons are made with
python3 AMICRADLE/scripts/port_icons.py amiga/icons/icons.json --out
amiga/icons.


Known limits
------------

- Lab, 5 November, with Keen 4 v1.4 shareware: the launcher finds it
  ("Episode 4, Version 1.4"), the Galaxy engine loads, the "Goodbye,
  Galaxy!" title, the wrist-computer menu, New Game (players,
  difficulty), the Shadowlands map with Keen walking, Esc to the menu,
  Quit CG back to the Shell. Esc quits the launcher. Keen 4's pictures
  and colours are right, so the big-endian graphics reading holds for
  Galaxy. Vorticon (Keen 1-3) is still untested: the only Keen 1 data at
  hand is v1.3, which can't be played, and the v1.31 download keeps its
  files in the 1990 installer's own .SHR archive.
- Started again and again from the same Shell, the third start fails
  ("Thread creation failed: No signal bits available for thread sync",
  then "Failed loading the Video Driver"): each run leaves signal bits
  allocated in the Shell's own process (SDL2.module's thread
  synchronisation; the game's thread pool frees its mutexes and
  conditions). Started with Run, or from the Workbench, each start is a
  process of its own and works.
- A start that only opens the launcher and quits still leaves about
  2.6 MB of Fast RAM in use (it was about 4 MB before the exit changes).
- Saved games and high scores are written in the machine's own byte order:
  saves made on a PC do not load here, nor these on a PC.
- Pixel code assumes 32-bit surfaces (as upstream makes them).
- No downloader, no OpenGL, no Cosmo, no on-screen gamepad.
- Keen 1 v1.3 (1990 shareware) can't be played (upstream doesn't know that
  version either); v1.31 can.

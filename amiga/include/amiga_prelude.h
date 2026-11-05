/*
 * Included before every C++ file of the AmigaOS 3 build (-include).
 * - The toolchain's <pthread.h>, which libstdc++ pulls in, turns read(),
 *   write(), open() ... into cancellation-point macros when <unistd.h> came
 *   first, and then <istream>'s own read() member does not compile. Taken
 *   first, here, it defines none of them.
 * - glibc's headers bring <time.h> in with <string> and friends; libnix's
 *   don't.
 * - SDL's AUDIO_S16 and AUDIO_U16 are the little-endian formats on every
 *   machine, but Commander Genius, GsKit and RefKeen mix their 16-bit
 *   samples in the machine's own order: here they are the native
 *   (big-endian) formats, as on a PC they are the native little-endian ones.
 * The AmiCradle Team. GPL 2 or later, like Commander Genius.
 */
#ifndef AMIGA_PRELUDE_H
#define AMIGA_PRELUDE_H
#include <pthread.h>
#include <time.h>
#ifdef __cplusplus
#include <SDL_audio.h>
#undef AUDIO_S16
#define AUDIO_S16 AUDIO_S16SYS
#undef AUDIO_U16
#define AUDIO_U16 AUDIO_U16SYS
#endif
#endif

/*
 * Commander Genius and GsKit join their paths the Unix way: a search path
 * plus "/" plus a name ("PROGDIR:" "/" "games/keen1"), "./" for the
 * current drawer, "a//b" now and then, "dir/../x" in a few places. On
 * AmigaDOS "PROGDIR:/games" is the drawer above PROGDIR:, "a//b" is a's
 * parent, and "." is no name at all.
 *
 * The Makefile links the program with --wrap for each call below, so that
 * every path upstream's code, libstdc++ and SDL_ttf hand to libnix or to
 * SDL first goes through amiga_path(), which makes an AmigaDOS name of it:
 *   - a volume or assign ("PROGDIR:", "Work:") keeps its colon, and no "/"
 *     follows it; anything written before the volume is dropped;
 *   - "." and empty parts go, "x/.." goes;
 *   - a ".." that climbs above the start (no volume) becomes AmigaDOS's
 *     leading "/" (parent); above a volume it is dropped;
 *   - a "/" at the very start (no volume) stays the parent, so that a
 *     name made here comes out the same when it is put through again
 *     (libnix's fopen() calls open(), both wrapped).
 * "" is the current drawer, as AmigaDOS has it.
 *
 * The AmiCradle Team. GPL 2 or later, like Commander Genius.
 */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#define AP_MAX 1024
#define AP_PARTS 64

const char *amiga_path(const char *in, char *out, size_t size)
{
	const char *colon, *p, *start[AP_PARTS];
	size_t len[AP_PARTS], n = 0, o = 0, vol = 0, i;
	int parents = 0;

	if (!in || size == 0)
		return in;

	colon = strchr(in, ':');
	if (colon) {
		/* "./PROGDIR:x" or "/PROGDIR:x": the volume starts the name */
		const char *b = colon;
		while (b > in && b[-1] != '/' && b[-1] != '\\')
			b--;
		vol = (size_t)(colon - b) + 1;
		if (vol >= size)
			return in;
		memcpy(out, b, vol);
		o = vol;
		p = colon + 1;
	} else {
		p = in;
		/* leading slashes: AmigaDOS's parent drawer */
		while (*p == '/') {
			parents++;
			p++;
		}
	}

	while (*p) {
		const char *s;
		size_t l;
		while (*p == '/' || *p == '\\')
			p++;
		if (!*p)
			break;
		s = p;
		while (*p && *p != '/' && *p != '\\')
			p++;
		l = (size_t)(p - s);
		if (l == 1 && s[0] == '.')
			continue;
		if (l == 2 && s[0] == '.' && s[1] == '.') {
			if (n > 0)
				n--;
			else if (!vol)
				parents++;
			continue;
		}
		if (n == AP_PARTS)
			return in;
		start[n] = s;
		len[n] = l;
		n++;
	}

	for (; parents > 0; parents--) {
		if (o + 1 >= size)
			return in;
		out[o++] = '/';
	}
	for (i = 0; i < n; i++) {
		if (o + len[i] + 2 >= size)
			return in;
		memcpy(out + o, start[i], len[i]);
		o += len[i];
		if (i + 1 < n)
			out[o++] = '/';
	}
	out[o] = '\0';
	return out;
}

#define AP(name) char name##_buf[AP_MAX]; const char *name##_a = amiga_path(name, name##_buf, sizeof(name##_buf))

FILE *__real_fopen(const char *, const char *);
FILE *__wrap_fopen(const char *name, const char *mode)
{
	AP(name);
	return __real_fopen(name_a, mode);
}

FILE *__real_freopen(const char *, const char *, FILE *);
FILE *__wrap_freopen(const char *name, const char *mode, FILE *f)
{
	if (!name)
		return __real_freopen(name, mode, f);
	{
		AP(name);
		return __real_freopen(name_a, mode, f);
	}
}

int __real_open(const char *, int, ...);
int __wrap_open(const char *name, int flags, ...)
{
	va_list ap;
	int mode;
	AP(name);
	va_start(ap, flags);
	mode = va_arg(ap, int);
	va_end(ap);
	return __real_open(name_a, flags, mode);
}

int __real_stat(const char *, struct stat *);
int __wrap_stat(const char *name, struct stat *st)
{
	AP(name);
	return __real_stat(name_a, st);
}

int __real_lstat(const char *, struct stat *);
int __wrap_lstat(const char *name, struct stat *st)
{
	AP(name);
	return __real_lstat(name_a, st);
}

DIR *__real_opendir(const char *);
DIR *__wrap_opendir(const char *name)
{
	AP(name);
	return __real_opendir(name_a);
}

int __real_mkdir(const char *, mode_t);
int __wrap_mkdir(const char *name, mode_t mode)
{
	AP(name);
	/* "" (the current drawer) or a bare volume is there already */
	if (!name_a[0] || name_a[strlen(name_a) - 1] == ':')
		return -1;
	return __real_mkdir(name_a, mode);
}

int __real_rmdir(const char *);
int __wrap_rmdir(const char *name)
{
	AP(name);
	return __real_rmdir(name_a);
}

int __real_remove(const char *);
int __wrap_remove(const char *name)
{
	AP(name);
	return __real_remove(name_a);
}

int __real_unlink(const char *);
int __wrap_unlink(const char *name)
{
	AP(name);
	return __real_unlink(name_a);
}

int __real_rename(const char *, const char *);
int __wrap_rename(const char *from, const char *to)
{
	AP(from);
	AP(to);
	return __real_rename(from_a, to_a);
}

int __real_chdir(const char *);
int __wrap_chdir(const char *name)
{
	AP(name);
	return __real_chdir(name_a);
}

int __real_access(const char *, int);
int __wrap_access(const char *name, int mode)
{
	AP(name);
	return __real_access(name_a, mode);
}

SDL_RWops *__real_SDL_RWFromFile(const char *, const char *);
SDL_RWops *__wrap_SDL_RWFromFile(const char *name, const char *mode)
{
	AP(name);
	return __real_SDL_RWFromFile(name_a, mode);
}

SDL_Surface *__real_IMG_Load(const char *);
SDL_Surface *__wrap_IMG_Load(const char *name)
{
	AP(name);
	return __real_IMG_Load(name_a);
}

TTF_Font *__real_TTF_OpenFont(const char *, int);
TTF_Font *__wrap_TTF_OpenFont(const char *name, int size)
{
	AP(name);
	return __real_TTF_OpenFont(name_a, size);
}

TTF_Font *__real_TTF_OpenFontIndex(const char *, int, long);
TTF_Font *__wrap_TTF_OpenFontIndex(const char *name, int size, long index)
{
	AP(name);
	return __real_TTF_OpenFontIndex(name_a, size, index);
}

Mix_Music *__real_Mix_LoadMUS(const char *);
Mix_Music *__wrap_Mix_LoadMUS(const char *name)
{
	AP(name);
	return __real_Mix_LoadMUS(name_a);
}

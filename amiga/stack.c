/* Commander Genius nests its menus, widgets and Lua deeply, and RefKeen's
 * Keen Dreams engine keeps big locals; a Shell gives 4 KB. libnix's
 * startup sets this much up instead.
 * The AmiCradle Team. GPL 2 or later, like Commander Genius. */
unsigned long __stack = 512 * 1024;
static const char version[] __attribute__((used)) = "$VER: CommanderGenius 3.6.4 Amiga (05.11.2026)";

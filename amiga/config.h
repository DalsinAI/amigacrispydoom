/* config.h for the AmigaOS 3.x build (amiga/Makefile.amigaos), in place of
 * the one CMake makes from cmake/config.h.cin. 8-bit paletted (no
 * CRISPY_TRUECOLOR), no FluidSynth, libsamplerate or libpng; zlib for
 * compressed ZDBSP nodes. */
#define PACKAGE_NAME "Crispy Doom"
#define PACKAGE_TARNAME "crispy-doom"
#define PACKAGE_VERSION "7.2.0"
#define PACKAGE_STRING "Crispy Doom 7.2.0"
#define PROGRAM_PREFIX "crispy-"

#define HAVE_DIRENT_H
#define HAVE_LIBZ
#define HAVE_DECL_STRCASECMP 1
#define HAVE_DECL_STRNCASECMP 1

//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 2005-2014 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//	Main program, simply calls D_DoomMain high level loop.
//

#include "config.h"
#include "crispy.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h> // [crispy] setlocale

#include "SDL.h"

#ifdef __amigaos__
#include <string.h>
#include <strings.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/icon.h>
#endif

#include "doomtype.h"
#include "i_system.h"
#include "m_argv.h"
#include "m_misc.h"


//
// D_DoomMain()
// Not a globally visible function, just included for source reference,
// calls all startup code, parses command line options.
//

void D_DoomMain (void);

#ifdef __amigaos__
// libnix gives the program this much stack when it starts with less (the
// Shell's default is 4 KB): the renderer and the game logic recurse, and
// Crispy's larger limits (segs, visplanes, hires) want room to spare.
unsigned long __stack = 524288;

// Started from its Workbench icon, the program's output goes to a console
// window that opens only if something is printed, doesn't take the
// keyboard from the game's screen and closes when the game ends.
const char *__stdiowin = "CON:0/12/640/200/Crispy Doom/AUTO/CLOSE/INACTIVE";
#endif

#ifdef __amigaos__
// A Workbench start's arguments, as from a Shell: the program's name, the
// words of the ARGS tool type of each icon (the program's, then a
// project's, e.g. ARGS=-warp 1 1), and for each project icon that is a
// WAD (freedoom1.wad's icon has this program as its default tool) -iwad
// and its full name.
static void WorkbenchArgs(int *argc, char ***argv)
{
    struct WBStartup *wb = (struct WBStartup *) *argv;
    static char *args[64];
    struct Library *IconBase;
    char name[512];
    int n = 0, i;

    args[n++] = PROGRAM_PREFIX "doom";
    IconBase = OpenLibrary((CONST_STRPTR) "icon.library", 36);

    for (i = 0; wb != NULL && i < wb->sm_NumArgs && n < 60; ++i)
    {
        struct WBArg *arg = &wb->sm_ArgList[i];
        BPTR old = CurrentDir(arg->wa_Lock);
        struct DiskObject *icon = NULL;
        const char *tt;
        size_t len;

        if (IconBase != NULL)
        {
            icon = GetDiskObject((CONST_STRPTR) arg->wa_Name);
        }
        if (icon != NULL
         && (tt = (const char *) FindToolType((CONST_STRPTR *) icon->do_ToolTypes,
                                              (CONST_STRPTR) "ARGS")) != NULL)
        {
            char *words = M_StringDuplicate(tt), *w;

            for (w = strtok(words, " \t"); w != NULL && n < 60;
                 w = strtok(NULL, " \t"))
            {
                args[n++] = w;
            }
        }
        if (icon != NULL)
        {
            FreeDiskObject(icon);
        }

        len = strlen((const char *) arg->wa_Name);
        if (i > 0 && len > 4
         && !strcasecmp((const char *) arg->wa_Name + len - 4, ".wad")
         && NameFromLock(arg->wa_Lock, (STRPTR) name, sizeof(name))
         && AddPart((STRPTR) name, arg->wa_Name, sizeof(name)))
        {
            args[n++] = "-iwad";
            args[n++] = M_StringDuplicate(name);
        }
        CurrentDir(old);
    }

    if (IconBase != NULL)
    {
        CloseLibrary(IconBase);
    }
    args[n] = NULL;
    *argc = n;
    *argv = args;
}
#endif

int main(int argc, char **argv)
{
#ifdef __amigaos__
    // From Workbench argc is 0 and argv is the WBStartup message.
    if (argc == 0)
    {
        WorkbenchArgs(&argc, &argv);
    }
#endif

    // save arguments

    myargc = argc;
    myargv = malloc(argc * sizeof(char *));
    assert(myargv != NULL);

    for (int i = 0; i < argc; i++)
    {
        myargv[i] = M_StringDuplicate(argv[i]);
    }

    // [crispy] Print date and time in the Load/Save Game menus in the current locale
    setlocale(LC_TIME, "");

    //!
    // Print the program version and exit.
    //
    if (M_ParmExists("-version") || M_ParmExists("--version")) {
        puts(PACKAGE_STRING);
        exit(0);
    }

    {
        char buf[16];
        SDL_version version;
        SDL_GetVersion(&version);
        M_snprintf(buf, sizeof(buf), "%d.%d.%d", version.major, version.minor, version.patch);
        crispy->sdlversion = M_StringDuplicate(buf);
        crispy->platform = SDL_GetPlatform();
    }

#if defined(_WIN32)
    // compose a proper command line from loose file paths passed as arguments
    // to allow for loading WADs and DEHACKED patches by drag-and-drop
    M_AddLooseFiles();
#endif

    M_FindResponseFile();
    M_SetExeDir();

    SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1");

    // start doom

    D_DoomMain ();

    return 0;
}


/* Copyright (C) 2001 by Alex Kompel <shurikk@pacbell.net> */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef MSWINMainWindow_h
#define MSWINMainWindow_h

/* this is a main appliation window */

#include "winMS.h"

extern TCHAR szMainWindowClass[];
HWND mswin_init_main_window ();
void mswin_layout_main_window(HWND changed_child);
void mswin_select_map_mode(int map_mode);

#endif /* MSWINMainWindow_h */


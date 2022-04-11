/*
 * Panel - CDE Front Panel Emulator
 *
 * Copyright (C) 1997 Matthew Baron
 *
 * E-mail: mbaron@d.umn.edu
 * URL   : http://www.d.umn.edu/~mbaron/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/* $Id: global.h,v 0.6 1997/05/15 05:50:46 mbaron Exp mbaron $ */

#ifndef GLOBAL_H
#define GLOBAL_H

#include <X11/Intrinsic.h>
#include "layout.h"
#include "record.h"
#include "table.h"

#define APP_CLASS "Panel"
#define APP_NAME "panel"

typedef struct _SysPixmaps {
   Pixmap blank_arrow, up_arrow, down_arrow, spacer, unknown, left_decor,
	right_decor, menu_decor, lock, exit;
} SysPixmaps;

extern SubTable subpanels;
extern ControlTable controls;
extern DeskTable desks;

extern Screen *screen;
extern Display *display;

extern LayoutRec layout;
extern SysPixmaps pixmaps;
extern PanelRec panel;

extern char *rc_file_name;

extern XtTranslations no_drag;

extern ICommandRec icmd[];

#endif

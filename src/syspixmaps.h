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

/* $Id: syspixmaps.h,v 0.7 1997/05/15 05:11:06 mbaron Exp mbaron $ */

#ifndef SYSPIXMAPS_H
#define SYSPIXMAPS_H


#ifdef WIDE_ARROWS
   #include "icons/fp_arrow_up.xpm"
   #include "icons/fp_arrow_down.xpm"
   #include "icons/fp_arrow_blank.xpm"
#else
   #include "icons/arrowup.xpm"
   #include "icons/arrowblank.xpm"
   #include "icons/arrowdown.xpm"
#endif

#include "icons/spacer6.xpm"

#include "icons/left_menu.xpm"
#include "icons/right_menu.xpm"

#include "icons/title_decor.xpm"

#include "icons/exit.xpm"
#include "icons/lock.xpm"

#include "icons/blankicon.xpm"

/* The pixmap to be used when the specified one can't be found */
#include "icons/unknown1.xpm"

#endif





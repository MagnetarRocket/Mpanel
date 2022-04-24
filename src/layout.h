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

/* $Id: layout.h,v 1.5 1997/05/15 05:21:49 mbaron Exp mbaron $ */

#ifndef LAYOUT_H
#define LAYOUT_H

#include <Xm/Xm.h>

typedef struct layoutrec {
   Widget toplevel, left, center, right, center_left, center_center,
	center_right, lip, rhandle, lhandle, rdecor, ldecor, lribs, rribs, myleftrib, myrightrib;
} LayoutRec;

void init_layout(LayoutRec*, Widget parent);
void create_ribs(LayoutRec*);

#endif

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

/* $Id: subedit.h,v 1.1 1997/06/23 09:07:11 mbaron Exp mbaron $ */

#ifndef SUBEDIT_H
#define SUBEDIT_H

#include <X11/Intrinsic.h>

#include "record.h"

void edit_global_subpanel_properties(Widget);
void edit_global_subpanel_properties_cb(Widget, XtPointer, XtPointer);

void edit_subpanel_properties(Widget, SubpanelRec*);
void edit_subpanel_properties_cb(Widget, XtPointer, XtPointer);


#endif

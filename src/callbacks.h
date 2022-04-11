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

/* $Id: callbacks.h,v 0.6 1997/05/15 05:14:04 mbaron Exp mbaron $ */ 

#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <X11/Intrinsic.h>

void execute_str(Widget, XtPointer, XtPointer);
void execute_control(Widget, XtPointer, XtPointer);
void execute_subpanel_control(Widget, XtPointer, XtPointer);

void exit_panel(Widget, XtPointer, XtPointer);
void iconify_panel(Widget, XtPointer, XtPointer);
void edit_properties_cb(Widget, XtPointer, XtPointer);
void sigsegv_cb(int);

#endif


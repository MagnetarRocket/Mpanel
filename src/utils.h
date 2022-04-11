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

/* $Id: utils.h,v 0.5 1997/05/15 05:29:44 mbaron Exp mbaron $ */

#ifndef UTILS_H
#define UTILS_H

int create_xpm_data(Widget, char**, Pixmap*);
int create_xpm_file(Widget, char*, Pixmap*);
Widget button_with_pixmap(Widget, Pixmap*, char*);
Widget flat_button_with_text(Widget, char*, char*);
Widget labeled_entry(Widget, char*, int, char*);
void FakeCallback(Widget, XtPointer, XtPointer);

void play_au_file(char*);

#endif

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

/* $Id: subpanel.h,v 0.7 1997/05/15 05:53:14 mbaron Exp mbaron $ */

#ifndef SUBPANEL_H
#define SUBPANEL_H

#include "record.h"

void create_subpanel(SubpanelRec*);
Widget new_subpanel_item(SubpanelRec*,ControlRec*);

void open_subpanel(SubpanelRec*);
void close_subpanel(SubpanelRec*);

void position_subpanel(SubpanelRec*);
void bind_subpanel_arrow(SubpanelRec*);

#endif

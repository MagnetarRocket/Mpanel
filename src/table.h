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

/* $Id: table.h,v 0.5 1997/05/15 05:28:31 mbaron Exp mbaron $ */

#ifndef TABLE_H
#define TABLE_H

#include "record.h"

void init_subpanel_table(SubTable*);
void register_subpanel(SubTable*, SubpanelRec*);
SubpanelRec* lookup_subpanel(SubTable,char*);

void init_control_table(ControlTable*);
void register_control(ControlTable*, ControlRec*);
ControlRec* lookup_control(ControlTable, char*);

void init_desk_table(DeskTable*);
void register_desk(DeskTable*, DeskRec*);

   void print_control_table(ControlTable);
   void print_subtable(SubTable);

#endif

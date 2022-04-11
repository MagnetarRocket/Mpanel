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

/* $Id: record.c,v 0.6 1997/05/21 19:07:23 mbaron Exp mbaron $ */

#include "record.h"

ControlRec* new_control() {
ControlRec *c;

   c = (ControlRec*)malloc(sizeof(ControlRec));

   if (c == NULL) return NULL;

   init_control_rec(c);
   return c;
}

void init_control_rec(ControlRec *c) {
   c->icon = c->alt_icon = c->click_action = c->drop_action = c->id = NULL;
   c->label = c->container_name = NULL;

   c->parent_widget = c->arrow = c->label_widget = c->icon_widget = NULL;
   c->subpanel = NULL;

   c->container_type = Box;
   c->control_type = Icon;
   c->action_type = Shell;
   c->link = NULL;
}

SubpanelRec* new_subpanel() {
SubpanelRec *s;

   s = (SubpanelRec*)malloc(sizeof(SubpanelRec));

   if (s == NULL) return NULL;
   init_subpanel_rec(s);

   return s;
}

void init_subpanel_rec(SubpanelRec *s) {
   s->id = s->label = s->container_name = NULL;
   s->shell = s->over = s->content = NULL;
   s->state = 0;
   s->x = s->y = 0;
   s->link = 0;
   s->needs_coords = True;
}

DeskRec* new_desk() {
DeskRec *d;
  
   d = (DeskRec*)malloc(sizeof(DeskRec));

   if (d == NULL) return NULL;

   d->label = NULL;
   d->x = d->y = 0;

   return d;
}

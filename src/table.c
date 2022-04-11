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

/* $Id: table.c,v 0.5 1997/05/15 05:28:51 mbaron Exp mbaron $ */

#include "table.h"
#include "global.h"

void init_subpanel_table(SubTable *t) {
   t->num = 0;
   t->head = NULL;
}

void register_subpanel(SubTable *t, SubpanelRec *s) {
   s->link = t->head;
   t->head = s;
   t->num++;
}

SubpanelRec* lookup_subpanel(SubTable t, char *id) {
SubpanelRec *tmp = t.head;

   while(tmp != NULL) {
      if (tmp->id[0] == id[0] && !strcmp(tmp->id, id))
         return tmp;
      tmp = tmp->link;
   }

   return NULL;
}

void init_control_table(ControlTable *t) {
   t->num = 0;
   t->head = t->tail = NULL;
}

void register_control(ControlTable *t, ControlRec *c) {

   if (t->num == 0)
      t->head = t->tail = c;
   else
      t->tail = t->tail->link = c;

   t->num++;
}

ControlRec* lookup_control(ControlTable t, char *id) {
ControlRec *tmp = t.head;


   while(tmp != NULL) {
      if (tmp->id[0] == id[0] && !strcmp(tmp->id, id))
         return tmp;
      tmp = tmp->link;
   }

   return NULL;
}

void register_desk(DeskTable *t, DeskRec *d) {
   t->list[t->num++] = d;
}

void init_desk_table(DeskTable *d) {
   d->num = 0;
}

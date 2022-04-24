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

/* $Id: record.h,v 0.8 1997/05/21 19:07:29 mbaron Exp mbaron $ */

#ifndef RECORD_H
#define RECORD_H

#include <X11/Intrinsic.h>
#define MAX_CONTROLS 50

typedef enum _ContainerType {Box, Subpanel, Switch} ContainerType;
typedef enum _ControlType {Icon, Biff, Clock, Blank} ControlType;
typedef enum _SubpanelModel {SlideUp, PopUp} SubpanelModel;
typedef enum _ActionType {Internal, Shell} ActionType;

typedef struct _ControlRec {
   char *id, *click_action, *drop_action, *icon, *alt_icon, *label;

   char *container_name;
   ContainerType container_type;

   ControlType control_type;
   ActionType action_type;

   Widget parent_widget, arrow, icon_widget, label_widget;
   Pixmap pixmap, alt_pixmap;

   struct _SubpanelRec *subpanel;
   struct _ControlRec *link;

} ControlRec;

typedef struct _ControlTable {

   int num;
   ControlRec *head, *tail;

} ControlTable;

typedef struct _SubpanelRec {

   char *id, *label, *container_name;
   Widget shell, content, over;
   short state;
   int x, y, height;
   struct _SubpanelRec *link;
   Boolean needs_coords;

} SubpanelRec;

typedef struct _SubTable {
   int num;
   SubpanelRec *head;
} SubTable;

typedef struct _DeskRec {
   char *label;
   Widget button;
   int x,y;
} DeskRec;

typedef struct _ICommandRec {
   char *key;
   XtCallbackProc cmd;
} ICommandRec;

typedef struct _DeskTable {
   int num;
   int x;
   int y;
   int x1;  /* for fvwvm pager config */
   int y1;  /* for fvwvm pager config */
   DeskRec *list[10];
} DeskTable;

typedef struct _PanelRec {
   char *lock, *pixmap_path;
   SubpanelModel subpanel_model;
   int subpanel_y_offset, subpanel_x_offset;
} PanelRec;

ControlRec* new_control();
void init_control_rec(ControlRec*);
void print_control_rec(ControlRec);

SubpanelRec* new_subpanel();
void init_subpanel_rec(SubpanelRec*);
void print_subpanel_rec(SubpanelRec);

DeskRec* new_desk();

#endif

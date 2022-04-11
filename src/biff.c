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

/* $Id: biff.c,v 1.2 1997/05/21 19:07:04 mbaron Exp mbaron $ */

#include <Xm/XmAll.h>
#include <sys/stat.h>
#include "record.h"
#include "MdtString.h"

typedef struct _BiffRec {
   ControlRec *control;
   char *folder_name;
   int last_folder_size;
} BiffRec;

void biff_set_timer(BiffRec*);
void biff_update_cb(BiffRec*, XtIntervalId*);
void biff_raise_flag(BiffRec*);
void biff_lower_flag(Widget, XtPointer, XtPointer);

int get_folder_size(BiffRec biff) {
struct stat statb;

   if (stat(biff.folder_name, &statb) == -1)
      return 0; 

   return statb.st_size;
}

void biff_raise_flag(BiffRec *biff) {
   XtVaSetValues(biff->control->icon_widget,
	XmNlabelPixmap, biff->control->alt_pixmap,
	NULL);
}

void biff_lower_flag(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
BiffRec *biff = (BiffRec*)client_data;

   XtVaSetValues(biff->control->icon_widget,
	XmNlabelPixmap, biff->control->pixmap,
	NULL);
}

void biff_update_cb(BiffRec *biff, XtIntervalId *id) {
int new_folder_size = get_folder_size(*biff);
static cnt = 0;

   if (new_folder_size > biff->last_folder_size) {
      biff->last_folder_size = new_folder_size;
      biff_raise_flag(biff);
      /*XBell(XtDisplay(biff->control->icon_widget), 100);*/
   }

   biff_set_timer(biff);
}

void biff_set_timer(BiffRec *biff) {
   XtAppAddTimeOut(XtWidgetToApplicationContext(biff->control->icon_widget),
        10000, (XtTimerCallbackProc)biff_update_cb, biff);
} 

int biff_init(ControlRec *c) {
BiffRec *biff = NULL;

   if ((biff = (BiffRec*)malloc(sizeof(BiffRec))) == NULL) 
      return 0;

   if (c->alt_icon != NULL) {
      create_xpm_file(c->parent_widget, c->alt_icon, &c->alt_pixmap);
   } else {
      c->alt_pixmap = c->pixmap;
   }


   MdtStringSet(&biff->folder_name, "/usr/spool/mail/mbaron");
   biff->control = c;

   XtAddCallback(c->icon_widget, XmNactivateCallback, biff_lower_flag, biff);

   biff_set_timer(biff);
 
   return 1;       
}

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

/* $Id: callbacks.c,v 0.6 1997/05/15 05:55:12 mbaron Exp mbaron $ */

#include "callbacks.h"
#include "record.h"
#include "global.h"
#include "properties.h"

#include <X11/Intrinsic.h>

void execute_str(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
char *command = (char*)client_data;
  
   if (command != NULL) {
      char *command_str;

      MdtVaString(&command_str, command, " &", NULL);

      if (command_str != NULL) {
         system(command_str);
         MdtStringFree(&command_str);
      }

   }
}

void execute_control(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
ControlRec *c = (ControlRec*)client_data;

   execute_str(w, c->click_action, call_data);

}

void execute_subpanel_control(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
ControlRec *c = (ControlRec*)client_data;
XButtonPressedEvent *bevent = (XButtonPressedEvent*)call_data;

   switch(bevent->button) {
      case 1: 
         execute_str(w,c->click_action, call_data); 
         close_subpanel((SubpanelRec*)c->subpanel); 
      break;
   }
}

void exit_panel(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
   exit(0);
}

void iconify_panel(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
Window win = XtWindowOfObject(layout.toplevel);

   XIconifyWindow(display, win, 0);
}

void sigsegv_cb(int sig) {
   MdtErrorDialog(layout.toplevel, "Caught Signal SIGSEGV. Exiting...");
   exit(sig);
}

void edit_properties_cb(w, client_data, call_data) 
Widget w;
XtPointer client_data;
XtPointer call_data;
{
   edit_properties(layout.toplevel);
}








void  mov_cbk(Widget w, XEvent *event, 
String *args, int *num_args)

{   static Position x, y, last_x, last_y;
Position width, height;


if (strcmp(args[0], "down") == 0) 
{  /* anchor initial point (save its value) */
/*x = event->x;
y = event->y;*/
last_x = event->xbutton.x_root;
last_y = event->xbutton.y_root;
} 

if (strcmp(args[0], "motion") == 0) 
{ /* draw "ghost" box to show where it could go */
/* undraw last box */

x = event->xbutton.x_root;
y = event->xbutton.y_root;

width=x-last_x;
height=y-last_y;
XtVaGetValues(layout.toplevel, XmNx, &x, NULL);
XtVaGetValues(layout.toplevel, XmNy, &y, NULL);
x=x+width;
y=y+height;
XtVaSetValues(layout.toplevel, XmNx, x, NULL);
XtVaSetValues(layout.toplevel, XmNy, y, NULL);
last_x = event->xbutton.x_root;
last_y = event->xbutton.y_root;




}

       
if (strcmp(args[0], "up") == 0)             
{ /* draw full line */

}

}

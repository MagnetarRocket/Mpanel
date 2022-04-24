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

#include "deskswitch.h"
#include "record.h"
#include "wm.h"
#include <Xm/XmAll.h>

Widget last_pressed = NULL;
void handle_deskswitch_press(Widget, XtPointer, XtPointer);

Widget new_deskswitch_button(Widget parent, DeskRec *d) {
Widget button, frame1, frame2;
XmString str;
static int count=0;

   frame1 = XtVaCreateManagedWidget("DeskSwitchButton",
	xmFrameWidgetClass, parent,
	XmNshadowType, XmSHADOW_ETCHED_IN,
	XmNshadowThickness, 1,
	NULL);

   frame2 = XtVaCreateManagedWidget("DeskSwitchButton",
	xmFrameWidgetClass, frame1,
	XmNshadowType, XmSHADOW_OUT,
	XmNshadowThickness, 2,
	XmNmarginWidth, 0, XmNmarginHeight, 0, 
	NULL);

   str = XmStringCreateLocalized(d->label);

   d->button = XtVaCreateManagedWidget("DeskSwitchButton",
        xmLabelWidgetClass, frame2,
	XmNlabelString, str,
	XmNalignment, XmALIGNMENT_BEGINNING,
	XmNmarginLeft, 3,
        XmNhighlightThickness, 0,
        XmNborderWidth, 0,
	XmNshadowThickness, 0,
	XmNmarginHeight, 0,
	XmNmarginTop, 0,
	XmNmarginBottom,0,
	XmNhighlightOnEnter, True,
	XmNwidth, 125,
	XmNrecomputeSize, False,
        NULL);

   XmStringFree(str);

   XtAddEventHandler(d->button, 
	ButtonReleaseMask, False, (void*)handle_deskswitch_press, d);

   if (!count) {
      XtVaSetValues(frame2,
	XmNshadowType, XmSHADOW_IN,
	XmNshadowThickness, 2,
	NULL);
      last_pressed = d->button;
   }
   count++;
//   d->x = d->y = count-1;

   return frame1;
}

extern void GotoPage(Widget w, int x, int y);
extern Display * display;

void handle_deskswitch_press(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
DeskRec *d = (DeskRec*)client_data;

    goto_page(w, display, d->x, d->y);

/*    XtVaSetValues(XtParent(last_pressed),
	XmNshadowType, XmSHADOW_OUT,
	XmNshadowThickness, 2,
	NULL);

    XtVaSetValues(XtParent(w),
	XmNshadowType, XmSHADOW_IN,
	XmNshadowThickness, 2,
	NULL);

   last_pressed = w;*/
}

void new_deskswitch_select(DeskRec *d)
{
Widget w;

//DeskRec *d = desks.list[i];

    w=d->button;

    XtVaSetValues(XtParent(last_pressed),
	XmNshadowType, XmSHADOW_OUT,
	XmNshadowThickness, 2,
	NULL);

    XtVaSetValues(XtParent(w),
	XmNshadowType, XmSHADOW_IN,
	XmNshadowThickness, 2,
	NULL);

   last_pressed = w;
}


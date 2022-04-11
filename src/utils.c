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

/* $Id: utils.c,v 0.6 1997/05/15 06:00:06 mbaron Exp mbaron $ */

#include "xincludes.h"
#include <X11/xpm.h>

#include "syspixmaps.h"
#include "global.h"

int create_xpm_data(Widget back_widget, char **data, Pixmap *pixm) {
XpmAttributes attr;
Pixmap dummy_mask;
Window win;
Pixel back;
static XpmColorSymbol none_color = { NULL, "None", (Pixel) 0 };

   XtVaGetValues(back_widget,XmNbackground,&back,NULL);
   none_color.pixel = back;

   attr.valuemask = XpmReturnPixels | XpmColorSymbols | XpmCloseness;
   attr.colorsymbols = &none_color;
   attr.numsymbols = 1;
   attr.closeness = 80000;

   win = DefaultRootWindow(display);

   return (XpmCreatePixmapFromData(display,win,data, pixm, &dummy_mask, &attr));
}

int create_xpm_file(Widget back_widget, char *file, Pixmap *pixm) {
char *pixmap_and_path;
XpmAttributes attr;
Pixmap dummy_mask;
Window win;
Pixel back;
static XpmColorSymbol none_color = { NULL, "None", (Pixel) 0 };
int result;

   XtVaGetValues(back_widget,XmNbackground,&back,NULL);
   none_color.pixel = back;

   attr.valuemask = XpmReturnPixels | XpmColorSymbols | XpmCloseness;
   attr.colorsymbols = &none_color;
   attr.numsymbols = 1;
   attr.closeness = 80000;

   win = DefaultRootWindow(display);

   if (file != NULL && file[0] != '/')
      MdtVaString(&pixmap_and_path, panel.pixmap_path, "/", file, NULL);
   else 
      pixmap_and_path = file;

   if (XpmReadFileToPixmap(display, win, pixmap_and_path, pixm,
       &dummy_mask, &attr)) {
      create_xpm_data(back_widget, unknown_xpm, pixm);
      return 0;
   }

   return 1;
}

Widget flat_button_with_text(Widget parent, char *text, char *res) {
Widget button;
XmString str;

   str = XmStringCreateSimple(text);

   button = XtVaCreateManagedWidget(res,
        xmPushButtonWidgetClass, parent,
	XmNlabelString, str,
        XmNhighlightThickness, 0,
        XmNborderWidth, 0,
	XmNshadowThickness, 0,
	XmNmarginHeight, 0,
	XmNmarginTop, 0,
	XmNmarginBottom, 0,
	XmNfillOnArm, False,
	NULL);

   XmStringFree(str);

   XtOverrideTranslations(button, no_drag);

   return button;
}

Widget button_with_pixmap(Widget parent, Pixmap *p, char *res) {
Widget button;

   button = XtVaCreateManagedWidget(res,
        xmPushButtonWidgetClass, parent,
        XmNlabelType, XmPIXMAP,
        XmNlabelPixmap, p,
        XmNhighlightThickness, 1,
        XmNborderWidth, 0,
	XmNshadowThickness, 0,
	XmNmarginHeight, 0,
	XmNmarginTop, 0,
	XmNmarginBottom, 0,
	XmNhighlightOnEnter, True,
	NULL);

   XtOverrideTranslations(button, no_drag);

   return button;
}

Widget labeled_entry(Widget parent, char *label_txt, int width, char *value) {
Widget form, entry,  label;

   form = XtVaCreateManagedWidget("form",
	xmFormWidgetClass, parent,
	NULL);

   label = XtVaCreateManagedWidget(label_txt,
	xmLabelGadgetClass, form,
	XmNleftAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNwidth, width,
	XmNalignment, XmALIGNMENT_END,
	NULL);

   entry = XtVaCreateManagedWidget("header",
        xmTextFieldWidgetClass, form,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget,label,
	/*XmNrightAttachment, XmATTACH_FORM,*/
        XmNtopAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNhighlightThickness, 1,
	XmNvalue, value,
        NULL);

   return entry;    
}

void FakeCallback(Widget w, XtPointer cb, XtPointer cb_data) {
   XtAddEventHandler(w, ButtonReleaseMask, False, (void*)cb, cb_data);
}


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

/* $Id: intro.c,v 0.6 1997/05/15 05:58:00 mbaron Exp mbaron $ */

#include "xincludes.h"

Widget intro_shell, intro_msg;

#ifdef XPM_INTRO
  #include "icons/panel.xpm"
#endif

void raise_intro(Widget parent) {
Dimension width, height;
Widget frame, text_frame, text_form, form, title, ver, author;
XmString str;

   intro_shell = XtVaCreateWidget("Intro",
	topLevelShellWidgetClass, parent,
	XmNmwmDecorations, 0,
	XmNmappedWhenManaged, False,
	NULL);

   frame = XtVaCreateManagedWidget("frame",
	xmFrameWidgetClass, intro_shell,
	XmNshadowType, XmSHADOW_OUT,
	XmNshadowThickness, 2,
	NULL);

   form = XtVaCreateManagedWidget("form",
	xmFormWidgetClass, frame,
	NULL);

   text_frame = XtVaCreateManagedWidget("text_frame",
	xmFrameWidgetClass, form,
	XmNshadowType, XmSHADOW_IN,
	XmNshadowThickness, 2,
	XmNtopAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNleftAttachment, XmATTACH_FORM,
	XmNleftOffset, 8, XmNrightOffset, 8,
	XmNtopOffset, 8,
	NULL);

   text_form = XtVaCreateManagedWidget("Text",
	xmRowColumnWidgetClass, text_frame,
	XmNpacking, XmPACK_COLUMN,
	XmNorientation, XmVERTICAL,
	XmNnumColumns, 1,
	XmNentryAlignment, XmALIGNMENT_CENTER,
	XmNspacing, 2, 
	NULL);

#ifndef XPM_INTRO
   str = XmStringCreateSimple("P a n e l");
   XtVaCreateManagedWidget("Title",
	xmLabelGadgetClass, text_form,
	XmNlabelString, str,
	NULL);
   XmStringFree(str);

   str = XmStringCreateSimple("version 0.4");
   XtVaCreateManagedWidget("Version",
	xmLabelGadgetClass, text_form,
	XmNlabelString, str,
	NULL);
   XmStringFree(str);

   str = XmStringCreateSimple("by Matthew Baron");
   XtVaCreateManagedWidget("Author",
	xmLabelGadgetClass, text_form,
	XmNlabelString, str,
	NULL);
   XmStringFree(str);
#else
   {
   Pixmap p;
   create_xpm_data(text_form, panel_xpm, &p);
   XtVaCreateManagedWidget("Title",
	xmLabelGadgetClass, text_form,
	XmNlabelType, XmPIXMAP,
	XmNlabelPixmap, p,
	NULL);
   }
#endif
   intro_msg = XtVaCreateManagedWidget("Message",
	xmTextFieldWidgetClass, form,
	XmNtopAttachment, XmATTACH_WIDGET,
	XmNtopWidget, text_form,
	XmNrightAttachment, XmATTACH_FORM,
	XmNleftAttachment, XmATTACH_FORM,
	XmNshadowThickness, 0,
	XmNeditable, False,
	XmNhighlightThickness, 0,
	XmNcolumns, 30,
	XmNcursorPositionVisible, False,
	NULL);

   XtRealizeWidget(intro_shell);

   XtVaGetValues(intro_shell, XmNwidth, &width, XmNheight, &height, NULL);

   XtVaSetValues(intro_shell,
        XmNx, (int)((WidthOfScreen(XtScreen(intro_shell))-width)/2),
        XmNy, (int)((HeightOfScreen(XtScreen(intro_shell))-height)/2),
        NULL);    

   XtMapWidget(intro_shell);
   XmUpdateDisplay(intro_shell);
}

void kill_intro() {
   XtPopdown(intro_shell);
   XtDestroyWidget(intro_shell);
}

void update_intro(char *pre, char *message) {
char buff[75];

   sprintf(buff, "%s %s", pre, message);

   XtVaSetValues(intro_msg, XmNvalue, buff, NULL);
   XmUpdateDisplay(intro_shell);
}

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

/* $Id: edit.c,v 1.1 1997/06/23 08:15:05 mbaron Exp mbaron $ */

#include "record.h"
#include "global.h"
#include "MdtDialog.h"
#include <Xm/XmAll.h>

typedef struct _CEditRec {
   Widget shell, msgbox, icon_sample, icon_entry, action_entry, label_entry;
   Pixmap icon_pixmap;
   ControlRec *control;
} CEditRec;

void cedit_ok_cb(Widget, XtPointer, XtPointer);
void cedit_cancel_cb(Widget, XtPointer, XtPointer);
void icon_browse_cb(Widget, XtPointer, XtPointer);

void edit_control(Widget parent, ControlRec *c) {
CEditRec *r;
Widget icon_frame, action_frame, label_frame, main_form;

   r = (CEditRec*)malloc(sizeof(CEditRec));
   r->control = c; 

   r->shell = XtVaCreatePopupShell("ControlEditor",
        xmDialogShellWidgetClass, parent,
        XmNautoUnmanage, False,
        XmNtitle, "Control Editor",
	XmNmwmDecorations, 0 | 2 | 8 | 16,
	XmNallowShellResize, True,
        NULL);

   r->msgbox =  XtVaCreateWidget("message_box",
        xmMessageBoxWidgetClass, r->shell,
	XmNautoUnmanage, False,
	NULL);

   main_form = XtVaCreateManagedWidget("form",
	xmFormWidgetClass, r->msgbox,
	NULL);

   icon_frame = XtVaCreateManagedWidget("icon_frame",
	xmFrameWidgetClass, main_form,
	XmNtopAttachment, XmATTACH_FORM,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNmarginWidth, 8, XmNmarginHeight, 4,
	NULL);

   XtVaCreateManagedWidget("Icon",
	xmLabelGadgetClass, icon_frame,
	XmNchildType, XmFRAME_TITLE_CHILD,
	NULL);

   /* Create the icon attributes */ {
   Widget form, button;

   form = XtVaCreateManagedWidget("form",
	xmFormWidgetClass, icon_frame,
	NULL);

   create_xpm_file(form,
	(r->control->icon != NULL ? r->control->icon : ""),
	&r->icon_pixmap);

   r->icon_sample = XtVaCreateManagedWidget("icon_sample",
	xmLabelWidgetClass, form,
	XmNlabelType, XmPIXMAP,
	XmNlabelPixmap, r->icon_pixmap,
        XmNtopAttachment, XmATTACH_FORM,
	XmNleftAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
	NULL);

   XtOverrideTranslations(r->icon_sample, no_drag);

   r->icon_entry = XtVaCreateManagedWidget("icon_entry",
	xmTextFieldWidgetClass, form,
	XmNleftAttachment, XmATTACH_WIDGET,
	XmNleftWidget, r->icon_sample,
	XmNhighlightThickness, 1,
	XmNleftOffset, 8,
	XmNvalue, c->icon,
	NULL);

   button = XtVaCreateManagedWidget("Browse",
	xmPushButtonGadgetClass, form,
	XmNleftAttachment, XmATTACH_WIDGET,
	XmNleftWidget, r->icon_entry,
	XmNleftOffset, 8,
	XmNrightAttachment, XmATTACH_FORM,
	XmNmarginLeft, 6, XmNmarginRight, 6,
	NULL);

   XtAddCallback(button, XmNactivateCallback, icon_browse_cb, r);
   }

   action_frame = XtVaCreateManagedWidget("action_frame",
	xmFrameWidgetClass, main_form,
	XmNtopAttachment, XmATTACH_WIDGET,
	XmNtopWidget, icon_frame,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNtopOffset, 6,
	XmNmarginWidth, 8, XmNmarginHeight, 4,
	NULL);

   XtVaCreateManagedWidget("Action",
	xmLabelGadgetClass, action_frame,
	XmNchildType, XmFRAME_TITLE_CHILD,
	NULL);

   r->action_entry = XtVaCreateManagedWidget("action_entry",
	xmTextFieldWidgetClass, action_frame,
	XmNvalue, c->click_action,
	XmNhighlightThickness, 1,
	NULL);

   label_frame = XtVaCreateManagedWidget("label_frame",
	xmFrameWidgetClass, main_form,
	XmNtopAttachment, XmATTACH_WIDGET,
	XmNtopWidget, action_frame,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNtopOffset, 6,
	XmNmarginWidth, 8, XmNmarginHeight, 4,
	NULL);

   XtVaCreateManagedWidget("Label",
	xmLabelGadgetClass, label_frame,
	XmNchildType, XmFRAME_TITLE_CHILD,
	NULL);

   r->label_entry = XtVaCreateManagedWidget("label_entry",
	xmTextFieldWidgetClass, label_frame,
	XmNvalue, c->label,
	XmNhighlightThickness, 1,
	NULL);

   XtAddCallback(r->msgbox, XmNokCallback, cedit_ok_cb, r);
   XtAddCallback(r->msgbox, XmNcancelCallback, cedit_cancel_cb, r);

   XtUnmanageChild(XmMessageBoxGetChild(r->msgbox,XmDIALOG_HELP_BUTTON));

   XtManageChild(r->msgbox);
   XtManageChild(r->shell);
   XtPopup(r->shell, XtGrabExclusive);
   XtAddGrab(r->shell, True, False);
}

void cedit_ok_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
CEditRec *r = (CEditRec*)client_data;
char *tmp;

   XtPopdown(r->shell);

   tmp = XmTextGetString(r->action_entry);
   MdtStringFree(&r->control->click_action);
   MdtStringSet(&r->control->click_action, tmp);
   XtFree(tmp);

   tmp = XmTextGetString(r->icon_entry),
   MdtStringFree(&r->control->icon);
   MdtStringSet(&r->control->icon, tmp);
   XtFree(tmp);

   tmp = XmTextGetString(r->label_entry),
   MdtStringFree(&r->control->label);
   MdtStringSet(&r->control->label, tmp);
   XtFree(tmp);

   if (r->control->label_widget != NULL) {
      XmString str = XmStringCreateSimple(r->control->label);

      XtVaSetValues(r->control->label_widget,
	XmNlabelString, str,
        NULL);
      
      XmStringFree(str);
   }

   if (r->control->icon_widget != NULL) {
      r->control->pixmap = r->icon_pixmap;
      XtVaSetValues(r->control->icon_widget,
	XmNlabelPixmap, r->control->pixmap,
	NULL);
   }

   if (r->control->container_type == Subpanel)
      r->control->subpanel->needs_coords = True;

   XtDestroyWidget(r->shell);

   free(r);
}

void cedit_cancel_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
CEditRec *r = (CEditRec*)client_data;

   XtPopdown(r->shell);
   XtDestroyWidget(r->shell);
   free(r);
}

void icon_browse_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
CEditRec *r = (CEditRec*)client_data;
char *file;

   file = SelectFile(r->shell, "Select Pixmap", panel.pixmap_path, "*pm");

   if (file == NULL)
      return;

   XtVaSetValues(r->icon_entry, XmNvalue, file, NULL);

   XFreePixmap(display, r->icon_pixmap);

   create_xpm_file(XtParent(r->icon_sample), file, &r->icon_pixmap);
   XtVaSetValues(r->icon_sample, XmNlabelPixmap, r->icon_pixmap, NULL);

   XtFree(file);
}

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

/* $Id: subedit.c,v 1.1 1997/06/23 09:08:50 mbaron Exp mbaron $ */

#include "subedit.h"
#include "record.h"
#include "global.h"
#include "MdtDialog.h"
#include <Xm/XmAll.h> 

typedef struct _GSEditRec {
   Widget shell, msgbox;
   Widget x_scale, y_scale, slideup_model, popup_model;
} GSEditRec;

typedef struct _SEditRec {
   Widget shell, msgbox;
   Widget title_entry;
   SubpanelRec *subpanel;
} SEditRec;

void gsedit_cb(Widget, XtPointer, XtPointer);
void sedit_cb(Widget, XtPointer, XtPointer);

void edit_global_subpanel_properties_cb(w,client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{

   edit_global_subpanel_properties(layout.toplevel);

}

void edit_global_subpanel_properties(Widget parent) {
GSEditRec *r;
Widget model_frame, offset_frame, main_form, rowcol, radio_box;
XmString str;

   r = (GSEditRec*)malloc(sizeof(GSEditRec));

   r->shell = XtVaCreatePopupShell("GlobalSubpanelProperties",
        xmDialogShellWidgetClass, parent,
        XmNautoUnmanage, False,
        XmNtitle, "Global Subpanel Properties",
	XmNmwmDecorations, 0 | 2 | 8 | 16, 
        NULL);

   r->msgbox =  XtVaCreateWidget("message_box",
        xmMessageBoxWidgetClass, r->shell,
	XmNautoUnmanage, False,
	XtVaTypedArg, XmNhelpLabelString, XmRString, "Apply", strlen("Apply"),
	NULL);

   main_form = XtVaCreateManagedWidget("form",
	xmFormWidgetClass, r->msgbox,
	NULL);

   model_frame = XtVaCreateManagedWidget("model_frame",
	xmFrameWidgetClass, main_form,
	XmNtopAttachment, XmATTACH_FORM,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNmarginWidth, 8, XmNmarginHeight, 4,
	NULL);

   XtVaCreateManagedWidget("Subpanel Model",
	xmLabelGadgetClass, model_frame,
	XmNchildType, XmFRAME_TITLE_CHILD,
	NULL);

   radio_box = XmCreateRadioBox(model_frame, "radio_box", NULL, 0);
   
   r->slideup_model = XmCreateToggleButtonGadget(radio_box,
	"Use Animated Subpanels", NULL, 0);

   XtVaSetValues(r->slideup_model,
	XmNhighlightThickness, 0,
	XmNset, (panel.subpanel_model == SlideUp ? True : False),
	NULL);
   XtManageChild(r->slideup_model);

   r->popup_model = XmCreateToggleButtonGadget(radio_box,
	"Use Popup Subpanels", NULL, 0);

   XtVaSetValues(r->popup_model,
	XmNhighlightThickness, 0,
	XmNset, (panel.subpanel_model == PopUp ? True : False),
	NULL);
   XtManageChild(r->popup_model); 

   XtManageChild(radio_box);

   offset_frame = XtVaCreateManagedWidget("offset_frame",
	xmFrameWidgetClass, main_form,
	XmNtopAttachment, XmATTACH_WIDGET,
	XmNtopWidget, model_frame,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNtopOffset, 6,
	XmNmarginWidth, 8, XmNmarginHeight, 4,
	NULL);

   XtVaCreateManagedWidget("Subpanel Offsets",
	xmLabelGadgetClass, offset_frame,
	XmNchildType, XmFRAME_TITLE_CHILD,
	NULL);

   rowcol = XtVaCreateWidget("rowcol",
	xmRowColumnWidgetClass, offset_frame,
	XmNpacking, XmPACK_COLUMN,
	XmNorientation, XmVERTICAL,
	XmNnumColumns, 1,
	NULL);

   str = XmStringCreateSimple("X Offset");
   r->x_scale = XtVaCreateManagedWidget("scale",
	xmScaleWidgetClass, rowcol,
	XmNorientation, XmHORIZONTAL,
	XmNshowValue, True,
	XmNminimum, -50, XmNmaximum, 50,
	XmNhighlightThickness, 0,
	XmNtitleString, str,
	XmNvalue, panel.subpanel_x_offset,
	NULL);
   XmStringFree(str);

   str = XmStringCreateSimple("Y Offset");
   r->y_scale = XtVaCreateManagedWidget("scale",
	xmScaleWidgetClass, rowcol,
	XmNorientation, XmHORIZONTAL,
	XmNshowValue, True,
	XmNminimum, -50, XmNmaximum, 50,
	XmNhighlightThickness, 0,
	XmNtitleString, str,
	XmNvalue, panel.subpanel_y_offset,
	NULL);
   XmStringFree(str);

   XtManageChild(rowcol);

   XtAddCallback(r->msgbox, XmNokCallback, gsedit_cb, r);
   XtAddCallback(r->msgbox, XmNcancelCallback, gsedit_cb, r);
   XtAddCallback(r->msgbox, XmNhelpCallback, gsedit_cb, r);

   XtManageChild(r->msgbox);
   XtManageChild(r->shell);
   XtPopup(r->shell, XtGrabExclusive);
}

void gsedit_cb(w,client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
GSEditRec *r = (GSEditRec*)client_data;
XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct *)call_data;

   switch(cbs->reason) {
      case XmCR_OK: gsedit_apply(r); XtDestroyWidget(r->shell); break;
      case XmCR_CANCEL: XtDestroyWidget(r->shell); break;
      case XmCR_HELP: gsedit_apply(r); break;
   }
}

void gsedit_apply(GSEditRec *r) {
int tmp_x, tmp_y;
SubpanelRec *s;
Boolean slideup;

   XtVaGetValues(r->x_scale, XmNvalue, &tmp_x, NULL);
   XtVaGetValues(r->y_scale, XmNvalue, &tmp_y, NULL);

   if (tmp_x != panel.subpanel_x_offset || tmp_y != panel.subpanel_y_offset) {
      for(s = subpanels.head; s != NULL; s = s->link)
         s->needs_coords = True;
      panel.subpanel_x_offset = tmp_x;   panel.subpanel_y_offset = tmp_y;
   }

   XtVaGetValues(r->slideup_model, XmNset, &slideup, NULL);

   if (slideup == True)
      panel.subpanel_model = SlideUp;
   else
      panel.subpanel_model = PopUp;
}

/*****************************************************************************/


void edit_subpanel_properties_cb(w,client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{



}

void edit_subpanel_properties(Widget parent, SubpanelRec *s) {
SEditRec *r;
Widget main_form, title_frame;

   r = (SEditRec*)malloc(sizeof(SEditRec));

   r->subpanel = s;

   r->shell = XtVaCreatePopupShell("SubpanelProperties",
        xmDialogShellWidgetClass, parent,
        XmNautoUnmanage, False,
        XmNtitle, "Subpanel Properties",
	XmNmwmDecorations, 0 | 2 | 8 | 16, 
        NULL);

   r->msgbox =  XtVaCreateWidget("message_box",
        xmMessageBoxWidgetClass, r->shell,
	XmNautoUnmanage, False,
	XtVaTypedArg, XmNhelpLabelString, XmRString, "Apply", strlen("Apply"),
	NULL);

   main_form = XtVaCreateManagedWidget("form",
	xmFormWidgetClass, r->msgbox,
	NULL);

   title_frame = XtVaCreateManagedWidget("title_frame",
	xmFrameWidgetClass, main_form,
	XmNtopAttachment, XmATTACH_FORM,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNmarginWidth, 8, XmNmarginHeight, 4,
	NULL);

   XtVaCreateManagedWidget("Title",
	xmLabelGadgetClass, title_frame,
	XmNchildType, XmFRAME_TITLE_CHILD,
	NULL);

   r->title_entry = XtVaCreateManagedWidget("title_entry",
	xmTextFieldWidgetClass, title_frame,
	XmNvalue, s->label,
	XmNhighlightThickness, 1,
	NULL);

   XtAddCallback(r->msgbox, XmNokCallback, sedit_cb, r);
   XtAddCallback(r->msgbox, XmNcancelCallback, sedit_cb, r);
   XtAddCallback(r->msgbox, XmNhelpCallback, sedit_cb, r);

   XtManageChild(r->msgbox);
   XtManageChild(r->shell);
   XtPopup(r->shell, XtGrabExclusive);
   XtAddGrab(r->shell, True, False); 
}

void sedit_apply(SEditRec *r) {
char *tmp;

   tmp = XmTextGetString(r->title_entry);
   if (r->subpanel->label != NULL)
      MdtStringFree(&r->subpanel->label);
   MdtStringSet(&r->subpanel->label, tmp);
   XtFree(tmp);

   XtVaSetValues(r->subpanel->shell,
	XmNtitle, r->subpanel->label,
	NULL);
}

void sedit_cb(w,client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
SEditRec *r = (SEditRec*)client_data;
XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct *)call_data;

   switch(cbs->reason) {
     case XmCR_OK: sedit_apply(r); XtDestroyWidget(r->shell); free(r); break;
     case XmCR_CANCEL: XtDestroyWidget(r->shell); free(r); break;
     case XmCR_HELP: sedit_apply(r); break;
   }
}

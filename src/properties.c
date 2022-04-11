
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

/* $Id: properties.c,v 1.1 1997/06/16 08:32:25 mbaron Exp mbaron $ */

#include "record.h"
#include "global.h"
#include "utils.h"
#include "contedit.h"
#include "subedit.h"
#include "properties.h"
#include "MdtDialog.h"
#include <Xm/XmAll.h>

typedef struct _Prop {
   Widget shell, msgbox, lock, pixmap_path, x_offset, y_offset, slideup,
	control_selector, subpanel_selector;
} Prop;

void properties_cancel_cb(Widget, XtPointer, XtPointer);
void properties_ok_cb(Widget, XtPointer, XtPointer);
void properties_save_cb(Widget, XtPointer, XtPointer);

Widget create_panel_properties(Prop*, Widget);
Widget create_control_properties(Prop*, Widget);
Widget create_subpanel_properties(Prop*, Widget);

void build_control_list(Prop*);
void build_subpanel_list(Prop*);
void control_selected_cb(Widget, XtPointer, XtPointer);

void edit_control_cb(Widget, XtPointer, XtPointer);
void edit_subpanel_cb(Widget, XtPointer, XtPointer);

void apply_panel_properties_cb(Widget, XtPointer, XtPointer);

void edit_properties(Widget parent) {
static Prop *p;
Widget form, control_area, panel_area, subpanel_area;
Dimension width;

   p = (Prop*)malloc(sizeof(Prop));

   p->shell = XtVaCreatePopupShell("Properties",
        xmDialogShellWidgetClass, parent,
        XmNautoUnmanage, False,
        XmNtitle, "Panel Properties",
	XmNmwmDecorations, 0 | 2 | 8 | 16, 
        NULL);

   p->msgbox =  XtVaCreateWidget("message_box",
        xmMessageBoxWidgetClass, p->shell,
	XmNautoUnmanage, False,
	XtVaTypedArg, XmNhelpLabelString, XmRString, "Save...", strlen("Apply"),
	NULL);

   form = XtVaCreateManagedWidget("form",
	xmFormWidgetClass, p->msgbox,
	NULL);

   panel_area = create_panel_properties(p, form);
   control_area = create_control_properties(p, form);
   subpanel_area = create_subpanel_properties(p, form);

   XtVaSetValues(panel_area,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	NULL);

   XtVaSetValues(control_area,
	XmNleftAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_WIDGET,
	XmNtopWidget, panel_area,
	NULL);

   XtVaSetValues(subpanel_area,
	XmNleftAttachment, XmATTACH_WIDGET,
	XmNleftWidget, control_area,
	XmNleftOffset, 6,
	XmNrightAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_WIDGET,
	XmNtopWidget, panel_area,
	XmNbottomAttachment, XmATTACH_FORM,
	NULL);

   XtAddCallback(p->msgbox, XmNcancelCallback, properties_cancel_cb, p);
   XtAddCallback(p->msgbox, XmNokCallback, properties_ok_cb, p);
   XtAddCallback(p->msgbox, XmNhelpCallback, properties_save_cb, p);

   XtManageChild(p->msgbox);
   XtManageChild(p->shell);

   XtPopup(p->shell, XtGrabNone); 
}

void properties_save_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
Prop *p = (Prop*)client_data;
char *msg;

   if (!write_panelrc(rc_file_name)) {
      MdtVaString(&msg, "Error saving '", rc_file_name, "'", NULL);
      MdtErrorDialog(p->shell, msg);
   }
   else {
      MdtVaString(&msg, "Saved '", rc_file_name, "'", NULL);
      MdtInfoDialog(p->shell, msg);
   }
 
   MdtStringFree(&msg);
}

void properties_cancel_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
Prop *p = (Prop*)client_data;

   XtPopdown(p->shell);
   XtDestroyWidget(p->shell);
   p = NULL;
   free(p);
}

void properties_ok_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
Prop *p = (Prop*)client_data;

   XtPopdown(p->shell);
   apply_panel_properties_cb(w, p, NULL);

   XtDestroyWidget(p->shell);

   free(p);
}

Widget create_panel_properties(Prop *p, Widget parent) {
int width=90;
Widget frame, main_form, rowcol, label, pixmap_path_form, lock_form, tmp;
XmString str;

   frame = XtVaCreateManagedWidget("panel_frame",
	xmFrameWidgetClass, parent,
	XmNshadowType, XmSHADOW_ETCHED_IN,
	XmNmarginWidth, 6, XmNmarginHeight, 6,
	NULL);

   XtVaCreateManagedWidget("General Attributes",
	xmLabelGadgetClass, frame,
	XmNchildType, XmFRAME_TITLE_CHILD,
	XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	NULL);

   rowcol = XtVaCreateManagedWidget("form",
	xmRowColumnWidgetClass, frame,
	XmNpacking, XmPACK_COLUMN,
	XmNorientation, XmVERTICAL,
	XmNnumColumns, 1,
	XmNspacing, 6,
	NULL);

   lock_form = XtVaCreateManagedWidget("form",
	xmFormWidgetClass, rowcol,
	NULL);

   label = XtVaCreateManagedWidget("Lock Command",
	xmLabelGadgetClass, lock_form,
	XmNleftAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	NULL);

   p->lock = XtVaCreateManagedWidget("lock_entry",
	xmTextFieldWidgetClass, lock_form,
	XmNhighlightThickness, 1,
	XmNvalue, panel.lock,
	XmNcolumns, 25,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_WIDGET,
	XmNtopWidget, label,
	NULL);

   pixmap_path_form = XtVaCreateManagedWidget("form",
	xmFormWidgetClass, rowcol,
	NULL);

   label = XtVaCreateManagedWidget("Pixmap Path",
	xmLabelGadgetClass, pixmap_path_form,
	XmNleftAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	NULL);

   p->pixmap_path = XtVaCreateManagedWidget("lock_entry",
	xmTextFieldWidgetClass, pixmap_path_form,
	XmNhighlightThickness, 1,
	XmNvalue, panel.pixmap_path,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_WIDGET,
	XmNtopWidget, label,
	NULL);
 
   return frame;
}

Widget create_control_properties(Prop *p, Widget parent) {
Widget frame, rowcol, form, label, button;

   frame = XtVaCreateManagedWidget("controls_frame",
	xmFrameWidgetClass, parent,
	XmNshadowType, XmSHADOW_ETCHED_IN,
	XmNmarginWidth, 8, XmNmarginHeight, 8,
	NULL);

   XtVaCreateManagedWidget("Controls",
	xmLabelGadgetClass, frame,
	XmNchildType, XmFRAME_TITLE_CHILD,
	XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	NULL);

   form = XtVaCreateManagedWidget("form",
	xmFormWidgetClass, frame,
	NULL);

   p->control_selector = XmCreateScrolledList(form,
	"control_selector", NULL, 0);

   XtVaSetValues(p->control_selector,
	XmNvisibleItemCount, 6,
	NULL);

   build_control_list(p);

   XtManageChild(p->control_selector);

   rowcol = XtVaCreateManagedWidget("rowcol",
	xmRowColumnWidgetClass, form,
	XmNpacking, XmPACK_COLUMN,
	XmNorientation, XmVERTICAL,
	XmNnumColumns, 1,
	NULL);

   button = XtVaCreateManagedWidget("Edit...",
	xmPushButtonGadgetClass, rowcol,
	NULL);

   XtAddCallback(button, XmNactivateCallback, edit_control_cb, p);

   button = XtVaCreateManagedWidget("New...",
	xmPushButtonGadgetClass, rowcol,
	XmNsensitive, False,
	NULL);

   button = XtVaCreateManagedWidget("Delete",
	xmPushButtonGadgetClass, rowcol,
	XmNsensitive, False,
	NULL);

   XtVaSetValues(XtParent(p->control_selector),
	XmNtopAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_WIDGET,
	XmNrightWidget, rowcol,
	NULL);

   XtVaSetValues(rowcol,
	XmNtopAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	NULL);

   return frame;
}

Widget create_subpanel_properties(Prop *p, Widget parent) {
Widget frame, rowcol, form, button;

   frame = XtVaCreateManagedWidget("subpanels_frame",
	xmFrameWidgetClass, parent,
	XmNshadowType, XmSHADOW_ETCHED_IN,
	XmNmarginWidth, 8, XmNmarginHeight, 8,
	NULL);

   XtVaCreateManagedWidget("Subpanels",
	xmLabelGadgetClass, frame,
	XmNchildType, XmFRAME_TITLE_CHILD,
	XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	NULL);

   form = XtVaCreateManagedWidget("form",
	xmFormWidgetClass, frame,
	NULL);

   p->subpanel_selector = XmCreateScrolledList(form,
	"subpanel_selector", NULL, 0);

   XtVaSetValues(p->subpanel_selector,
	XmNvisibleItemCount, 6,
	NULL);

   build_subpanel_list(p);

   XtManageChild(p->subpanel_selector);

   rowcol = XtVaCreateWidget("rowcol",
	xmRowColumnWidgetClass, form,
	XmNpacking, XmPACK_COLUMN,
	XmNorientation, XmVERTICAL,
	XmNnumColumns, 1,
	NULL);

   button = XtVaCreateManagedWidget("Edit...",
	xmPushButtonGadgetClass, rowcol,
	NULL);
   XtAddCallback(button, XmNactivateCallback, edit_subpanel_cb, p);

   button = XtVaCreateManagedWidget("New...",
	xmPushButtonGadgetClass, rowcol,
	XmNsensitive, False,
	NULL);

   button = XtVaCreateManagedWidget("Delete",
	xmPushButtonGadgetClass, rowcol,
	XmNsensitive, False,
	NULL);

   button = XtVaCreateManagedWidget("Global...",
	xmPushButtonGadgetClass, rowcol,
	NULL);
   XtAddCallback(button, XmNactivateCallback,
	edit_global_subpanel_properties_cb, NULL);

   XtVaSetValues(rowcol,
	XmNtopAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	NULL);

   XtVaSetValues(XtParent(p->subpanel_selector),
	XmNtopAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_WIDGET,
	XmNrightWidget, rowcol,
	NULL);

   XtManageChild(rowcol);   

   return frame;
}

void apply_panel_properties_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
Prop *p = (Prop*)client_data;
char *tmp;
int tmp_x, tmp_y;

   tmp = XmTextGetString(p->lock);
   if (panel.lock != NULL)
      MdtStringFree(&panel.lock);
   MdtStringSet(&panel.lock, tmp);
   XtFree(tmp);

   tmp = XmTextGetString(p->pixmap_path);
   if (panel.pixmap_path != NULL)
      MdtStringFree(&panel.pixmap_path);
   MdtStringSet(&panel.pixmap_path, tmp);
   XtFree(tmp);
}

void build_subpanel_list(Prop *p) {
SubpanelRec *s = subpanels.head;
XmString str;

   while(s != NULL) {
      str = XmStringCreateSimple(s->id);
      XmListAddItemUnselected(p->subpanel_selector, str, 0);
      XmStringFree(str);
      s = s->link;
   }

}

void build_control_list(Prop *p) {
ControlRec *c = controls.head;
XmString str;

   while(c != NULL) {
      if (c->control_type == Icon) {
         str = XmStringCreateSimple(c->id);
         XmListAddItemUnselected(p->control_selector, str, 0);
         XmStringFree(str);
      }
      c = c->link;
   }
}

void edit_control_cb(w,client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
ControlRec *c;
Prop *p = (Prop*)client_data;
XmStringTable strs;
char *control_id;
int selected_count;

   XtVaGetValues(p->control_selector,
	XmNselectedItems, &strs,
	XmNselectedItemCount, &selected_count,
	NULL);

   if (selected_count < 1) {
      MdtErrorDialog(p->msgbox, "Please select a control");
      return;      
   }

   XmStringGetLtoR(strs[0], XmFONTLIST_DEFAULT_TAG, &control_id);

   c = lookup_control(controls, control_id);
 
   if (c == NULL) {
      MdtErrorDialog(p->msgbox, "Error in control lookup");
      return;
   }

   edit_control(p->msgbox, c);
}

void edit_subpanel_cb(w,client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
SubpanelRec *s;
Prop *p = (Prop*)client_data;
XmStringTable strs;
char *subpanel_id;
int selected_count;

   XtVaGetValues(p->subpanel_selector,
	XmNselectedItems, &strs,
	XmNselectedItemCount, &selected_count,
	NULL);

   if (selected_count < 1) {
      MdtErrorDialog(p->msgbox, "Please select a subpanel");
      return;      
   }

   XmStringGetLtoR(strs[0], XmFONTLIST_DEFAULT_TAG, &subpanel_id);

   s = lookup_subpanel(subpanels, subpanel_id);
 
   if (s == NULL) {
      MdtErrorDialog(p->msgbox, "Error in subpanel lookup");
      return;
   }

   edit_subpanel_properties(p->msgbox, s);
}

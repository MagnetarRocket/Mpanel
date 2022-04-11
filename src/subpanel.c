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

/* $Id: subpanel.c,v 0.7 1997/05/21 19:07:00 mbaron Exp mbaron $ */

#include "subpanel.h"
#include "global.h"
#include "utils.h"
#include "syspixmaps.h"
#include "record.h"
#include "table.h"
#include "callbacks.h"
#include "xincludes.h"
#include <unistd.h>

#define SLIDE_INCREMENT 15

void wm_delete_cb(Widget,XtPointer,XtPointer);
void subpanel_cb(Widget, XtPointer, XtPointer);
void calculate_subpanel_coords(SubpanelRec*);
void position_subpanel(SubpanelRec*);

#ifdef _CUSTOM_TITLE_BARS

Widget create_title_bar(Widget parent, char *title_str) {
Widget form, decor, title, frame, groove;
XmString str;

   frame = XtVaCreateManagedWidget("SubpanelTitle",
	xmFrameWidgetClass, parent,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	XmNshadowType, XmSHADOW_OUT,
	XmNshadowThickness, 2,
	XmNmarginWidth, 0,
	XmNmarginHeight, 0,
	NULL);

   form = XtVaCreateManagedWidget("",
	xmFormWidgetClass, frame,
	XmNmarginWidth, 0,
	XmNmarginHeight, 0,
	NULL);

   create_xpm_data(form, titledecor_xpm, &pixmaps.menu_decor);

   decor = XtVaCreateManagedWidget("menu",
	xmPushButtonWidgetClass, form,
        XmNlabelType, XmPIXMAP,
        XmNlabelPixmap, pixmaps.menu_decor,
        XmNhighlightThickness, 0,
        XmNborderWidth, 0,
	XmNshadowThickness, 0,
	XmNmarginHeight, 0,
	XmNmarginTop, 0,
	XmNmarginBottom, 0,
	XmNhighlightOnEnter, False,
	XmNleftAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	NULL);

   groove = XtVaCreateManagedWidget("separator",
	xmSeparatorWidgetClass, form,
	XmNorientation, XmVERTICAL,
	XmNleftAttachment, XmATTACH_WIDGET,
	XmNleftWidget, decor,
	XmNtopAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	NULL);

   title = XtVaCreateManagedWidget("TitleLabel",
	xmLabelWidgetClass, form,
	XmNmarginWidth, 0, XmNmarginHeight, 0,
	XmNleftAttachment, XmATTACH_WIDGET,
	XmNleftWidget, groove,
	XmNrightAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNalignment, XmALIGNMENT_CENTER,
	XtVaTypedArg, XmNlabelString, XmRString, title_str, strlen(title_str),
	NULL);

   return frame;	
}
#endif


Widget create_install_icon(Widget parent, Widget top_attach) {
Widget item_form, inner, outer, sep, label;
XmString str;

   item_form =  XtVaCreateManagedWidget("InstallIcon",
	xmFormWidgetClass, parent,
	XmNtopAttachment, XmATTACH_WIDGET,
	XmNtopWidget, top_attach,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	NULL);

   outer = XtVaCreateManagedWidget("Outer",
	xmFrameWidgetClass, item_form,
	XmNshadowType, XmSHADOW_IN,
	XmNleftAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNleftOffset, 10,
	NULL);

   inner = XtVaCreateManagedWidget("Inner",
	xmFrameWidgetClass, outer,
	XmNshadowType, XmSHADOW_IN,
	XmNwidth, 30,
	XmNheight,20,
	NULL);

   str = XmStringCreateSimple("Install Icon");

   label = XtVaCreateManagedWidget("Label",
	xmLabelWidgetClass, item_form,
	XmNlabelString, str,
	XmNleftAttachment, XmATTACH_WIDGET,
	XmNleftWidget, outer,
	XmNtopAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNalignment, XmALIGNMENT_BEGINNING,
	XmNleftOffset, 10,
	NULL);

   XtOverrideTranslations(label, no_drag);

   XmStringFree(str);

   sep = XtVaCreateManagedWidget("",
	xmSeparatorWidgetClass, parent,
	XmNorientation, XmHORIZONTAL,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_WIDGET,
	XmNtopWidget, item_form,
	NULL);

   return sep;
}

void create_subpanel(SubpanelRec *s) {
Widget content_frame, outside_form, content_form, install_icon, title;
Atom WM_DELETE_WINDOW;
int decor=0;

#ifdef _CUSTOM_TITLE_BARS
   decor = 0;
#else
   decor = MWM_DECOR_MENU | MWM_DECOR_TITLE;
#endif

   s->shell = XtVaCreateWidget("Subpanel",
	topLevelShellWidgetClass, layout.toplevel,
	XmNscreen, screen,
	XmNmwmDecorations, decor,
        XmNtitle, (s->label != NULL ? s->label : "Untitled"),
	XmNallowShellResize, True,
	NULL);

   outside_form = XtVaCreateManagedWidget("Subpanel",
	xmFormWidgetClass, s->shell,
	XmNmarginWidth, 0,
	XmNmarginHeight, 0,
	NULL);

#ifdef _CUSTOM_TITLE_BARS
   title = create_title_bar(outside_form, s->label);
#else
   title = s->shell;
#endif

   content_frame = XtVaCreateManagedWidget("SubpanelMain",
	xmFrameWidgetClass, outside_form,
	XmNshadowType, XmSHADOW_OUT,
	XmNshadowThickness, 1,
	XmNmarginWidth, 0,
	XmNmarginHeight, 0,
	XmNtopAttachment, XmATTACH_WIDGET,
	XmNtopWidget, title,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	NULL);

   content_form = XtVaCreateManagedWidget("",
	xmFormWidgetClass, content_frame,
	XmNmarginWidth, 0,
	XmNmarginHeight, 0,
	NULL);
	
   install_icon = create_install_icon(content_form, title);

   s->content = XtVaCreateManagedWidget("Subpanel",
	xmRowColumnWidgetClass, content_form,
	XmNmarginWidth, 6,
	XmNmarginHeight, 6,
	XmNtopAttachment, XmATTACH_WIDGET,
	XmNtopWidget, install_icon,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNpacking, XmPACK_COLUMN,
	XmNorientation, XmVERTICAL,
	XmNnumColumns, 1,
	XmNbottomOffset, 4,
	XmNspacing, 6,
	NULL);

   WM_DELETE_WINDOW = XmInternAtom(display, "WM_DELETE_WINDOW", False);
   XmAddWMProtocols(s->shell, &WM_DELETE_WINDOW, 1);
   XmAddWMProtocolCallback(s->shell, WM_DELETE_WINDOW, wm_delete_cb, s);
}

void subpanel_slideup(SubpanelRec *s) {
Dimension height;
int y;
Window topw = XtWindowOfObject(layout.toplevel);
Window menuw = XtWindowOfObject(s->shell);


   XtVaGetValues(s->shell, XmNheight, &height, NULL);

   /*
    *  Place the subpanel so that is below the subpanel arrow in
    *  terms of y screen coordinates
    */
   XtVaSetValues(s->shell, XmNx, s->x, XmNy, s->y + height, NULL);

   /* Make the supanel appear on the screen */
   XtPopup(s->shell, XtGrabNone);
  
   /*
    *  Raise the front panel proper so that it appears that the subpanel
    *  is sliding up from within the front panel instead of sliding
    *  up from behind the front panel
    */
   XRaiseWindow(display, topw);
   y = s->y + height;

   /* Incrementally raise the subpanel in terms of y screen coords */
   while(y > s->y) {
      XMoveWindow(display, menuw, s->x, y);
      XmUpdateDisplay(s->shell);
      XSync(display, 0);
      y -= SLIDE_INCREMENT;
   }
}

void subpanel_slidedown(SubpanelRec *s) {
Dimension height;
int start_y, stop_y;
Window topw = XtWindowOfObject(layout.toplevel);
Window menuw = XtWindowOfObject(s->shell);

   XtVaGetValues(s->shell, XmNheight, &height, NULL);

   start_y = s->y;
   stop_y = s->y + height;

   /* 
    * Make sure the toplevel widget is on top so that it appears the
    * subpanel is going back down into the front panel
    */
   XRaiseWindow(display, topw);

   /* Loop lowering the subpanel on the screen each iteration */
   while(start_y < stop_y) {
      start_y += SLIDE_INCREMENT;
      XMoveWindow(display, menuw, s->x, start_y);
      XmUpdateDisplay(s->shell);
      XSync(display, 0);
   }

   /* Remove the subpanel shell from the display */
   XtPopdown(s->shell);
}

void open_subpanel(SubpanelRec *s) {

   if (panel.subpanel_model == SlideUp) {
      subpanel_slideup(s);
      position_subpanel(s);
   } else {
      position_subpanel(s);
      XtPopup(s->shell, XtGrabNone);
      position_subpanel(s);
   }

   XtVaSetValues(s->over, XmNlabelPixmap, pixmaps.down_arrow, NULL);
   s->state = 1;
}

void close_subpanel(SubpanelRec *s) {

   if (panel.subpanel_model == SlideUp)
      subpanel_slidedown(s);
   else
      XtPopdown(s->shell);

   XtVaSetValues(s->over, XmNlabelPixmap, pixmaps.up_arrow, NULL);

   s->state = 0;
}

void subpanel_arrow_click(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
SubpanelRec *s = (SubpanelRec*)client_data;
XButtonPressedEvent *event = (XButtonPressedEvent*)call_data;

   /* 
    *  Some day this proc will hav code that checks to see if 3rd button was 
    *  clicked and resent a menu with options for editing, opening, etc.
    */

   if (event->button != 1) return;

   if(s->needs_coords)
      calculate_subpanel_coords(s);

   if (!s->state)
      open_subpanel(s);
   else 
      close_subpanel(s);   
}

void bind_subpanel_arrow(SubpanelRec *s) {
   XtAddEventHandler(s->over, ButtonPressMask, False,
        (void*)subpanel_arrow_click, s);   
}

void wm_delete_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
   close_subpanel((SubpanelRec*)client_data);
}

void position_subpanel(SubpanelRec *s) {
Window subpanel_w = XtWindowOfObject(s->shell);  

    XMoveWindow(display, subpanel_w, s->x, s->y);
    XRaiseWindow(display, subpanel_w);
}

void calculate_subpanel_coords(SubpanelRec *s) {
short right_adjust=0;
Dimension x, lip_h, left_h, top_y, shell_h;
Widget over = s->over;

   s->x = s->y = 0;
 
   /* Find the absolute x coord of the lower left corner of the subpanel */
   while(over && over != layout.toplevel) {
       XtVaGetValues(over, XmNx, &x, NULL); 
       s->x += x; 
       if(over == layout.right) right_adjust = 1;
       over = XtParent(over);
    } 

    if (right_adjust) {
       Dimension shell_width, over_width;
       XtVaGetValues(s->shell, XmNwidth, &shell_width, NULL);
       XtVaGetValues(s->over, XmNwidth, &over_width, NULL);
       s->x = s->x - shell_width + over_width - panel.subpanel_x_offset;
    } else {
       s->x += panel.subpanel_x_offset;
    }

   /* Find the absolute y coord of the lowerleft corner of the subpanel */
   /* Anyone know a better way to do this? */
   XtVaGetValues(layout.toplevel, XmNy, &top_y, NULL);
   XtVaGetValues(s->shell, XmNheight, &shell_h, NULL);
   XtVaGetValues(layout.lip, XmNheight, &lip_h, NULL);
   XtVaGetValues(layout.left, XmNheight, &left_h, NULL);

   s->y = (top_y - shell_h + (lip_h - left_h)/2 - 1) + panel.subpanel_y_offset;

   position_subpanel(s);
   s->needs_coords = False;
}

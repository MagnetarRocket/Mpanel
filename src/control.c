/*
 * Panel - CDE Front Panel Emulator
 *
 * Copyright (C) 1997 Matthew Baron
 * Copyright (C) 2004 Sergey Sharashkin
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

/* $Id: control.c,v 0.9 1997/06/11 07:14:33 mbaron Exp mbaron $ */

#include "control.h"
#include "xincludes.h"
#include <stdlib.h>
#include <unistd.h>

#include "syspixmaps.h"
#include "utils.h"
#include "record.h"
#include "global.h"
#include "subpanel.h"
#include "table.h"
#include "callbacks.h"
#include "biff.h"
#include "cclock.h"
#include "properties.h"

/* 
 * Seconds to wait before closing a subpanel after one of it's controls has
 * been clicked on
 */
#define SUBPANEL_CLOSE_DELAY 1

void handle_subpanel_click(Widget, XtPointer, XtPointer);
void control_clicked_cb(Widget, XtPointer, XtPointer);

Widget create_control_top(ControlRec*, Widget, Boolean);
void highlight_cb(Widget, XtPointer, XtPointer);
void unhighlight_cb(Widget, XtPointer, XtPointer);

int create_control(ControlRec *c) {
Pixmap menu_pixmap;
Widget form, top;

#ifdef DEBUG
	printf("create_control started\n");
#endif

   form =  XtVaCreateManagedWidget("Control",
	xmFormWidgetClass, c->parent_widget,
	NULL);

#ifdef DEBUG
	printf("create_control: form created\n");
#endif

   /* We only add the top part if the control is in a Box */
   if (c->container_type == Box) {
      int num_children;
      XtVaGetValues(c->parent_widget, XmNnumChildren, &num_children, NULL);
      top = create_control_top(c, form, (num_children > 1 ? False : True));
   } 

#ifdef DEBUG
	printf("create_control: box complet\n");
#endif

   if (c->control_type == Blank)
      return 1;

#ifdef DEBUG
	printf("create_control: blank complet\n");
#endif

   /*
    * Here we need to set up the border with of the subpanel control so that
    * when the control is entered with the mouse, a border is drawn around
    * the entire subpanel entry (the pixmap and the label).  This seems
    * like an awful way to do it, but it's the only way I could come up with.
    */
   if (c->container_type == Subpanel)
      top = form;

#ifdef DEBUG
	printf("create_control: subpanel complet\n");
#endif

   if (c->control_type == Clock) {
      Widget clock;
      /*clock_create_digital(form, top);*/
#ifdef DEBUG
	printf("create_control: enter to clock\n");
#endif
      create_xpm_file(c->parent_widget, c->icon, &c->pixmap);
#ifdef DEBUG
	printf("create_control: xpm created\n");
#endif
      clock = clock_create_analog(form, c->pixmap);
#ifdef DEBUG
	printf("create_control: clock created\n");
#endif
     XtVaSetValues(clock,
	XmNtopAttachment, XmATTACH_WIDGET,
	XmNtopWidget, top,
	NULL);
      return 1;
   }

#ifdef DEBUG
	printf("create_control: before pixmap\n");
#endif

   if (!create_xpm_file(c->parent_widget, c->icon, &c->pixmap))
      printf("Unable to resolve pixmap '%s' (control)\n", c->icon);

   /* Create the push button that represents the icon */
   c->icon_widget = button_with_pixmap(form, (Pixmap*)c->pixmap,
	"ControlIcon");

   XtVaSetValues(c->icon_widget,
	XmNtopAttachment, XmATTACH_WIDGET,
	XmNtopWidget, top,
	XmNleftAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	NULL);

   XtOverrideTranslations(c->icon_widget, no_drag);
 
   /* If we are dealing with a Subpanel, add a label */
   if (c->container_type == Subpanel && c->label != NULL) {
      Pixel color;

      c->label_widget = flat_button_with_text(form, c->label, "Label");

      XtVaSetValues(c->label_widget,
      	XmNleftAttachment, XmATTACH_WIDGET,
      	XmNleftWidget, c->icon_widget,
      	XmNrightAttachment, XmATTACH_FORM,
      	XmNtopAttachment, XmATTACH_FORM,
      	XmNbottomAttachment, XmATTACH_FORM,
	XmNalignment, XmALIGNMENT_BEGINNING,
	XmNmarginRight, 4,
      	NULL);

      XtVaSetValues(c->icon_widget,
	XmNhighlightThickness, 0,
	NULL);

      XtAddCallback(c->label_widget, XmNactivateCallback,
	control_clicked_cb, c);
   }

   XtAddCallback(c->icon_widget, XmNactivateCallback, control_clicked_cb, c);

   if (c->control_type == Biff) biff_init(c);

#ifdef DEBUG
	printf("create_control complet\n");
#endif

   return 1;
}

Widget create_control_top(ControlRec *c, Widget parent, Boolean first) {
Widget top_form, bs, ls;
Pixel highlight;

#ifdef DEBUG
	printf("create_control_top started\n");
#endif

   top_form = XtVaCreateManagedWidget("TopForm",
        xmFormWidgetClass, parent,
        XmNborderWidth, 0,
	XmNtopAttachment, XmATTACH_FORM,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
        NULL);

   /*
    *  We cheat a little bit here to get the bottom of the arrow widget to 
    *  look sunken compared to the area that the control is in
    */ 
   XtVaGetValues(top_form, XmNtopShadowColor, &highlight, NULL);

   bs = XtVaCreateManagedWidget("separator",
	xmSeparatorWidgetClass, top_form,
	XmNorientation, XmHORIZONTAL,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
        XmNseparatorType, XmSINGLE_LINE,
	XmNforeground, highlight,
        XmNshadowThickness, 1,
	NULL);

   if (first == False)
      ls = XtVaCreateManagedWidget("separator",
	xmSeparatorWidgetClass, top_form,
	XmNorientation, XmVERTICAL,
	XmNtopAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_WIDGET,
	XmNbottomWidget, bs,
	XmNleftAttachment, XmATTACH_FORM,
	NULL);
   else
      ls = top_form;

   /*
    * By default, we put a blank pixmap in the control top.  When the subpanels
    * are resolved and attached to their respective controls, a pixmap with
    * an arrow replaces this pixmap.
    */

   if (c->control_type == Blank)
      c->arrow = button_with_pixmap(top_form, 
	(Pixmap*)pixmaps.spacer ,"SubArrow");
   else
      c->arrow = button_with_pixmap(top_form, 
	(Pixmap*)pixmaps.blank_arrow ,"SubArrow");

   XtVaSetValues(c->arrow, 
	XmNtopAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_WIDGET,
	XmNbottomWidget, bs,
	XmNleftAttachment, XmATTACH_WIDGET,
	XmNleftWidget, ls,
	XmNrightAttachment, XmATTACH_FORM,
	XmNalignment, XmALIGNMENT_CENTER,
	XmNhighlightThickness, 0,
	XmNrecomputeSize, False,
	NULL);
#ifdef DEBUG
	printf("create_control_top complet\n");
#endif

   return top_form;
}

void create_system_controls() {
Widget lock, exit;

   create_xpm_data(layout.center_left, lock_xpm, (Pixmap*)&pixmaps.lock);
   lock = button_with_pixmap(
	layout.center_left, (Pixmap*)pixmaps.lock, "Lock");

   create_xpm_data(layout.center_left, exit_xpm, (Pixmap*)&pixmaps.exit);
   exit = button_with_pixmap(
	layout.center_right, (Pixmap*)pixmaps.exit, "Exit");

   XtAddCallback(lock, XmNactivateCallback, execute_str, panel.lock);
   XtAddCallback(exit, XmNactivateCallback, exit_panel, NULL);
}

void control_clicked_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
ControlRec *c = (ControlRec*)client_data;
  /* 
   * Some day there will be more to this routine like checking if we are
   * executing some internal command, but for now we just run a UNIX(R)
   * command line
   */

   if (c->click_action == NULL)
      return;

   if (c->click_action[0] == 'F' && c->click_action[1] == 'p' &&
       !strcmp(c->click_action, "FpProperties")) {
      edit_properties(layout.toplevel);
      return;
   }
      

   execute_str(w, c->click_action, NULL);

   if (c->container_type == Subpanel && c->subpanel) {
      sleep(SUBPANEL_CLOSE_DELAY);
      close_subpanel(c->subpanel);
   }
}

void highlight_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
Widget win = (Widget)client_data;
Pixel color;

   XtVaGetValues(win, XmNuserData, &color, NULL);
   XtVaSetValues(win,
	XmNshadowType, XmSHADOW_ETCHED_OUT,
	XmNborderWidth, 1,
	XmNborderColor, color,
	NULL);
}

void unhighlight_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{     
Pixel color;

   XtVaGetValues(w, XmNbackground, &color, NULL);
   XtVaSetValues((Widget)client_data, XmNborderColor, color, NULL); 
}

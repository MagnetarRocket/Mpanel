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

/* $Id: render.c,v 0.7 1997/05/21 19:07:43 mbaron Exp mbaron $ */

#include <stdio.h>

#include "global.h"
#include "record.h"
#include "table.h"
#include "intro.h"

void render_desk_switch() {
int i=0;
Widget desk;

   update_intro("Creating deskswitch...", "");

   for(i = 0; i < desks.num; i++) {
      new_deskswitch_button(layout.center_center, desks.list[i]);
   }

   XtManageChild(layout.center_center);
}

void center_the_deskswitch() {
Dimension switch_height, buttons_height;
int offset;

   XtVaGetValues(layout.left, XmNheight, &switch_height, NULL);
   XtVaGetValues(layout.center_center, XmNheight, &buttons_height, NULL);

   offset = (switch_height - buttons_height)/2;

   XtVaSetValues(layout.center_center,XmNtopOffset, offset, NULL);
   XtVaSetValues(layout.center_left, XmNtopOffset, offset, NULL);
   XtVaSetValues(layout.center_right, XmNbottomOffset, offset, NULL);
}

void render_box_control(ControlRec *c) {
#ifdef DEBUG
	printf("render_box_control started\n");
#endif
	if (!strcmp(c->container_name, "Left"))
      c->parent_widget = layout.left;
   else
   if (!strcmp(c->container_name, "Right"))
      c->parent_widget = layout.right;
   else {
      fprintf(stderr, "Invalid container for control '%s'\n", c->id);
      return;
   }

   create_control(c);
#ifdef DEBUG
	printf("render_box_control complet\n\n");
#endif
}

void render_subpanel_control(ControlRec *c) {
SubpanelRec *s;

#ifdef DEBUG
	printf("render_subpanel_control started\n");
#endif

   s = lookup_subpanel(subpanels, c->container_name);

   if (s == NULL) {
      fprintf(stderr, "Invalid container for control '%s'\n", c->id);
      return;
   }
  
   c->parent_widget = s->content;
   c->subpanel = s;
   create_control(c);
#ifdef DEBUG
	printf("render_subpanel_control complet\n\n");
#endif
}

void render_panel() {
int i;
ControlRec *c, *control_ptr;
SubpanelRec *subpanel_ptr;
Pixmap p;

   /* 
    * First we walk through all of the controls and create each one
    * resolving the container in which the control resides.
    */

   control_ptr = controls.head;

   while(control_ptr != NULL) {
      switch(control_ptr->container_type) {
         case Box: render_box_control(control_ptr); break;
         case Subpanel: render_subpanel_control(control_ptr); break;
         default: fprintf(stderr, "Invalid container for control '%s'\n",
		control_ptr->id);
      }
      update_intro("Creating control", control_ptr->id);
      control_ptr = control_ptr->link;
   }

   /*
    * Now we walk through the subpanels, and figure out over which control the
    * subpanel should slide up.
    */
   subpanel_ptr = subpanels.head;

   while(subpanel_ptr != NULL) {

      c =  lookup_control(controls, subpanel_ptr->container_name);

      if (c == NULL) {
         fprintf(stderr, "Couldn't resolve subpanel '%s'\n", subpanel_ptr->id);
         continue;
      }

      /*
       * Change the pixmap of the top part of the control to have an up arrow
       * instead of the default blank arrow.
       */
      XtVaSetValues(c->arrow,
	XmNlabelPixmap, pixmaps.up_arrow,
	XmNhighlightThickness, 1,
	NULL);

      /* 
       * This is how we connect the subpanel to the control. The subpanel has
       *  a pointer to the top part of the control over which it slides up.
       */
      subpanel_ptr->over = c->arrow;

      /* 
       * Add a callback for the subpanel arrow that makes the subpanel slide
       * slide up when it is clicked.
       */
      bind_subpanel_arrow(subpanel_ptr);

      /* Finally, we manage the rowcolumn widget for the subpanel */
      XtManageChild(subpanel_ptr->content);

      subpanel_ptr = subpanel_ptr->link;
   }
}

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

/* $Id: layout.c,v 1.4 1997/06/11 21:46:33 mbaron Exp mbaron $ */

#include "layout.h"
#include "xincludes.h"

#include "utils.h"
#include "syspixmaps.h"
#include "global.h"
#include "callbacks.h"
#include "intro.h"

#define RIB_HEIGHT 2

void create_ribs(LayoutRec *l) {
Dimension left_height, decor_height;
int ribbing_height, curr_height=0;
Widget left_top = l->lribs, right_top = l->rribs, rib;

   update_intro("Creating ribs...", "");

   XtVaGetValues(l->left, XmNheight, &left_height, NULL);
   XtVaGetValues(l->ldecor, XmNheight, &decor_height, NULL);
 
   ribbing_height = left_height - decor_height;

   for(;curr_height <= ribbing_height; curr_height += RIB_HEIGHT) {
      rib = XtVaCreateManagedWidget("ribbing",
	xmSeparatorGadgetClass, l->lribs,
	XmNorientation, XmHORIZONTAL,
	XmNtopAttachment, XmATTACH_WIDGET,
	XmNtopWidget, left_top,
	XmNrightAttachment, XmATTACH_FORM,
	XmNleftAttachment, XmATTACH_FORM,
	NULL);
      left_top = rib;

      rib = XtVaCreateManagedWidget("ribbing",
	xmSeparatorGadgetClass, l->rribs,
	XmNorientation, XmHORIZONTAL,
	XmNtopAttachment, XmATTACH_WIDGET,
	XmNtopWidget, right_top,
	XmNrightAttachment, XmATTACH_FORM,
	XmNleftAttachment, XmATTACH_FORM,
	NULL);
      right_top = rib;
   }

   XtManageChild(l->lribs);
   XtManageChild(l->rribs);
}

void create_handles(LayoutRec *l) {
Widget lseparator, rseparator;

   create_xpm_data(l->lhandle, leftmenu_xpm, &pixmaps.left_decor);
   create_xpm_data(l->rhandle, rightmenu_xpm, &pixmaps.right_decor);

   rseparator = XtVaCreateManagedWidget("ribbing",
	xmSeparatorWidgetClass, l->rhandle,
	XmNorientation, XmVERTICAL,
	XmNtopAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNleftAttachment, XmATTACH_FORM,
	XmNmarginWidth, 0,
	XmNmarginHeight, 0,
	NULL);   

   lseparator = XtVaCreateManagedWidget("ribbing",
	xmSeparatorWidgetClass, l->lhandle,
	XmNorientation, XmVERTICAL,
	XmNtopAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNmarginWidth, 0,
	XmNmarginHeight, 0,
	NULL);

   l->ldecor = XtVaCreateManagedWidget("LeftDecor",
        xmLabelWidgetClass, l->lhandle,
        XmNlabelType, XmPIXMAP,
        XmNlabelPixmap, pixmaps.left_decor,
	XmNtopAttachment, XmATTACH_FORM,
        XmNhighlightThickness, 0,
        XmNborderWidth, 0,
	XmNmarginWidth, 0,
	XmNmarginHeight, 0,
        NULL);

      l->rdecor = XtVaCreateManagedWidget("RightDecor",
        xmLabelWidgetClass, l->rhandle,
        XmNlabelType, XmPIXMAP,
        XmNlabelPixmap, pixmaps.right_decor,
	XmNtopAttachment, XmATTACH_FORM,
        XmNhighlightThickness, 0,
        XmNborderWidth, 0,
	XmNmarginWidth, 0,
	XmNmarginHeight, 0,
        NULL);

   FakeCallback(l->rdecor, iconify_panel, NULL);

   l->lribs = XtVaCreateWidget("LeftRibs",
   	xmFormWidgetClass, l->lhandle,
   	XmNtopAttachment, XmATTACH_WIDGET,
   	XmNtopWidget, l->ldecor,
   	XmNleftAttachment, XmATTACH_FORM,
   	XmNrightAttachment, XmATTACH_WIDGET,
   	XmNrightWidget, lseparator,
	XmNbottomAttachment, XmATTACH_FORM,
   	XmNmarginWidth, 0, XmNmarginHeight, 0,
   	NULL);

   l->rribs = XtVaCreateWidget("RightRibs",
   	xmFormWidgetClass, l->rhandle,
   	XmNtopAttachment, XmATTACH_WIDGET,
   	XmNtopWidget, l->rdecor,
   	XmNrightAttachment, XmATTACH_FORM,
   	XmNleftAttachment, XmATTACH_WIDGET,
   	XmNleftWidget, rseparator,
	XmNbottomAttachment, XmATTACH_FORM,
   	XmNmarginWidth, 0, XmNmarginHeight, 0,
   	NULL);
}

void init_layout(LayoutRec *l, Widget parent) {
Widget inside_lip, main, center_inside, center_frame;

   l->lip = XtVaCreateManagedWidget("Lip",
	xmFrameWidgetClass, parent,
	XmNshadowType, XmSHADOW_OUT,
	XmNshadowThickness, 2,
	NULL);

   inside_lip = XtVaCreateManagedWidget("",
	xmFrameWidgetClass, l->lip,
	XmNshadowType, XmSHADOW_OUT,
	XmNshadowThickness, 2,
	XmNmarginWidth, 0,
	XmNmarginHeight, 0,
	NULL);

   main = XtVaCreateManagedWidget("",
	xmFormWidgetClass, inside_lip,
	XmNmarginWidth, 0,
	XmNmarginHeight, 0,
	NULL);

   l->lhandle = XtVaCreateWidget("Handle",
	xmFormWidgetClass, main,
	XmNleftAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNmarginWidth, 0,
	XmNmarginHeight, 0,
	NULL);

   l->rhandle = XtVaCreateWidget("Handle",
	xmFormWidgetClass, main,
	XmNrightAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNmarginWidth, 0,
	XmNmarginHeight, 0,
	NULL);

   center_frame  = XtVaCreateManagedWidget("",
	xmFrameWidgetClass, main,
	XmNshadowType, XmSHADOW_OUT,
	XmNshadowThickness, 1,
	XmNmarginWidth, 0,
	XmNmarginHeight, 0,
	NULL);

   center_inside = XtVaCreateManagedWidget("Center",
	xmFormWidgetClass, center_frame,
	XmNmarginWidth, 0,
	XmNmarginHeight, 0,
	NULL);

   l->center_left = XtVaCreateManagedWidget("CenterLeft",
	xmRowColumnWidgetClass, center_inside,
	XmNorientation, XmVERTICAL,
	XmNcolumns, 1,
	XmNmarginHeight, 0, 
	NULL);

   l->center_center = XtVaCreateWidget("CenterCenter",
	xmRowColumnWidgetClass, center_inside,
	XmNtopAttachment, XmATTACH_FORM,
	XmNorientation, XmHORIZONTAL,
	XmNpacking, XmPACK_COLUMN,
	XmNnumColumns, 2,
	XmNmarginWidth, 0,
	XmNmarginHeight, 0,
	XmNadjustLast, False,
	NULL);

   l->center_right = XtVaCreateManagedWidget("CenterRight",
	xmRowColumnWidgetClass, center_inside,
	XmNorientation, XmVERTICAL,
	XmNcolumns, 1,
	XmNmarginHeight, 0,
	NULL);

   l->left = XtVaCreateManagedWidget("Left",
	xmRowColumnWidgetClass, main,
	XmNorientation, XmHORIZONTAL,
	XmNcolumns, 1,
	XmNentryBorder,0,
	XmNspacing, 0,
	XmNmarginWidth, 0,
	XmNmarginHeight, 0,
	NULL);

   l->right = XtVaCreateManagedWidget("Right",
	xmRowColumnWidgetClass, main,
	XmNorientation, XmHORIZONTAL,
	XmNcolumns, 1,
	XmNentryBorder,0,
	XmNspacing, 0,
	XmNmarginWidth, 0,
	XmNmarginHeight, 0,
	NULL);

   XtVaSetValues(l->right,
	XmNrightAttachment, XmATTACH_WIDGET,
	XmNrightWidget, l->rhandle,
	XmNtopAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	NULL);

   XtVaSetValues(l->left,
	XmNleftAttachment, XmATTACH_WIDGET,
	XmNleftWidget, l->lhandle,
	XmNtopAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	NULL);

   XtVaSetValues(center_frame,
	XmNtopAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNleftAttachment, XmATTACH_WIDGET,
	XmNleftWidget, l->left,
	XmNrightAttachment, XmATTACH_WIDGET,
	XmNrightWidget, l->right,
	NULL);

   XtVaSetValues(l->center_left,
	XmNleftAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	XmNspacing, 4,
	NULL);

   XtVaSetValues(l->center_right,
	XmNrightAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNspacing, 4,
	NULL);

   XtVaSetValues(l->center_center,
 	XmNleftAttachment, XmATTACH_WIDGET,
 	XmNleftWidget, l->center_left,
 	XmNrightAttachment, XmATTACH_WIDGET,
 	XmNrightWidget, l->center_right,
	XmNspacing, 5,
	NULL);

   create_handles(l);

   /*
    * Not sure where I want to manage the "Handle" widgets, so I will do
    * it here. In the future, options may be available to have the handles
    * not appear, in which case, the Handle's shouldn't be managed at all.
    */
   XtManageChild(l->lhandle);
   XtManageChild(l->rhandle);
}













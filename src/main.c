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

/* $Id: main.c,v 0.6 1997/05/15 05:58:55 mbaron Exp mbaron $ */

#include "xincludes.h"
#include <X11/xpm.h>
#include <signal.h>
#include <stdlib.h>

#include "global.h"
#include "layout.h"
#include "deskswitch.h"
#include "panelrc.h"
#include "subpanel.h"
#include "control.h"
#include "record.h"
#include "table.h"
#include "utils.h"
#include "syspixmaps.h"
#include "resources.h"
#include "MdtString.h"
#include "render.h"
#include "callbacks.h"

Display *display;
Screen *screen;
LayoutRec layout;
PanelRec panel;

SubTable subpanels;
ControlTable controls;
DeskTable desks;

SysPixmaps pixmaps;

XtTranslations no_drag;

char *rc_file_name;

void DoNothing() {}

static char noDragTranslations[] = "#override <Btn2Down>: DoNothing()";
static XtActionsRec noDragActions[] =
	{{"DoNothing", (XtActionProc) DoNothing}};

void Test(Widget foo, XtPointer bar, XtPointer foofoo) {
   printf ("Test Called...");
}

static ICommandRec icmd[] = {
   {"FpCommand", Test},
   {"FpProperties", edit_properties_cb}
};

void init_system_pixmaps() {
   create_xpm_data(layout.left, arrowup_xpm, &pixmaps.up_arrow);
   create_xpm_data(layout.left, arrowblank_xpm, &pixmaps.blank_arrow);
   create_xpm_data(layout.left, arrowdown_xpm, &pixmaps.down_arrow);
   create_xpm_data(layout.left, unknown_xpm, &pixmaps.unknown);
   create_xpm_data(layout.left, spacer_xpm, &pixmaps.spacer);

   /* Decor pixmaps are created in the layout routines... */
}

void main(int argc, char **argv) {
XtAppContext app;
Dimension w,h;
char *rcfile, *home;

   signal(SIGSEGV, sigsegv_cb);

   XtSetLanguageProc(NULL,NULL,NULL);

   layout.toplevel = XtVaAppInitialize(&app, APP_CLASS,
	NULL, 0, &argc, argv, fallbacks, NULL);

   /* Attempt to connect with Fvwm2 */
   wm_init(argc, argv, app);

   display = XtDisplay(layout.toplevel);
   screen = XtScreen(layout.toplevel);

   no_drag = XtParseTranslationTable(noDragTranslations);
   XtAppAddActions(app, noDragActions, XtNumber(noDragActions));

   /*
    * Create the base widgets for the panel.  Also create the handles,
    * "menu decorations, and ribs.
    */
   init_layout(&layout, layout.toplevel);

   init_system_pixmaps();

   /* Misc house keeping stuff */
   init_subpanel_table(&subpanels);
   init_control_table(&controls);
   init_desk_table(&desks);

   /* Create the string that specifies the complete path to the rcfile */
   if ((home = getenv("HOME")) != NULL)
      MdtVaString(&rc_file_name, home, "/.panelrc", NULL);
   else {
      MdtErrorDialog(layout.toplevel, "Couldn't get env variable 'HOME'");
      exit(1);
   }

   /* Open the startup info box */
   raise_intro(layout.toplevel);

   /* Read the panelrc file but don't create anything */
   if (!read_panelrc(rc_file_name)) {
      char *msg;
      MdtVaString(&msg, "Error opening '", rcfile, "'", NULL);
      kill_intro();
      MdtErrorDialog(layout.toplevel, msg);
      MdtStringFree(&msg);
      exit(1);
   }

   /* Now that everything is read in, create all the controls, subpanels, etc */
   render_panel();

   /*
    * This is a cheap hack to get around the fact that we currently don't
    * allow controls to be placed in the deskswitch area.  This will change
    * in the future.
    */
   create_system_controls();

   /*
    * Request that the panel not have any decorations and that
    * it not show up on the screen until we explicitly tell it to
    */
   XtVaSetValues(layout.toplevel,
   	XmNmwmDecorations, 0,
   	XmNmappedWhenManaged, False,
   	NULL);

   render_desk_switch();

   XtRealizeWidget(layout.toplevel);

   /* 
    * At this point the toplevel window has been realized, but not mapped.
    * The rest of this main function *must* be done *after* the 
    * toplevel widget has been realized so the the widths and
    * heights will be known.  Notice that the last thing before the
    * main loop is that the toplevel window is mapped.
    */
   create_ribs(&layout);
   center_the_deskswitch();

   /* Center the panel on the screen */
   XtVaGetValues(layout.toplevel, XmNwidth, &w, XmNheight, &h, NULL);
   XtVaSetValues(layout.toplevel,
	XmNx, (int)((WidthOfScreen(screen)-w)/2),
	XmNy, (int)HeightOfScreen(screen)-h,
	NULL);
 
   kill_intro();

   (icmd[1].cmd)(layout.toplevel, NULL, NULL);

   /* Finally make it appear on the screen */
   XtMapWidget(layout.toplevel);
   XtAppMainLoop(app);
}

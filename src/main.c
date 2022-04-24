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


Atom a_NET_DESKTOP_GEOMETRY;
Atom a_NET_DESKTOP_VIEWPORT;

char *rc_file_name;

void DoNothing() {}

static char noDragTranslations[] = "#override <Btn2Down>: DoNothing()";
static XtActionsRec noDragActions[] =
	{{"DoNothing", (XtActionProc) DoNothing}};

void Test(Widget foo, XtPointer bar, XtPointer foofoo) {
   printf ("Test Called...");
}

//static ICommandRec icmd[] = {
//   {"FpCommand", Test},
//   {"FpProperties", edit_properties_cb}
//};

void init_system_pixmaps() {
   create_xpm_data(layout.left, arrowup_xpm, &pixmaps.up_arrow);
   create_xpm_data(layout.left, arrowblank_xpm, &pixmaps.blank_arrow);
   create_xpm_data(layout.left, arrowdown_xpm, &pixmaps.down_arrow);
   create_xpm_data(layout.left, unknown_xpm, &pixmaps.unknown);
   create_xpm_data(layout.left, spacer_xpm, &pixmaps.spacer);

   /* Decor pixmaps are created in the layout routines... */
}


//void MyLoop(XtAppContext app);
void ReadConfWorkProc(XtPointer client_data);

XtAppContext app;

XtActionsRec actions;

String translations = "<Btn1Motion>: mov_cbk(motion) ManagerGadgetButtonMotion() \n\
<Btn1Down>: mov_cbk(down) ManagerGadgetArm() \n\
<Btn1Up>: mov_cbk(up)";

void create_atoms()
{

    a_NET_DESKTOP_GEOMETRY = XInternAtom(display, "_NET_DESKTOP_GEOMETRY", False);
	if (a_NET_DESKTOP_GEOMETRY == None) printf("atom error\n");

    a_NET_DESKTOP_VIEWPORT = XInternAtom(display, "_NET_DESKTOP_VIEWPORT", False);
	if (a_NET_DESKTOP_VIEWPORT == None) printf("atom error\n");


}



void main(int argc, char **argv) {
//XtAppContext app;
Dimension w,h;
char *rcfile, *home;
XtWorkProcId id;
XtIntervalId iid;
Window win;
char tmpstr[30];
int n;
//XtActionsRec actions;

//String translations = "<Btn1Motion>: mov_cbk(motion) ManagerGadgetButtonMotion() \n\
//<Btn1Down>: mov_cbk(down) ManagerGadgetArm() \n\
//<Btn1Up>: mov_cbk(up)";

/*String translations = "<Btn1Motion>: mov_cbk(motion)\n\
<Btn1Down>: mov_cbk(down)\n\
<Btn1Up>: mov_cbk(up)";
*/

   signal(SIGSEGV, sigsegv_cb);

   XtSetLanguageProc(NULL,NULL,NULL);

   layout.toplevel = XtVaAppInitialize(&app, APP_CLASS,
	NULL, 0, &argc, argv, fallbacks, XmNoverrideRedirect, True, NULL);

   actions.string = "mov_cbk";
   actions.proc = mov_cbk;
   XtAppAddActions(app, &actions, 1);


   desks.x=0;
   desks.y=0;

   display = XtDisplay(layout.toplevel);
   screen = XtScreen(layout.toplevel);

   create_atoms();

   win = DefaultRootWindow(display);

   get_desktop_size(win, display);

#ifdef DEBUG
   n=ScreenCount(display);
   printf("Screen number %d\n", n);
#endif

   no_drag = XtParseTranslationTable(noDragTranslations);
   XtAppAddActions(app, noDragActions, XtNumber(noDragActions));

//    a_NET_CURRENT_DESKTOP = XInternAtom(display, "_NET_CURRENT_DESKTOP", False);
//	if (a_NET_CURRENT_DESKTOP == None) printf("atom error\n");

    XSelectInput(display, win, PropertyChangeMask);
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
#ifdef DEBUG
   sprintf(tmpstr, "X=%d Y=%d", desks.x1, desks.y1);
   MdtErrorDialog(layout.toplevel, tmpstr);
#endif

   raise_intro(layout.toplevel);

   iid=XtAppAddTimeOut(app, 500, ReadConfWorkProc, (XtPointer)rcfile);

//   id=XtAppAddWorkProc(app, ReadConfWorkProc, (XtPointer)rcfile);

//   MyLoop(app);
   /* Read the panelrc file but don't create anything */
//   if (!read_panelrc(rc_file_name)) {
//      char *msg;
//      MdtVaString(&msg, "Error opening '", rcfile, "'", NULL);
//      kill_intro();
//      MdtErrorDialog(layout.toplevel, msg);
//      MdtStringFree(&msg);
//      exit(1);
//   }

   /* Now that everything is read in, create all the controls, subpanels, etc */
//   render_panel();

   /*
    * This is a cheap hack to get around the fact that we currently don't
    * allow controls to be placed in the deskswitch area.  This will change
    * in the future.
    */
//   create_system_controls();

   /*
    * Request that the panel not have any decorations and that
    * it not show up on the screen until we explicitly tell it to
    */
//   XtVaSetValues(layout.toplevel,
//   	XmNmwmDecorations, 0,
//   	XmNmappedWhenManaged, False,
//   	NULL);

//   render_desk_switch();

//   XtRealizeWidget(layout.toplevel);

   /* 
    * At this point the toplevel window has been realized, but not mapped.
    * The rest of this main function *must* be done *after* the 
    * toplevel widget has been realized so the the widths and
    * heights will be known.  Notice that the last thing before the
    * main loop is that the toplevel window is mapped.
    */
//   create_ribs(&layout);
//   center_the_deskswitch();

//XtAugmentTranslations(layout.myleftrib, XtParseTranslationTable(translations));
//XtAugmentTranslations(layout.myrightrib, XtParseTranslationTable(translations));

/*   XtVaSetValues(layout.myrib, 	XmNtranslations, XtParseTranslationTable(translations), NULL);*/

   /* Center the panel on the screen */
//   XtVaGetValues(layout.toplevel, XmNwidth, &w, XmNheight, &h, NULL);
//   XtVaSetValues(layout.toplevel,
//	XmNx, (int)((WidthOfScreen(screen)-w)/2),
//	XmNy, (int)HeightOfScreen(screen)-h,
//	NULL);
 
//   kill_intro();

   //   (icmd[1].cmd)(layout.toplevel, NULL, NULL);

   /* Finally make it appear on the screen */
//   XtMapWidget(layout.toplevel);
//   MyLoop(app);
   XtAppMainLoop(app);
}



void RenderWorkProc(XtPointer client_data);
void FinalWorkProc(XtPointer client_data);

void ReadConfWorkProc(XtPointer client_data)
{
   XtWorkProcId id;
   XtIntervalId iid;
   char *rcfile = (char *) client_data;
   /* Read the panelrc file but don't create anything */
//   MdtErrorDialog(layout.toplevel, "ReadConfWorkProc started");
   if (!read_panelrc(rc_file_name)) {
      char *msg;
      MdtVaString(&msg, "Error opening '", rcfile, "'", NULL);
      kill_intro();
      MdtErrorDialog(layout.toplevel, msg);
      MdtStringFree(&msg);
      exit(1);
   }

#ifdef DEBUG
   MdtErrorDialog(layout.toplevel, "ReadConfWorkProc executing...");
#endif

   iid=XtAppAddTimeOut(app, 1000, RenderWorkProc, NULL);
//   id=XtAppAddWorkProc(app, RenderWorkProc, NULL);
}

void RenderWorkProc(XtPointer client_data)
{
   Dimension w,h;
   XtWorkProcId id;
   XtIntervalId iid;
   
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

   XtAugmentTranslations(layout.myleftrib, XtParseTranslationTable(translations));
   XtAugmentTranslations(layout.myrightrib, XtParseTranslationTable(translations));

/*   XtVaSetValues(layout.myrib, 	XmNtranslations, XtParseTranslationTable(translations), NULL);*/

   /* Center the panel on the screen */
   XtVaGetValues(layout.toplevel, XmNwidth, &w, XmNheight, &h, NULL);
   XtVaSetValues(layout.toplevel,
	XmNx, (int)((WidthOfScreen(screen)-w)/2),
	XmNy, (int)HeightOfScreen(screen)-h,
	NULL);

//   MdtErrorDialog(layout.toplevel, "RenderWorkProc executing...");

   iid=XtAppAddTimeOut(app, 1000, FinalWorkProc, NULL);
//   id=XtAppAddWorkProc(app, FinalWorkProc, NULL);

}

//static Boolean work_proc(XtPointer client_data)
//{
//   GetWmCommand();
//   return True;
//   XtAppAddWorkProc(app, work_proc, NULL);
//}

//XtEventDispatchProc * PrevDispatcher;

Boolean (* orig_dispatcher)(XEvent* event);

Boolean property_dispatcher(XEvent* event)
{
    Atom at;
    Window win, rwin;
    int index;

    if (event->type == PropertyNotify )
	{
	at = event->xproperty.atom;
	win = event->xproperty.window;

	if (win == DefaultRootWindow(display))
		{
		if (at == a_NET_DESKTOP_VIEWPORT) 
			{
			index = get_current_viewport(win, display);
#ifdef DEBUG
			printf("current viewport %d\n", index);
#endif
			new_deskswitch_select(desks.list[index]);
			}
		}
	if (orig_dispatcher) (* orig_dispatcher) (event);
	}


}

void FinalWorkProc(XtPointer client_data)
{
   Dimension w,h;
   Window win;
   int index;


   kill_intro();


   /* Finally make it appear on the screen */
   XtMapWidget(layout.toplevel);
   win = DefaultRootWindow(display);
   index = get_current_viewport(win, display);
   new_deskswitch_select(desks.list[index]);

   orig_dispatcher = XtSetEventDispatcher(display, PropertyNotify, property_dispatcher);


//   XtAppAddWorkProc(app, work_proc, NULL);

}

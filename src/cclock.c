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

/* $Id: cclock.c,v 1.2 1997/06/10 19:52:26 mbaron Exp mbaron $ */

#include "xincludes.h"
#include "cclock.h"
#include <time.h>
#include <math.h>

#define HOUR_HAND_LENGTH 8
#define MINUTE_HAND_LENGTH 12

static char *months[12] = 
	{"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
	 "Aug", "Sep", "Oct", "Nov", "Dec"};

void clock_set_timer(ClockRec*);
void clock_digital_update_time(ClockRec);
void clock_digital_update_date(ClockRec);

void clock_analog_update(ClockRec*);
void spin_wait(ClockRec*, int);
void spin_cb(ClockRec*, XtIntervalId*);

void clock_check_cb(ClockRec*, XtIntervalId*);
void clock_set_timer(ClockRec*);
void clock_get_time(TimeRec*);

/*
 * Stuff for the Analog Clock 
 */

Widget clock_create_analog(Widget parent, Pixmap p) {
ClockRec *c = (ClockRec*)malloc(sizeof(ClockRec));

Display *dpy = XtDisplay(parent);
Window win;
unsigned int dont_care;
XGCValues gcv;
unsigned long gcm;

   c->type = Analog;
   c->parent = parent;   

   gcm = GCGraphicsExposures;
   gcv.graphics_exposures = False;

   c->view.aview.gc = XCreateGC(dpy,
	RootWindowOfScreen(XtScreen(parent)), gcm, &gcv);

   XSetForeground(dpy, c->view.aview.gc,
	WhitePixelOfScreen(XtScreen(parent)));
   XSetLineAttributes(dpy, c->view.aview.gc, 1, LineSolid, CapProjecting,
	JoinRound);

   XGetGeometry(dpy,
	p, &win, &dont_care, &dont_care,
	&c->view.aview.w, &c->view.aview.h,
	&dont_care, &c->view.aview.d);

   c->view.aview.cx = c->view.aview.w / 2;
   c->view.aview.cy = c->view.aview.h / 2;

   c->view.aview.original = XCreatePixmap(dpy,
	RootWindowOfScreen(XtScreen(parent)),
	c->view.aview.w, c->view.aview.h, c->view.aview.d);

   XCopyArea(dpy, p, c->view.aview.original, c->view.aview.gc, 0, 0,
	c->view.aview.w, c->view.aview.h, 0, 0);

   c->view.aview.visible = p;

   c->view.aview.label = XtVaCreateManagedWidget("",
	xmLabelWidgetClass, parent,
	XmNlabelType, XmPIXMAP,
	XmNlabelPixmap, p,
	NULL);

   clock_get_time(&c->time);
   clock_analog_update(c);
   clock_set_timer(c);

   return c->view.aview.label;
}

void clock_analog_update(ClockRec *c) {
static double PIE = 3.14159;
static double PIE30 = 3.14159 / 30;
static double PIE6  = 3.14159 / 6;
static double PIE2  = 3.14159 / 2;

AnalogView *v = &c->view.aview;
Display *dpy = XtDisplay(v->label);
Window win = XtWindow(v->label);
float THETA, A, O;
XPoint pts[3];
Pixmap old_pix, new_pix;

   /* Find the coords for the hour hand */
   THETA = PIE2 - c->time.hour * PIE6;
   A = cos(THETA) * HOUR_HAND_LENGTH;
   O = sin(THETA) * HOUR_HAND_LENGTH;
   pts[0].x = v->cx + A;
   pts[0].y = v->cy - O;

   /* Find the coords for the minute hand */
   THETA = PIE2 - c->time.minute * PIE30;
   pts[2].x = v->cx + cos(THETA) * MINUTE_HAND_LENGTH;
   pts[2].y = v->cy - sin(THETA) * MINUTE_HAND_LENGTH;

   pts[1].x = v->cx; pts[1].y = v->cy;

   XtVaGetValues(v->label, XmNlabelPixmap, &old_pix, NULL);
   new_pix = XCreatePixmap(dpy,
	RootWindowOfScreen(XtScreen(v->label)), v->w, v->h, v->d);

   /* Copy the clock pixmap to offscreen buffer */
   XCopyArea(dpy, v->original, new_pix, v->gc, 0, 0, v->w, v->h, 0, 0);

   /* Draw The Hour hands */
   XDrawLines(dpy, new_pix, v->gc, pts, 3, CoordModeOrigin);

   XtVaSetValues(v->label, XmNlabelPixmap, new_pix, NULL);

   XFreePixmap(dpy, old_pix);

   XFlush(dpy);
}

/*
 * Stuff for the Digital Clock 
 */
void clock_create_digital(Widget parent_form, Widget top_widget) {
ClockRec *clock = (ClockRec*)malloc(sizeof(ClockRec));
Widget rowcol;
   
   if (clock == NULL) return;

   clock->parent = parent_form;
   clock->type = Digital;

   rowcol = XtVaCreateManagedWidget("Clock",
	xmRowColumnWidgetClass, parent_form,
	XmNtopAttachment, XmATTACH_WIDGET,
	XmNtopWidget, top_widget,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNorientation, XmVERTICAL,
	XmNpacking, XmPACK_COLUMN,
	XmNnumColumns, 1,
	XmNadjustLast, True,
	XmNentryAlignment, XmALIGNMENT_CENTER,
	NULL);

    clock->view.dview.date = XtVaCreateManagedWidget("Date",
	xmLabelWidgetClass, rowcol,
	NULL);

   clock->view.dview.time = XtVaCreateManagedWidget("Time",
	xmLabelWidgetClass, rowcol,
	NULL);

   clock_get_time(&clock->time);

   clock_digital_update_time(*clock);
   clock_digital_update_date(*clock);

   clock_set_timer(clock);
}

void clock_digital_update_time(ClockRec c) {
char time_str[9];
XmString str;

   sprintf(time_str,"%d:%.2d", c.time.hour, c.time.minute);
   str = XmStringCreateSimple(time_str);
   XtVaSetValues(c.view.dview.time, XmNlabelString, str, NULL);
   XmStringFree(str);
}

void clock_digital_update_date(ClockRec c) {
char date_str[7];
XmString str;

   sprintf(date_str,"%s %d", months[c.time.month], c.time.day);
   str = XmStringCreateSimple(date_str);
   XtVaSetValues(c.view.dview.date, XmNlabelString, str, NULL);
   XmStringFree(str);
}

/* 
 * Functions common to digital and analog clocks
 */
void clock_get_time(TimeRec *t) {
time_t the_time;
struct tm *ts;

   the_time = time(&the_time);
   ts = localtime(&the_time);

   t->hour = ts->tm_hour;
   t->minute = ts->tm_min;
   t->month = ts->tm_mon;
   t->day = ts->tm_wday;
}

void clock_set_timer(ClockRec *c) {
   XtAppAddTimeOut(XtWidgetToApplicationContext(c->parent),
        10000, (XtTimerCallbackProc)clock_check_cb, c);
}

void clock_check_cb(ClockRec *c, XtIntervalId *id) {
TimeRec tmp_clock;

   clock_get_time(&tmp_clock);

   if (tmp_clock.minute != c->time.minute) {
      c->time.hour = tmp_clock.hour;
      c->time.minute = tmp_clock.minute;
      if (c->type == Analog) clock_analog_update(c);

      if (tmp_clock.day != c->time.day && c->type == Digital) {
         c->time.day = tmp_clock.day; c->time.month = tmp_clock.month;
      }
   }

   clock_set_timer(c);
}

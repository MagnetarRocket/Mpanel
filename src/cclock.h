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

/* $Id: cclock.h,v 1.1 1997/06/10 19:50:23 mbaron Exp mbaron $ */

#include <X11/Intrinsic.h>
#include "record.h"

typedef enum _ClockType {Analog, Digital} ClockType;

typedef struct _AnalogView {
   Widget label;
   Pixmap original, buff, visible;
   GC gc;
   Display *dpy;
   unsigned int cx, cy, w, h, d;
} AnalogView;

typedef struct _DigitalView {
   Widget date, time;
} DigitalView;

typedef struct _TimeRec {
   int hour, minute, day, month;
} TimeRec;

typedef struct _ClockRec {
   ClockType type;
   Widget parent;
   int freq;
   
   TimeRec time;

   union {
      AnalogView aview;
      DigitalView dview;
   } view;

} ClockRec;

void clock_create_digital(Widget, Widget);
Widget clock_create_analog(Widget, Pixmap);

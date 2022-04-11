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

/* $Id: resources.h,v 0.5 1997/05/15 05:26:39 mbaron Exp mbaron $ */

#ifndef RESOURCES_H
#define RESOURCES_H

#include <X11/Intrinsic.h>

static String fallbacks[] = {
	"*background: #aeb2c3",
	"*foreground: #ffffff",

	"*XmTextField.background: #ffffff",
	"*XmTextField.foreground: #000000",

	"*XmList.background: #ffffff",
	"*XmList.foreground: #000000",

	"*XmRowColumn.selectColor: #A52A2A",

	"*highlightColor: #000000",

	"*Lip.background: #aeb2c3",

	"*Clock*foreground: #000000",

	"*fontList: -adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-*-*",
	"*DeskSwitchButton*fontList: -adobe-helvetica-medium-r-*-*-12-*-*",
	"*Subpanel*fontList: -adobe-helvetica-medium-r-*-*-14-*-*",
	"*Clock.Time.fontList: -adobe-helvetica-medium-r-*-*-14-*-*",
	"*Clock.Date.fontList: -adobe-helvetica-medium-r-*-*-14-*-*",


	"*Lip.marginWidth: 2",
	"*Lip.marginHeight: 2",

	"*InstallIcon.Outer.topOffset: 10",
	"*InstallIcon.Outer.bottomOffset: 10",
	"*InstallIcon.Outer.marginWidth: 2",
	"*InstallIcon.Outer.marginHeight: 2",

	"*DeskSwitchButton*background: #718aa6",
	"*DeskSwitchButton*foreground: #ffffff",
	"*DeskSwitchButton.width: 125",
	"*DeskSwitchButton.height: 25",

	"*Intro*background: #aeb2c3",
	"*Intro*Title.fontList: -*-times-bold-r-*--24-240-*",
	"*Intro*Title.foreground: blue4",
	"*Intro*Version.fontList: -adobe-helvetica-medium-r-*-*-14-*-*",
	"*Intro*Version.foreground: red",
	"*Intro*Author.fontList: -adobe-helvetica-medium-r-*-*-14-*-*",
	"*Intro*Author.foreground: blue4",
	"*Intro*Message.fontList: -adobe-helvetica-medium-r-*-*-14-*-*",
	"*Intro*Message.foreground: #000000",
	NULL

};

#endif

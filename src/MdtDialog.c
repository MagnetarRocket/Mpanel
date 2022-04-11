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

#include "MdtDialog.h"
#include "xincludes.h"

#include <stdio.h>

enum state {Up, Down};
typedef enum state State;

typedef struct _DialogRec {
   int pressed;
   State s;
   Widget dialog;
} DialogRec;

#define NoPress -1

void press_cb(Widget, XtPointer, XtPointer);
void fs_press_cb(Widget, XtPointer, XtPointer);
void wait_for_press(DialogRec*);

void MdtDefaultSegvHandler(int sig) {
   fprintf(stderr, "Caught SIGSEGV (signal %d). Exit(11)...\n", sig);
   exit(11);
}

int MdtErrorDialog(Widget parent, char *error_message) {
XmString message;
DialogRec d;

   message = XmStringCreateLocalized(error_message);
   d.dialog = XmCreateErrorDialog(parent,"Error",NULL,0);

   XtVaSetValues(d.dialog,
        XmNmessageString, message,
        NULL);

   XtVaSetValues(XtParent(d.dialog),
        XmNtitle, "Error",
        NULL);

   XmStringFree(message);

   XtUnmanageChild(XmMessageBoxGetChild(d.dialog,XmDIALOG_CANCEL_BUTTON));
   XtUnmanageChild(XmMessageBoxGetChild(d.dialog,XmDIALOG_HELP_BUTTON));
   XtAddCallback(d.dialog,XmNokCallback, press_cb, &d);

   XtManageChild(d.dialog);

   d.pressed = NoPress;

   wait_for_press(&d);
   XtDestroyWidget(d.dialog);
   return 1;
}

int MdtYesNoDialog(Widget parent, char *question) {
XmString message, yes, no, cancel;
DialogRec d;

   message = XmStringCreateLocalized(question);
   yes = XmStringCreateLocalized("Yes");
   no = XmStringCreateLocalized("No");
   cancel = XmStringCreateLocalized("Cancel");
         
   d.dialog = XmCreateQuestionDialog(parent,"Question",NULL,0);

   XtVaSetValues(d.dialog,
        XmNmessageString, message,
        XmNokLabelString, yes,
        XmNcancelLabelString, no,
	XmNhelpLabelString, cancel,
        NULL);

   XtVaSetValues(XtParent(d.dialog),
        XmNtitle, "Question",
        NULL);

   XmStringFree(message);

   XtAddCallback(d.dialog,XmNokCallback, press_cb, &d);
   XtAddCallback(d.dialog,XmNcancelCallback, press_cb, &d);
   XtAddCallback(d.dialog,XmNhelpCallback, press_cb, &d);

   XtManageChild(d.dialog);

   d.pressed = NoPress;

   wait_for_press(&d);
   XtDestroyWidget(d.dialog);

   switch(d.pressed) {
      case XmCR_OK: return Ok;
      case XmCR_CANCEL: return No;
      case XmCR_HELP: return Cancel;
      default: return d.pressed;
   }
}

int MdtInfoDialog(Widget parent, char *info_message) {
XmString message;
DialogRec d;

   message = XmStringCreateLocalized(info_message);
   d.dialog = XmCreateInformationDialog(parent,"Information",NULL,0);

   XtVaSetValues(d.dialog,
        XmNmessageString, message,
        NULL);

   XtVaSetValues(XtParent(d.dialog),
        XmNtitle, "Information",
        NULL);

   XmStringFree(message);

   XtUnmanageChild(XmMessageBoxGetChild(d.dialog,XmDIALOG_CANCEL_BUTTON));
   XtUnmanageChild(XmMessageBoxGetChild(d.dialog,XmDIALOG_HELP_BUTTON));

   XtAddCallback(d.dialog,XmNokCallback, press_cb, &d);
   XtManageChild(d.dialog);

   d.pressed = NoPress;
   wait_for_press(&d);
   XtDestroyWidget(d.dialog);
   return 1;
}

char* SelectFile(Widget parent, char *title, char *init_dir, char *file_mask) {
DialogRec d;
XmString dir_str, filemask_str, str;
char *filename;

   d.pressed = NoPress;
   d.dialog = XmCreateFileSelectionDialog(parent, "FileSelector", NULL, 0);

   XtVaSetValues(XtParent(d.dialog),
	XmNtitle, title,
	NULL);

   dir_str = XmStringCreateSimple(init_dir);
   filemask_str = XmStringCreateSimple(file_mask);
   XtVaSetValues(d.dialog,
	XmNdirectory, dir_str,
	XmNpattern, filemask_str,
	NULL);
   XmStringFree(dir_str);
   XmStringFree(filemask_str);

   XtUnmanageChild(XmFileSelectionBoxGetChild(d.dialog,XmDIALOG_HELP_BUTTON));
   XtAddCallback(d.dialog, XmNcancelCallback, fs_press_cb, &d);
   XtAddCallback(d.dialog, XmNokCallback, fs_press_cb, &d);

   XtManageChild(d.dialog);

   wait_for_press(&d);

   XtVaGetValues(d.dialog, XmNtextString, &str, NULL);
   XmStringGetLtoR(str, XmFONTLIST_DEFAULT_TAG, &filename);

   XtDestroyWidget(d.dialog);

   if (d.pressed == XmCR_CANCEL)
      return NULL;

   return filename;
}

void wait_for_press(DialogRec *d) {
  while (d->pressed == NoPress)
         XtAppProcessEvent(XtWidgetToApplicationContext(d->dialog), XtIMAll);
}

void fs_press_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
DialogRec *d = (DialogRec*)client_data;
XmFileSelectionBoxCallbackStruct *cbs =
	(XmFileSelectionBoxCallbackStruct*)call_data;
char *filename;

   XmStringGetLtoR(cbs->value,XmFONTLIST_DEFAULT_TAG, &filename);
   d->pressed = cbs->reason;
}

void press_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
DialogRec *d = (DialogRec*)client_data;
XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*)call_data;

   d->pressed = cbs->reason;
}

void MdtSorry(Widget w, XtPointer client_data, XtPointer call_data) {
Widget parent = (Widget)client_data;

   MdtInfoDialog(parent, "This function is not implemented yet");
}

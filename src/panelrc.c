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

/* $Id: panelrc.c,v 0.6 1997/05/21 19:07:14 mbaron Exp mbaron $ */

#include "panelrc.h"
#include "record.h"
#include "control.h"
#include "deskswitch.h"
#include "global.h"
#include "table.h"
#include "uscanner.h"

#include <stdio.h>

enum panelrctokens {DontCare, ControlSym, DeskSwitchSym, IconSym, 
	PushActionSym, DropActionSym, LabelSym, SubpanelSym, 
	PanelSym, ControlTypeSym, ContainerNameSym, ContainerTypeSym, BoxSym,
	NumRowsSym, LockSym, BiffSym, AltIconSym, SubpanelOffsetSym,
	TrueSym, FalseSym, SubpanelsTypeSym, ClockSym, BlankSym,
	PixmapPathSym};

typedef enum panelrctokens Token;

static TokenDefRec tokens[] = {
	{ControlSym, "CONTROL"},
	{ContainerNameSym, "CONTAINER_NAME"},
	{ContainerTypeSym, "CONTAINER_TYPE"},
	{ControlTypeSym, "TYPE"},
	{IconSym, "ICON"},
	{PushActionSym, "PUSH_ACTION"},
	{DropActionSym, "DROP_ACTION"},
	{LabelSym, "LABEL"},
	{SubpanelSym, "SUBPANEL"},
	{BoxSym, "BOX"},
	{TrueSym, "True"},
	{FalseSym, "False"},
	{BlankSym, "blank"},
	{PanelSym, "PANEL"},
	{NumRowsSym, "NUMBER_OF_ROWS"},
	{ClockSym, "clock"},
	{LockSym, "LOCK"},
	{SubpanelsTypeSym, "ANIMATED_SUBPANELS"},
	{DeskSwitchSym, "SWITCH"},
	{SubpanelOffsetSym, "SUBPANEL_OFFSET"},
	{AltIconSym, "ALTERNATE_ICON"},
	{PixmapPathSym, "PIXMAP_PATH"},
	{BiffSym, "biff"}
};

ScanRec *s = NULL;

void deskswitch() {
int done=0;

   Match(s, DeskSwitchSym);
   Match(s, LBrace);

   while(!done) {
      if(NextToken(s) == Id) {
         DeskRec *d = new_desk();
         Match(s, Id);
         CaptureToken(s, &d->label);
         register_desk(&desks,d);

         if (NextToken(s) == Comma)
            Match(s, Comma);
         else
            done = 1;
      } 
      else
         done = 1;
   }

   Match(s, RBrace);
}

void parse_control_type(ControlRec *c) {
   Match(s, ControlTypeSym);

   switch(NextToken(s)) {
      case BiffSym: c->control_type = Biff; break;
      case IconSym: c->control_type = Icon; break;
      case ClockSym: c->control_type = Clock; break;
      case BlankSym: c->control_type = Blank; break;
   }

   Match(s, Anything);
}

ControlRec* control() {
ControlRec *c;
int done=0;

   c = new_control();

   Match(s, ControlSym);
   Match(s, Id);
   CaptureToken(s, &c->id);
   Match(s, LBrace);

   while(!done) {
   switch(NextToken(s)) {
      case RBrace: done=1; break;

      case ControlTypeSym: parse_control_type(c); break;

      case ContainerNameSym:
         Match(s, ContainerNameSym);
         Match(s, Id);
         CaptureToken(s, &c->container_name);
      break;

      case ContainerTypeSym:
         Match(s, ContainerTypeSym);
         switch(NextToken(s)) {
            case BoxSym: 
               Match(s, BoxSym);
               c->container_type = Box;
            break;
            
            case SubpanelSym:
               Match(s, SubpanelSym);
               c->container_type = Subpanel;
            break;

            default:
               fprintf(stderr, "Bad container type '%s'\n", s->token_buffer);
               Match(s, Anything);
         }
      break;

      case LabelSym:
         Match(s, LabelSym);
         ScanId(s);
         CaptureToken(s, &c->label);
      break;
      
      case IconSym:
         Match(s, IconSym);
         Match(s, Id);
         CaptureToken(s, &c->icon);
      break;

      case AltIconSym:
         Match(s, AltIconSym);
         Match(s, Id);
         CaptureToken(s, &c->alt_icon);
      break;

      case PushActionSym:
         Match(s, PushActionSym);
         ScanId(s);
         CaptureToken(s, &c->click_action);
      break;

      default:
         fprintf(stderr, "Error '%s'\n", s->token_buffer);
         Match(s, Anything);
   }
   }
   Match(s, RBrace);

   return c;
}

SubpanelRec* subpanel() {
short done=0;
SubpanelRec *sub = new_subpanel();

   Match(s, SubpanelSym);
   Match(s, Id);
   CaptureToken(s, &sub->id);
   Match(s, LBrace);

   while(!done) {   
      switch(NextToken(s)) {
         case LabelSym:
            Match(s, LabelSym);
            ScanId(s);
            CaptureToken(s, &sub->label); 
         break;
         case ContainerNameSym:
            Match(s, ContainerNameSym);
            Match(s, Id);
            CaptureToken(s, &sub->container_name);
         break;

         case RBrace: done=1; break;

         default: Match(s, Anything);
      }
   }
   Match(s, RBrace);

   create_subpanel(sub);

   return sub;
}

void panel_attributes() {
short done=0;

   while(!done)
      switch(NextToken(s)) {
         case PixmapPathSym:
            Match(s, PixmapPathSym);
            Match(s, Id);
            CaptureToken(s, &panel.pixmap_path);
         break;

         case LockSym: 
            Match(s, LockSym); 
            ScanId(s);
            CaptureToken(s, &panel.lock); 
         break;

         case SubpanelsTypeSym:
            Match(s, SubpanelsTypeSym);
            panel.subpanel_model=(NextToken(s) == FalseSym ? PopUp : SlideUp);
            Match(s, Anything);            
         break;

         case SubpanelOffsetSym:
            Match(s, SubpanelOffsetSym);
            Match(s, Anything);
            panel.subpanel_x_offset = atoi(s->token_buffer);
            Match(s, Comma);
            Match(s, Anything);
            panel.subpanel_y_offset = atoi(s->token_buffer);
         break;

         case RBrace: done=1; break;
         default: Match(s, Anything);
      }

   Match(s, RBrace);
}

Token statement() {
ControlRec *c;
SubpanelRec *sub;

   switch (NextToken(s)) {
      case EofSym: return EofSym;

      case PanelSym: panel_attributes(); break;

      case ControlSym: 
         c = control();
         register_control(&controls, c);
      break;

      case DeskSwitchSym: deskswitch(); break;

      case SubpanelSym:
         sub = subpanel();
         register_subpanel(&subpanels, sub);
      break;
   }

   return Anything;
}

int read_panelrc(char *file) {

   if ((s = NewScanner(file, tokens, XtNumber(tokens))) == NULL)
      return 0;

   panel.subpanel_x_offset = panel.subpanel_y_offset = 0;
   panel.lock = panel.pixmap_path = NULL;
   panel.subpanel_model = PopUp;

   while(statement() != EofSym);

   ScannerDone(s);

   return 1;
}

void write_panelrc_line(FILE *fp, char *attr, char *value) {
     if (value != NULL)
    fprintf(fp, "\t%s %s\n", attr, value);
}

int write_panelrc(char *filename) {
FILE *fp = stdout;
SubpanelRec *s;
ControlRec *c;
char *tmp_str;
int i;

   if ((fp = fopen(filename, "w")) == NULL)
      return 0;

   /* First we write the panel attributes */
   fprintf(fp, "PANEL {\n");
   
   write_panelrc_line(fp, "LOCK", panel.lock);

   fprintf(fp, "\tSUBPANEL_OFFSET\t\t\t%d, %d\n", panel.subpanel_x_offset,
	panel.subpanel_y_offset);
   
   write_panelrc_line(fp, "PIXMAP_PATH", panel.pixmap_path);
   write_panelrc_line(fp, "ANIMATED_SUBPANELS",
	(panel.subpanel_model == SlideUp ? "True" : "False"));

   fprintf(fp, "}\n\n");

   /* Write the deskswitch */
   fprintf(fp, "SWITCH {");
   for (i = 0; i < desks.num; i++)
      fprintf(fp, "%s%c", desks.list[i]->label, (i == desks.num - 1 ? ' ' : ','));
   fprintf(fp, "}\n\n");

   /* Now we write all of the subpanels */
   s = subpanels.head;
   while(s != NULL) {
      fprintf(fp, "SUBPANEL %s {\n", s->id);
      write_panelrc_line(fp, "LABEL", s->label);
      write_panelrc_line(fp, "CONTAINER_NAME", s->container_name);
      s = s->link;
      fprintf(fp, "}\n\n");
   }

   /* Now we write all of the controls */
   c = controls.head;
   while(c != NULL) {
      fprintf(fp, "CONTROL %s {\n", c->id);

      switch(c->control_type) {
         case Icon: tmp_str = "icon"; break;
         case Blank: tmp_str = "blank"; break;
         case Clock: tmp_str = "clock"; break;
         case Biff: tmp_str = "biff"; break;
      }

      write_panelrc_line(fp, "TYPE", tmp_str);
      write_panelrc_line(fp, "LABEL", c->label);
      write_panelrc_line(fp, "CONTAINER_NAME", c->container_name);
      write_panelrc_line(fp, "CONTAINER_TYPE",
	(c->container_type == Box ? "BOX" : "SUBPANEL"));
      write_panelrc_line(fp, "ICON", c->icon);
      write_panelrc_line(fp, "PUSH_ACTION", c->click_action);

      fprintf(fp, "}\n\n");
      c = c->link;
   }

   fclose(fp);
   return 1;
}

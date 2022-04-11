/*
 * Panel - CDE Front Panel Emulator
 *
 * Copyright (C) 1997 Matthew Baron
 *         
 * E-mail: mbaron@d.umn.edu
 * URL   : http://www.d.umn.edu/~mbaron/
 *
 * Portions Copyright (C) 1993 Robert Nation
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

/* $Id: wm.c,v 0.6 1997/05/15 05:31:21 mbaron Exp mbaron $ */

#include "wm.h"
#include "module.h"
#include "global.h"
#include <stdlib.h>
#include <signal.h>
#include "xincludes.h"

unsigned long window = 0L;
int fd[2];

void DeadPipe(int foo) {
   fprintf(stderr, "Quitting because Fvwm is quitting...\n");
   exit(0);
}

int wm_init(int argc, char **argv, XtAppContext app) {
char *str=NULL;

   if((argc != 6)&&(argc != 7))
      return 0;

   fd[0] = atoi(argv[1]);
   fd[1] = atoi(argv[2]);

   signal(SIGPIPE, DeadPipe);

   MdtVaString(&str, "Style ", """", APP_NAME, """", " Sticky, WindowListSkip",
	NULL);

   SendFvwmCmd(str);   

   MdtStringFree(&str);

   return 1;
}

void SendFvwmCmd(char *message) {
  int w;
  
  if(message != NULL)
    {
      write(fd[0], &window, sizeof(unsigned long));
      
      w=strlen(message);
      write(fd[0],&w,sizeof(int));
      write(fd[0],message,w);
      
      /* keep going */
      w = 1;
      write(fd[0],&w,sizeof(int));
    }
}

int ReadFvwmPacket(int fd, unsigned long *header, unsigned long **body)
{
  int count,total,count2,body_length;
  char *cbody;

  if((count = read(fd,header,HEADER_SIZE*sizeof(unsigned long))) >0)
    {
      if(header[0] == START_FLAG)
        {
          body_length = header[2]-HEADER_SIZE;
          *body = (unsigned long *)
            malloc(body_length * sizeof(unsigned long));
          cbody = (char *)(*body);
          total = 0;
          while(total < body_length*sizeof(unsigned long))
            {
              if((count2=
                  read(fd,&cbody[total],
                       body_length*sizeof(unsigned long)-total)) >0)
                {
                  total += count2;
                }
              else if(count2 < 0)
                {
                  DeadPipe(1);
                }
            }
        }
      else
        count = 0;
    }
  if(count <= 0)
    DeadPipe(1);
  return count;
}


void GotoPage(int x, int y) {
char str[10];

   sprintf(str, "Desk 0 %d", x);
   SendFvwmCmd(str);
}

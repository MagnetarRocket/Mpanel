/* -*-c-*- */
/* This program is free software; you can redistribute it and/or modify
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
** Module.c: code for modules to communicate with fvwm
*/
//#include "config.h"
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
//#include "libs/defaults.h"
//#include "Module.h"
//#include "safemalloc.h"

#include "wm.h"
#include "module.h"
#include "global.h"
#include <stdlib.h>
#include <signal.h>
#include "xincludes.h"
#include "Parse.h"


//#define MAX_TOKEN_LENGTH 1023

Boolean FvwmModuleMode;
int fd_width;

unsigned long window = 0L;
int fd[2];

extern Atom a_NET_DESKTOP_GEOMETRY;
extern Atom a_NET_DESKTOP_VIEWPORT;



void goto_page(Widget w, Display *display, int x, int y)
{
    XClientMessageEvent xev;
    int rootwidth, rootheight;
    XWindowAttributes attrib;

    XGetWindowAttributes(display, DefaultRootWindow(display), &attrib);
    rootwidth=attrib.width;
    rootheight=attrib.height;

    xev.type = ClientMessage;
//    xev.window = XtWindow(w);
    xev.window = DefaultRootWindow(display);
    xev.send_event = True;
    xev.display = display;
    xev.message_type = a_NET_DESKTOP_VIEWPORT;
    xev.format = 32;
    xev.data.l[0] = rootwidth * x;
    xev.data.l[1] = rootheight * y;
    xev.data.l[2] = 0;
    xev.data.l[3] = 0;
    xev.data.l[4] = 0;
    XSendEvent(display, DefaultRootWindow(display), False, (SubstructureNotifyMask | SubstructureRedirectMask), (XEvent *) & xev);

}

int get_current_viewport(Window win, Display *display)
{
    Atom type_ret;
    int format_ret;
    unsigned long items_ret;
    unsigned long after_ret;
    unsigned char *prop_data;
    unsigned long *data;
    int rootwidth, rootheight;
    int x,y;
    XWindowAttributes attrib;

    prop_data = NULL;

    XGetWindowAttributes(display, win, &attrib);
    rootwidth=attrib.width;
    rootheight=attrib.height;

    if (XGetWindowProperty (display, win, a_NET_DESKTOP_VIEWPORT, 0, 0x7fffffff, False,
              XA_CARDINAL, &type_ret, &format_ret, &items_ret,
              &after_ret, &prop_data) != Success)
	return -1;
#ifdef DEBUG
    printf("win=%x rtype=%d rformat=%d nitems=%d\n", win, type_ret, format_ret, items_ret);

    printf("Rootwindow w=%d, h=%d\n", rootwidth, rootheight);
#endif

    if (items_ret > 0)
	{
	data = (unsigned long*) prop_data;
	x = *data/rootwidth;
	y = *(data+1)/rootheight;

#ifdef DEBUG
	printf("Desktop viewport x= %d y= %d\n", x, y);
#endif
	XFree(prop_data);
	return y*desks.x1+x;
	}

}

get_desktop_size(Window win, Display *display)
{
    Atom type_ret;
    int format_ret;
    unsigned long items_ret;
    unsigned long after_ret;
    unsigned char *prop_data;
    unsigned long *data;
    int rootwidth, rootheight;
    int x,y;
    XWindowAttributes attrib;

    prop_data = NULL;

    XGetWindowAttributes(display, win, &attrib);
    rootwidth=attrib.width;
    rootheight=attrib.height;

    if (XGetWindowProperty (display, win, a_NET_DESKTOP_GEOMETRY, 0, 0x7fffffff, False,
              XA_CARDINAL, &type_ret, &format_ret, &items_ret,
              &after_ret, &prop_data) != Success)
	return ;

#ifdef DEBUG
    printf("win=%x rtype=%d rformat=%d nitems=%d\n", win, type_ret, format_ret, items_ret);

    printf("Rootwindow w=%d, h=%d\n", rootwidth, rootheight);
#endif

    if (items_ret > 0)
	{

	data = (unsigned long*) prop_data;
	x = *data/rootwidth;
	y = *(data+1)/rootheight;

#ifdef DEBUG
	printf("Desktop size w= %ld h= %ld x= %d y= %d\n", *data, *(data+1), x, y);
#endif
	desks.x1=x;
	desks.y1=y;
	}
    XFree(prop_data);
}

/*
** DoPeekToken: returns next token from string, leaving string intact
**		(you must not free returned string)
**
** WARNING: The returned pointer points to a static array that will be
**	     overwritten in all functions in this file!
**
** For a description of the parameters see DoGetNextToken below. DoPeekToken
** is a bit faster.
*/
/* NOTE: If indata is the pointer returned by a previous call to PeekToken or
 * DoPeekToken, the input string will be destroyed. */
/*char *DoPeekToken(
	char *indata, char **token, char *spaces, char *delims, char *out_delim)
{
	char *end;
	int snum;
	int dnum;
	static char tmptok[MAX_TOKEN_LENGTH];

	snum = (spaces) ? strlen(spaces) : 0;
	dnum = (delims) ? strlen(delims) : 0;
	if (indata == NULL)
	{
		if (out_delim)
		{
			*out_delim = '\0';
		}
		*token = NULL;
		return NULL;
	}
	indata = SkipSpaces(indata, spaces, snum);
	end = CopyToken(indata, tmptok, spaces, snum, delims, dnum, out_delim);

	if (tmptok[0] == 0)
	{
		*token = NULL;
	}
	else
	{
		*token = tmptok;
	}

	return end;
}
*/
/*
 * PeekToken takes the input string "indata" and returns a pointer to the
 * token, stored in a static char array.  The pointer is invalidated by
 * the next call to PeekToken.  If "outdata" is not NULL, the pointer
 * to the first character after the token is returned through
 * *outdata. (Note that outdata is a char **, not just a char *).
 */
/*char *PeekToken(char *indata, char **outdata)
{
	char *dummy;
	char *token;

	if (!outdata)
	{
		outdata = &dummy;
	}
	*outdata = DoPeekToken(indata, &token, NULL, NULL, NULL);

	return token;
}
*/
int StrEquals( const char *s1, const char *s2 )
{
	if (s1 == NULL && s2 == NULL)
	{
		return 1;
	}
	if (s1 == NULL || s2 == NULL)
	{
		return 0;
	}

	return strcasecmp(s1,s2) == 0;
}

void DeadPipe(int foo) {
   fprintf(stderr, "Quitting because Fvwm is quitting...\n");
   exit(0);
}

int wm_init(int argc, char **argv, XtAppContext app) {
char *str=NULL;
char *tline= NULL;
  int dx = 3;
  int dy = 3;

//   if((argc != 6)&&(argc != 7))
	if (argc < 6)
	{
#ifdef DEBUG
		printf("\n not module\n");
#endif
		FvwmModuleMode=False;
   desks.x1=0;
   desks.y1=0;
      return 0;
	}
	FvwmModuleMode=True;
   fd[0] = atoi(argv[1]);
   fd[1] = atoi(argv[2]);

   signal(SIGPIPE, DeadPipe);
   fd_width = sysconf(_SC_OPEN_MAX);
   if (fd_width > 2048) fd_width = 2048;
  SetMessageMask(fd,
		 M_CONFIGURE_WINDOW|
		 M_DESTROY_WINDOW|
		 M_NEW_PAGE|
		 M_NEW_DESK|
		 M_CONFIG_INFO|
		 M_END_CONFIG_INFO);
  SetMessageMask(fd,
		 MX_VISIBLE_ICON_NAME|
		 MX_PROPERTY_CHANGE);
//   MdtVaString(&str, "*", APP_NAME, NULL);

//  InitGetConfigLine(fd, str);
  InitGetConfigLine(fd, "*panel");

//   MdtStringFree(&str);

  for (GetConfigLine(fd, &tline); tline != NULL; GetConfigLine(fd, &tline))
  {
    int g_x, g_y, flags;
    unsigned width,height;
    char *resource;
    char *arg1;
    char *arg2;
    char *tline2;
    char *token;
    char *next;
    Bool MoveThresholdSetForModule = False;

    arg1 = arg2 = NULL;

    token = PeekToken(tline, &next);
    if (StrEquals(token, "DesktopSize"))
    {
      token = PeekToken(next, &next);
      if (token)
      {
	sscanf(token, "%d", &dx);
	token = PeekToken(next, &next);
	if (token)
	  sscanf(token, "%d", &dy);
      }
      continue;
    }
  }
   desks.x1=dx;
   desks.y1=dy;

//   MdtVaString(&str, "Style FPClickRaisesFocused FPClickRaisesUnfocused", """", APP_NAME, """", " Sticky, WindowListSkip, FPClickRaisesFocused, FPClickRaisesUnfocused",
//	NULL);

   MdtVaString(&str, "Style ", """", APP_NAME, """", " Sticky, WindowListSkip",
	NULL);

   SendFvwmCmd(str);   

   MdtStringFree(&str);

   return 1;
}

void SendFvwmCmd(char *message) {
  int w;
  
  SendText(fd, message, 0);
  return ;
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

/*void GotoPage(int x, int y) {
char str[20];

   if (FvwmModuleMode==True && x>=0 && y>=0)
   {
	sprintf(str, "GotoPage %d %d", x, y);
   SendFvwmCmd(str);
   }
   else
   {
	   sprintf(str, "vdesk %d", x+1);
	   system(str);
   }
}
*/
extern void new_deskswitch_select(DeskRec *);

/*
 *
 *  Procedure:
 *      list_new_page - displays packet contents to stderr
 *
 */
void list_new_page(unsigned long *body)
{
char str1[40], str2[10];
int scrwidth, scrheight;
int col, row, n;

scrwidth=WidthOfScreen(screen);
scrheight=HeightOfScreen(screen);

col=body[0]/scrwidth;
row=body[1]/scrheight;
n=desks.x1*row+col;

//sprintf(str1, "Vx= %d Vy= %d", body[0], body[1]);
//sprintf(str1, "c= %d r= %d n= %d %s", col, row, n, desks.list[n]->label);
//MdtErrorDialog(layout.toplevel, str1);

new_deskswitch_select(desks.list[n]);


/*
  Scr.Vx = body[0];
  Scr.Vy = body[1];
  if (Scr.CurrentDesk != body[2])
  {*/
      /* first handle the new desk */
/*      body[0] = body[2];
      list_new_desk(body);
  }
  if (Scr.VxPages != body[5] || Scr.VyPages != body[6])
  {
    Scr.VxPages = body[5];
    Scr.VyPages = body[6];
    Scr.VWidth = Scr.VxPages * Scr.MyDisplayWidth;
    Scr.VHeight = Scr.VyPages * Scr.MyDisplayHeight;
    Scr.VxMax = Scr.VWidth - Scr.MyDisplayWidth;
    Scr.VyMax = Scr.VHeight - Scr.MyDisplayHeight;
    ReConfigure();
  }
  MovePage(False);
  MoveStickyWindows();
  Hilight(FocusWin,True);*/
}

void  process_message(FvwmPacket * packet )
{
  unsigned long type = packet->type;
  unsigned long length = packet->size;
  unsigned long* body = packet->body;

  switch (type)
    {
    case M_ADD_WINDOW:
//      list_configure(body);
      break;
    case M_CONFIGURE_WINDOW:
//      list_configure(body);
      break;
    case M_DESTROY_WINDOW:
//      list_destroy(body);
      break;
    case M_FOCUS_CHANGE:
//      list_focus(body);
      break;
    case M_NEW_PAGE:
      list_new_page(body);
//    MdtErrorDialog(layout.toplevel, "Message");
      break;
    case M_NEW_DESK:
//      list_new_desk(body);
      break;
    case M_RAISE_WINDOW:
//      list_raise(body);
      break;
    case M_LOWER_WINDOW:
//      list_lower(body);
      break;
    case M_ICONIFY:
    case M_ICON_LOCATION:
//      list_iconify(body);
      break;
    case M_DEICONIFY:
//      list_deiconify(body, length);
      break;
    case M_RES_CLASS:
    case M_RES_NAME:
    case M_VISIBLE_NAME:
//      list_window_name(body,type);
      break;
    case MX_VISIBLE_ICON_NAME:
//      list_icon_name(body);
      break;
    case M_MINI_ICON:
//      list_mini_icon(body);
      break;
    case M_END_WINDOWLIST:
//      list_end();
      break;
    case M_RESTACK:
//      list_restack(body,length);
      break;
    case M_CONFIG_INFO:
//      list_config_info(body);
      break;
    case MX_PROPERTY_CHANGE:
//      list_property_change(body);
      break;
//    case MX_REPLY:
//	    list_reply(body);
//	    break;
    default:
      /* ignore unknown packet */
      break;
    }
}


void GetWmCommand()
{
  fd_set in_fdset;
// struct timeval {
//             long    tv_sec;         /* seconds */
//             long    tv_usec;        /* microseconds */
//         };

 struct timeval tv;
   if (FvwmModuleMode==False) return;
	
  tv.tv_sec=0;
  tv.tv_usec=0;

  FD_ZERO(&in_fdset);
//  FD_SET(x_fd,&in_fdset);
  FD_SET(fd[1],&in_fdset);

  if (select(fd_width, &in_fdset, NULL, NULL, &tv) > 0)
  {

  if(FD_ISSET(fd[1], &in_fdset))
    {
      FvwmPacket* packet = ReadFvwmPacket(fd[1]);
      if ( packet == NULL )
	  exit(0);
      process_message( packet );
//    MdtErrorDialog(layout.toplevel, "Message");
	
    }
  }
  return 0;
	


}

/*
 * Loop until count bytes are read, unless an error or end-of-file
 * condition occurs.
 */
inline
static int positive_read(int fd, char *buf, int count)
{
	while (count > 0)
	{
		int n_read = read(fd, buf, count);
		if (n_read <= 0)
		{
			return -1;
		}
		buf += n_read;
		count -= n_read;
	}
	return 0;
}


/*
 * Reads a single packet of info from FVWM.
 * The packet is stored in static memory that is reused during
 * the next call.
 */

FvwmPacket *ReadFvwmPacket(int fd)
{
	static unsigned long buffer[FvwmPacketMaxSize];
	FvwmPacket *packet = (FvwmPacket *)buffer;
	unsigned long length;

	/* The `start flag' value supposedly exists to synchronize the
	 * FVWM -> module communication.  However, the communication goes
	 * through a pipe.  I don't see how any data could ever get lost,
	 * so how would FVWM & the module become unsynchronized?
	 */
	do
	{
		if (positive_read(fd, (char *)buffer, sizeof(unsigned long))
			< 0)
		{
			return NULL;
		}
	} while (packet->start_pattern != START_FLAG);

	/* Now read the rest of the header */
	if (positive_read(fd, (char *)(&buffer[1]), 3 * sizeof(unsigned long))
		< 0)
	{
		return NULL;
	}
	length = FvwmPacketBodySize_byte(*packet);
	if (length > FvwmPacketMaxSize_byte - FvwmPacketHeaderSize_byte)
	{
		/* packet too long */
		return NULL;
	}
	/* Finally, read the body, and we're done */
	if (positive_read(fd, (char *)(&buffer[4]), length) < 0)
	{
		return NULL;
	}

	return packet;
}


/*
 *
 * SendFinishedStartupNotification - informs fvwm that the module has
 * finished its startup procedures and is fully operational now.
 *
 */
void SendFinishedStartupNotification(int *fd)
{
	SendText(fd, ModuleFinishedStartupResponse, 0);
}

/*
 *
 * SendUnlockNotification - informs fvwm that the module has
 * finished it's procedures and fvwm may proceed.
 *
 */
void SendUnlockNotification(int *fd)
{
	SendText(fd, ModuleUnlockResponse, 0);
}

/*
 *
 * SendQuitNotification - informs fvwm that the module has
 * finished and may be killed.
 *
 */
static unsigned long ModuleContinue = 1;
void SendQuitNotification(int *fd)
{
	ModuleContinue = 0;
	SendText(fd, ModuleUnlockResponse, 0); /* unlock just in case */
}

/*
 *
 * SendText - Sends arbitrary text/command back to fvwm
 *
 */
void SendText(int *fd, const char *message, unsigned long window)
{
	char *p, *buf;
	unsigned int len;

	if (!message)
	{
		return;
	}

	/* Get enough memory to store the entire message. */
	len = strlen(message);
	p = buf = alloca(sizeof(long) * (3 + 1 + (len / sizeof(long))));

	/* Put the message in the buffer, and... */
	*((unsigned long *)p) = window;
	p += sizeof(unsigned long);

	*((unsigned long *)p) = len;
	p += sizeof(unsigned long);

	strcpy(p, message);
	p += len;

	memcpy(p, &ModuleContinue, sizeof(unsigned long));
	p += sizeof(unsigned long);

	/* Send it! */
	write(fd[0], buf, p - buf);
}

/*
 *
 * SendFvwmPipe - Sends message to fvwm:  The message is a comma-delimited
 * string separated into its component sections and sent one by one to fvwm.
 * It is discouraged to use this function with a "synchronous" module.
 * (Form FvwmIconMan)
 *
 */
void SendFvwmPipe(int *fd, const char *message, unsigned long window)
{
	const char *hold = message;
	const char *temp;

	while ((temp = strchr(hold, ',')) != NULL)
	{
		char *temp_msg = (char*)alloca(temp - hold + 1);

		strncpy(temp_msg, hold, (temp - hold));
		temp_msg[(temp - hold)] = '\0';
		hold = temp + 1;

		SendText(fd, temp_msg, window);
	}

	/*
	 * Send the last part of the string :
	 * we don't need to copy this into separate
	 * storage because we don't need to modify it ...
	 *
	 * NOTE: this makes this second call to SendText()
	 *       distinct from the first call. Two calls is
	 *       cleaner than hacking the loop to make only
	 *       one call.
	 */
	SendText(fd, hold, window);
}

void SetMessageMask(int *fd, unsigned long mask)
{
	char set_mask_mesg[50];

	sprintf(set_mask_mesg, "SET_MASK %lu", mask);
	SendText(fd, set_mask_mesg, 0);
}

void SetSyncMask(int *fd, unsigned long mask)
{
	char set_syncmask_mesg[50];

	sprintf(set_syncmask_mesg, "SET_SYNC_MASK %lu", mask);
	SendText(fd, set_syncmask_mesg, 0);
}

void SetNoGrabMask(int *fd, unsigned long mask)
{
	char set_nograbmask_mesg[50];

	sprintf(set_nograbmask_mesg, "SET_NOGRAB_MASK %lu", mask);
	SendText(fd, set_nograbmask_mesg, 0);
}

/*
 * Optional routine that sets the matching criteria for config lines
 * that should be sent to a module by way of the GetConfigLine function.
 *
 * If this routine is not called, all module config lines are sent.
 */
static int first_pass = 1;

void InitGetConfigLine(int *fd, char *match)
{
	char *buffer = (char *)alloca(strlen(match) + 32);
	first_pass = 0;              /* make sure get wont do this */
	sprintf(buffer, "Send_ConfigInfo %s", match);
	SendText(fd, buffer, 0);
}


/*
 * Gets a module configuration line from fvwm. Returns NULL if there are
 * no more lines to be had. "line" is a pointer to a char *.
 *
 * Changed 10/19/98 by Dan Espen:
 *
 * - The "isspace"  call was referring to  memory  beyond the end of  the
 * input area.  This could have led to the creation of a core file. Added
 * "body_size" to keep it in bounds.
 */
void GetConfigLine(int *fd, char **tline)
{
	FvwmPacket *packet;
	int body_count;

	if (first_pass)
	{
		SendText(fd, "Send_ConfigInfo", 0);
		first_pass = 0;
	}

	do
	{
		packet = ReadFvwmPacket(fd[1]);
		if (packet == NULL || packet->type == M_END_CONFIG_INFO)
		{
			*tline = NULL;
			return;
		}
	} while (packet->type != M_CONFIG_INFO);

	/* For whatever reason CONFIG_INFO packets start with three
	 * (unsigned long) zeros.  Skip the zeros and any whitespace that
	 * follows */
	*tline = (char *)&(packet->body[3]);
	body_count = FvwmPacketBodySize(*packet) * sizeof(unsigned long);

	while (body_count > 0 && isspace((unsigned char)**tline))
	{
		(*tline)++;
		--body_count;
	}
}


ModuleArgs *ParseModuleArgs(int argc, char *argv[], int use_arg6_as_alias)
{
	static ModuleArgs ma;

	/* Need at least six arguments:
	   [0] name of executable
	   [1] file descriptor of module->FVWM pipe (write end)
	   [2] file descriptor of FVWM->module pipe (read end)
	   [3] pathname of last config file read (ignored, use Send_ConfigInfo)
	   [4] application window context
	   [5] window decoration context

	   Optionally (left column used if use_arg6_as_alias is true):
	   [6] alias       or  user argument 0
	   [7] user arg 0  or  user arg 1
	   ...
	*/
	if (argc < 6)
	{
		return NULL;
	}

	/* Module name is (last component of) argv[0] or possibly an alias
	   passed on the command line. */
	if (use_arg6_as_alias && argc >= 7)
	{
		ma.name = argv[6];
		ma.user_argc = argc - 7;
		ma.user_argv = &(argv[7]);
	}
	else
	{
		char *p = strrchr(argv[0], '/');
		if (p == NULL)
		{
			ma.name = argv[0];
		}
		else
		{
			ma.name = ++p;
		}
		ma.user_argc = argc - 6;
		ma.user_argv = &(argv[6]);
	}

	ma.namelen=strlen(ma.name);

	if (ma.user_argc == 0)
	{
		ma.user_argv = NULL;
	}

	/* File descriptors for the pipes */
	ma.to_fvwm = atoi(argv[1]);
	ma.from_fvwm = atoi(argv[2]);

	/* Ignore argv[3] */

	/* These two are generated as long hex strings */
	ma.window = strtoul(argv[4], NULL, 16);
	ma.decoration = strtoul(argv[5], NULL, 16);

	return &ma;
}

/* expands certain variables in a command to be sent by a module */
/*char *module_expand_action(
	Display *dpy, int screen , char *in_action, rectangle *r,
	char *forecolor, char *backcolor)
{
	char *variables[] =
	{
		"$",
		"fg",
		"bg",
		"left",
		"-left",
		"right",
		"-right",
		"top",
		"-top",
		"bottom",
		"-bottom",
		"width",
		"height",
		NULL
	};
	char *action = NULL;
	char *src;
	char *dest;
	char *string = NULL;
	char *rest;
	int val = 0;
	int offset;
	int i;
	char *dest_org;
	Bool is_string;
	Bool is_value;
	Bool has_geom;
	Bool has_fg;
	Bool has_bg;
	rectangle tmpr = { 0, 0, 0, 0 };

	has_geom = (r == NULL) ? False : True;
	has_fg = (forecolor == NULL) ? False : True;
	has_bg = (backcolor == NULL) ? False : True;
	if (r == NULL)
	{
		r = &tmpr;
	}*/
	/* create a temporary storage for expanding */
/*	action = (char *)safemalloc(MAX_MODULE_INPUT_TEXT_LEN);
	for (src = in_action, dest = action; *src != 0; src++)
	{
		if (*src != '$')
		{
			*(dest++) = *src;
			continue;
		}*/
		/* it's a variable */
/*		dest_org = dest;
		is_string = False;
		is_value = False;
		*(dest++) = *(src++);
		i = GetTokenIndex(src, variables, -1, &rest);
		if (i == -1)
		{
			src--;
			continue;
		}
		switch (i)
		{*/
//		case 0: /* $ */
//			continue;
//		case 1: /* fg */
//			string = forecolor;
//			is_string = has_fg;
//			break;
//		case 2: /* bg */
//			if (backcolor == NULL)
//			{
//				continue;
//			}
//			string = backcolor;
//			is_string = has_bg;
//			break;
//		case 3: /* left */
//			val = r->x;
//			is_value = has_geom;
//			break;
//		case 4: /* -left */
//			val = DisplayWidth(dpy, screen) - r->x - 1;
//			is_value = has_geom;
//			break;
//		case 5: /* right */
//			val = r->x + r->width;
//			is_value = has_geom;
//			break;
//		case 6: /* -right */
//			val = DisplayWidth(dpy, screen) - r->x - r->width - 1;
//			is_value = has_geom;
//			break;
//		case 7: /* top */
//			val = r->y;
//			is_value = has_geom;
//			break;
//		case 8: /* -top */
//			val = DisplayHeight(dpy, screen) - r->y - 1;
//			is_value = has_geom;
//			break;
//		case 9: /* bottom */
//			val = r->y + r->height;
//			is_value = has_geom;
//			break;
//		case 10: /* -bottom */
//			val = DisplayHeight(dpy, screen) - r->y - r->height - 1;
//			is_value = has_geom;
//			break;
//		case 11: /* width */
//			val = r->width;
//			is_value = has_geom;
//			break;
//		case 12: /* height */
//			val = r->height;
//			is_value = has_geom;
//			break;
//		default: /* unknown */
//			src--;
//			continue;
//		} /* switch */
//		if (is_value == False && is_string == False)
//		{
//			src--;
//			continue;
//		}
//		dest = dest_org;
//		src = --rest;
//		if (is_value)
//		{
//			if (MAX_MODULE_INPUT_TEXT_LEN - (dest - action) <= 16)
//			{
//				/* out of space */
//				free(action);
//				return NULL;
//			}
			/* print the number into the string */
/*			sprintf(dest, "%d%n", val, &offset);
			dest += offset;
		}
		else if (is_string)
		{
			if (MAX_MODULE_INPUT_TEXT_LEN - (dest - action) <=
			    strlen(string))
			{*/
				/* out of space */
//				free(action);
//				return NULL;
//			}
			/* print the colour name into the string */
//			if (string)
//			{
//				sprintf(dest, "%s%n", string, &offset);
//				dest += offset;
//			}
//		}
//	} /* for */
//	*dest = 0;

//	return action;
//}

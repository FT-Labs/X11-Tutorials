#include <X11/X.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/Xatom.h>

#define POSX    500
#define POSY    500
#define WIDTH   500
#define HEIGHT  500
#define BORDER  10
#define LINE    4

static Display *dpy;
static int scr;
static Window root;


static Window create_win(int x, int y, int w, int h, int b)
{
	Window win;
	XSetWindowAttributes xwa = {.background_pixel = WhitePixel(dpy, scr), .border_pixel = BlackPixel(dpy, scr)};
	xwa.event_mask = Button1MotionMask | ButtonPressMask | ButtonReleaseMask | KeyPressMask;

	win = XCreateWindow(dpy, root, x, y, w, h, b, DefaultDepth(dpy, scr), InputOutput, DefaultVisual(dpy, scr),
			CWBackPixel | CWEventMask | CWBorderPixel, &xwa);
	return win;
}

static GC create_gc(int line_width)
{
	GC gc;
	XGCValues xgcv;
	unsigned long valuemask;

	xgcv.line_style = LineSolid;
	xgcv.line_width = line_width;
	xgcv.cap_style = CapButt;
	xgcv.join_style = JoinMiter;
	xgcv.fill_style = FillSolid;
	xgcv.foreground = BlackPixel(dpy, scr);
	xgcv.background = WhitePixel(dpy, scr);

	valuemask = GCForeground | GCBackground | GCFillStyle | GCLineStyle | GCLineWidth | GCCapStyle | GCJoinStyle;
	gc = XCreateGC(dpy, root, valuemask, &xgcv);
	return gc;
}

static void run(GC gc)
{
	XEvent ev;
	int init = 0;
	int prev_x = 0, prev_y = 0;

	while (!(XNextEvent(dpy, &ev)))
	{
		switch (ev.type)
		{
			case ButtonPress:
				if (ev.xbutton.button == Button1)
					XDrawPoint(dpy, ev.xbutton.window, gc, ev.xbutton.x, ev.xbutton.y);
				break;
			case MotionNotify:
				if (init)
				{
					XDrawLine(dpy, ev.xbutton.window, gc, prev_x, prev_y, ev.xbutton.x, ev.xbutton.y);
				}
				else
				{
					XDrawPoint(dpy, ev.xbutton.window, gc, ev.xbutton.x, ev.xbutton.y);
					init = 1;
				}
				prev_x = ev.xbutton.x, prev_y = ev.xbutton.y;
				break;
			case ButtonRelease:
				init = 0;
				break;
			case KeyPress:
				if (XkbKeycodeToKeysym(dpy, ev.xkey.keycode, 0, 0) == XK_q)
					return;
		}
	}
}


int main ()
{
	Window main_win;
	GC gc;
	if (!(dpy = XOpenDisplay(NULL)))
		errx(1, "Can't open display");

	scr = DefaultScreen(dpy);
	root = RootWindow(dpy, scr);

	main_win = create_win(POSX, POSY, WIDTH, HEIGHT, BORDER);
	gc = create_gc(LINE);

	XSizeHints xsh = {.min_width = WIDTH, .min_height = HEIGHT, .max_height = HEIGHT, .max_width = WIDTH};
	xsh.flags = PMinSize | PMaxSize;
	XSetSizeHints(dpy, main_win, &xsh, XA_WM_NORMAL_HINTS);

	XStoreName(dpy, main_win, "Paint Program");
	XMapWindow(dpy, main_win);

	run(gc);

	/* Clean up */
	XUnmapWindow(dpy, main_win);
	XDestroyWindow(dpy, main_win);
	XFreeGC(dpy, gc);

	XCloseDisplay(dpy);

	return 0;
}


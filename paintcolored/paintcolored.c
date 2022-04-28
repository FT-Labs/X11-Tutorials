#include <X11/X.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/Xatom.h>
#include <X11/Xft/Xft.h>

#define POSX    500
#define POSY    500
#define WIDTH   500
#define HEIGHT  500
#define BORDER  10
#define LINE    4
#define BLUE	"#98edfb"

static Display *dpy;
static int scr;
static Window root;
static Visual *vis;
static int swpcolor = 0;


static void create_color(XftColor *color_blue, const char* name)
{
	if (!XftColorAllocName(dpy, vis, DefaultColormap(dpy, scr), name, color_blue))
		errx(1, "Can't allocate color");

	color_blue->pixel |= 0xff << 24;
}


static Window create_win(int x, int y, int w, int h, int b)
{
	Window win;
	XSetWindowAttributes xwa = {.background_pixel = WhitePixel(dpy, scr), .border_pixel = BlackPixel(dpy, scr)};
	xwa.event_mask = Button1MotionMask | ButtonPressMask | ButtonReleaseMask | KeyPressMask;

	win = XCreateWindow(dpy, root, x, y, w, h, b, DefaultDepth(dpy, scr), InputOutput, vis,
			CWBackPixel | CWEventMask | CWBorderPixel, &xwa);
	return win;
}

static GC create_gc(int line_width, XftColor *fg)
{
	GC gc;
	XGCValues xgcv;
	unsigned long valuemask;

	xgcv.line_style = LineSolid;
	xgcv.line_width = line_width;
	xgcv.cap_style = CapButt;
	xgcv.join_style = JoinMiter;
	xgcv.fill_style = FillSolid;
	xgcv.foreground = fg->pixel; /* This sets foreground to blue */
	xgcv.background = WhitePixel(dpy, scr);

	valuemask = GCForeground | GCBackground | GCFillStyle | GCLineStyle | GCLineWidth | GCCapStyle | GCJoinStyle;
	gc = XCreateGC(dpy, root, valuemask, &xgcv);
	return gc;
}

static void run(GC gc, XftColor *color_blue)
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
				else if (ev.xbutton.button == Button3)
				{
					if (swpcolor)
					{
						XSetForeground(dpy, gc, BlackPixel(dpy, scr));
						swpcolor = 0;
					}
					else
					{
						XSetForeground(dpy, gc, color_blue->pixel);
						swpcolor = 1;
					}
				}
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
	XftColor *color_blue = malloc(sizeof(XftColor));

	if (!(dpy = XOpenDisplay(NULL)))
		errx(1, "Can't open display");

	scr = DefaultScreen(dpy);
	root = RootWindow(dpy, scr);
	vis = DefaultVisual(dpy, scr);

	main_win = create_win(POSX, POSY, WIDTH, HEIGHT, BORDER);
	create_color(color_blue, BLUE);
	gc = create_gc(LINE, color_blue);

	XSizeHints xsh = {.min_width = WIDTH, .min_height = HEIGHT, .max_height = HEIGHT, .max_width = WIDTH};
	xsh.flags = PMinSize | PMaxSize;
	XSetSizeHints(dpy, main_win, &xsh, XA_WM_NORMAL_HINTS);

	XStoreName(dpy, main_win, "Paint Program");
	XMapWindow(dpy, main_win);

	run(gc, color_blue);

	/* Clean up */
	XUnmapWindow(dpy, main_win);
	XDestroyWindow(dpy, main_win);
	XFreeGC(dpy, gc);
	XftColorFree(dpy, vis, DefaultColormap(dpy, scr), color_blue);

	XCloseDisplay(dpy);

	return 0;
}


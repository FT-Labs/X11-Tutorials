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
#define BORDER  5
#define LINE    4
#define BL		"#98edfb"
#define GR		"#00ff00"
#define RE		"#ff0000"

#define CHILD_W 50
#define CHILD_H 50
#define CHILD_B 10

enum {BLUE, GREEN, RED};
typedef XftColor Clr;

typedef struct {
	Window win;
	Clr *color;
} ColorButtons;

static Display *dpy;
static int scr;
static Window root;
static Visual *vis;
static ColorButtons color_buttons[3];
static const char *colors[3] = {BL, GR, RE};


static Window create_win(int x, int y, int w, int h, int b, Window* parent, int idx)
{
	Window win;
	XSetWindowAttributes xwa = {.background_pixel = WhitePixel(dpy, scr), .border_pixel = BlackPixel(dpy, scr)};

	if (parent == NULL)
	{
		xwa.event_mask = Button1MotionMask | ButtonPressMask | ButtonReleaseMask | KeyPressMask;

		win = XCreateWindow(dpy, root, x, y, w, h, b, DefaultDepth(dpy, scr), InputOutput, vis,
				CWBackPixel | CWEventMask | CWBorderPixel, &xwa);
	}
	else
	{
		xwa.event_mask = ButtonPress;
		xwa.background_pixel = color_buttons[idx].color->pixel;

		win = XCreateWindow(dpy, *parent, x, y, w, h, b, DefaultDepth(dpy, scr), InputOutput, vis,
				CWBackPixel | CWEventMask | CWBorderPixel, &xwa);
	}
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
	Window cur_win;
	int init = 0;
	int prev_x = 0, prev_y = 0;

	while (!(XNextEvent(dpy, &ev)))
	{
		switch (ev.type)
		{
			case ButtonPress:

				if (ev.xbutton.button == Button1)
				{
					cur_win = ev.xbutton.window;

					if (cur_win == color_buttons[BLUE].win)
						XSetForeground(dpy, gc, color_buttons[BLUE].color->pixel);
					else if (cur_win == color_buttons[RED].win)
						XSetForeground(dpy, gc, color_buttons[RED].color->pixel);
					else if (cur_win == color_buttons[GREEN].win)
						XSetForeground(dpy, gc, color_buttons[GREEN].color->pixel);
					else
						XDrawPoint(dpy, ev.xbutton.window, gc, ev.xbutton.x, ev.xbutton.y);
				}
				else if(ev.xbutton.button == Button3)
					XSetForeground(dpy, gc, BlackPixel(dpy, scr));
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
	int y = 0;

	if (!(dpy = XOpenDisplay(NULL)))
		errx(1, "Can't open display");

	scr = DefaultScreen(dpy);
	root = RootWindow(dpy, scr);
	vis = DefaultVisual(dpy, scr);
	main_win = create_win(POSX, POSY, WIDTH, HEIGHT, BORDER, NULL, 0);

	for (int i = 0; i < 3; i++)
	{
		color_buttons[i].color = malloc(sizeof(Clr));

		if (!color_buttons[i].color)
			errx(1, "Can't allocate memory for color");

		if (!XftColorAllocName(dpy, vis, DefaultColormap(dpy, scr), colors[i], color_buttons[i].color))
			errx(1, "Can't allocate xft color");

		color_buttons[i].color->pixel |= 0xff << 24;

		color_buttons[i].win = create_win(0, y, CHILD_W, CHILD_H, CHILD_B, &main_win, i);
		y += CHILD_H + CHILD_B;
	}

	gc = create_gc(LINE);

	XSizeHints xsh = {.min_width = WIDTH, .min_height = HEIGHT, .max_height = HEIGHT, .max_width = WIDTH};
	xsh.flags = PMinSize | PMaxSize;
	XSetSizeHints(dpy, main_win, &xsh, XA_WM_NORMAL_HINTS);

	XStoreName(dpy, main_win, "Paint Program");
	XMapWindow(dpy, main_win);
	XMapSubwindows(dpy, main_win);

	run(gc);

	/* Clean up */
	XUnmapWindow(dpy, main_win);
	XUnmapSubwindows(dpy, main_win);
	XDestroySubwindows(dpy, main_win);
	XDestroyWindow(dpy, main_win);
	XFreeGC(dpy, gc);

	for (int i = 0; i < 3; i++)
		XftColorFree(dpy, vis, DefaultColormap(dpy, scr), color_buttons[i].color);

	XCloseDisplay(dpy);

	return 0;
}


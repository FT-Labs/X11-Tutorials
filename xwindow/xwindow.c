#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <stdio.h>
#include <err.h>

#define POSX	500
#define POSY	500
#define WIDTH	750
#define HEIGHT	500
#define BORDER	25

static Display *dpy;
static int scr;
static Window root;

static Window create_win(int x, int y, int w, int h, int b)
{
	Window win;
	XSetWindowAttributes xwa;

	xwa.background_pixel = WhitePixel(dpy, scr);
	xwa.border_pixel = BlackPixel(dpy, scr);
	xwa.event_mask = ButtonPress;

	win = XCreateWindow(dpy, root, x, y, w, h, b, DefaultDepth(dpy, scr), InputOutput, DefaultVisual(dpy, scr),
			CWBackPixel | CWBorderPixel | CWEventMask, &xwa);
	return win;
}


static void run()
{
	XEvent ev;

	while (XNextEvent(dpy, &ev) == 0)
	{
		switch (ev.type)
		{
			case ButtonPress:
				return;
		}
	}
}


int main ()
{
	Window win;

	if ((dpy = XOpenDisplay(NULL)) == NULL)
		errx(1, "Can't open display");

	scr = DefaultScreen(dpy);
	root = RootWindow(dpy, scr);

	win = create_win(POSX, POSY, WIDTH, HEIGHT, BORDER);

	XMapWindow(dpy, win);

	run();

	/* Cleanup */
	XUnmapWindow(dpy, win);
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);

	return 0;
}


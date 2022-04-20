#include <X11/Xlib.h>
#include <stdio.h>
#include <err.h>


static Display* dpy;
static int scr;
static Window root;

#define POSY	500
#define POSX	500
#define WIDTH	500
#define HEIGHT	500
#define BORDER	15

int main ()
{
	Window win;
	XEvent ev;

	if ((dpy = XOpenDisplay(NULL)) == NULL)
		err(1, "Can't open display");

	/* get default screen and root window */
	scr = DefaultScreen(dpy);
	root = RootWindow(dpy, scr);

	/* Creating our simple window */
	win = XCreateSimpleWindow(dpy, root, POSX, POSY, WIDTH, HEIGHT, BORDER, BlackPixel(dpy, scr), WhitePixel(dpy, scr));
	/* Map our window to display server */
	XMapWindow(dpy, win);

	while (XNextEvent(dpy, &ev) == 0)
	{

	}

	/* unmap our simple window */
	// XUnmapWindow(dpy, win);
	// /* freeing our resources */
	// XDestroyWindow(dpy, win);
	// /* Close our connection with display server */
	// XCloseDisplay(dpy);

	return 0;
}


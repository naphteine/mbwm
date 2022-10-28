#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <X11/Xlib.h>

#define MAX(a, b)		((a) > (b) ? (a) : (b))

int
main(int argc, char *argv[])
{
	Display *dpy;
	XWindowAttributes attr;
	XEvent ev;
	XKeyEvent *kev;
	XButtonPressedEvent *bev;
	XButtonEvent start;

	if (!(dpy = XOpenDisplay(0x0))) return 1;

	XGrabButton(dpy, 1, Mod1Mask, DefaultRootWindow(dpy), True, ButtonPressMask|ButtonReleaseMask|PointerMotionMask,
			GrabModeAsync, GrabModeAsync, None, None);
	XGrabButton(dpy, 2, Mod1Mask, DefaultRootWindow(dpy), True, ButtonPressMask|ButtonReleaseMask|PointerMotionMask,
			GrabModeAsync, GrabModeAsync, None, None);
	XGrabButton(dpy, 3, Mod1Mask, DefaultRootWindow(dpy), True, ButtonPressMask|ButtonReleaseMask|PointerMotionMask,
			GrabModeAsync, GrabModeAsync, None, None);
	XGrabKey(dpy, XKeysymToKeycode(dpy, XStringToKeysym("t")), ControlMask|Mod1Mask, DefaultRootWindow(dpy),
			True, GrabModeAsync, GrabModeAsync);

	start.subwindow = None;
	XSync(dpy, False);

	while (1 && !XNextEvent(dpy, &ev)) {
		if (ev.type == KeyPress) {
			kev = &ev.xkey;
			if (kev->keycode == XKeysymToKeycode(dpy, XStringToKeysym("t"))) {
				if (fork() == 0) {
					if (dpy)
						close(ConnectionNumber(dpy));
					setsid();

					char *arg[2];
					arg[0] = "xterm";
					arg[1] = NULL;
					execvp(arg[0], arg);
					fprintf(stderr, "execvp xterm");
					perror(" failed");
					exit(EXIT_SUCCESS);
				}
			}
		} else if (ev.type == ButtonPress) {
			if (ev.xbutton.subwindow != None) {
				XGetWindowAttributes(dpy, ev.xbutton.subwindow, &attr);
				bev = &ev.xbutton;
				start = ev.xbutton;

				if (bev->button == 1 || bev->button == 2)
					XRaiseWindow(dpy, ev.xbutton.subwindow);
				else if (bev->button == 3)
					XLowerWindow(dpy, ev.xbutton.subwindow);

			}
		} else if (ev.type == MotionNotify) {
			if (ev.xbutton.subwindow != None) {
				int xdiff = ev.xbutton.x_root - start.x_root;
				int ydiff = ev.xbutton.y_root - start.y_root;
				XMoveResizeWindow(dpy, start.subwindow,
						attr.x + (start.button == 1 ? xdiff : 0),
						attr.y + (start.button == 1 ? ydiff : 0),
						MAX(1, attr.width + (start.button == 2 ? xdiff : 0)),
						MAX(1, attr.height + (start.button == 2 ? ydiff : 0)));
			}
		} else if (ev.type == ButtonRelease) {
			start.subwindow = None;
		}
	}
	return 0;
}

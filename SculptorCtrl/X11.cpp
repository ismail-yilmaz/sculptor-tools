#include "SculptorCtrl.h"

namespace Upp {

#ifdef PLATFORM_POSIX

#include <GL/glx.h>
#include <GL/gl.h>

#define Time    XTime
#define Font    XFont
#define Display XDisplay
#define Picture XPicture
#define Status  int

#ifdef GUI_GTK
#include <gdk/gdkx.h>
#endif

#undef Time
#undef Font
#undef Display
#undef Picture
#undef Status

static XVisualInfo                          *glxvisualinfo = nullptr;
static Colormap                              glxcolormap = 0;
static GLXContext                            glxmastercontext = nullptr;
static ::XDisplay                           *glxdisplay = nullptr;
static VectorMap<unsigned long, GLXContext>  glcontexts;

void SculptorCtrl::Create()
{
	MemoryIgnoreLeaksBlock __;

	Ctrl *top = GetTopCtrl();
	if(!top)
		return;

#ifdef GUI_GTK
	GdkWindow *gdk = top->gdk();
	if(!gdk)
		return;

	Window w = gdk_x11_window_get_xid(gdk);
#else
	Window w = top->GetWindow();
#endif

	ONCELOCK {
#ifdef GUI_GTK
		glxdisplay = GDK_DISPLAY_XDISPLAY(gdk_display_get_default());
#else
		glxdisplay = Xdisplay;
#endif
		int samples = msaasamples;

		do {
			Vector<int> attr;
			attr << GLX_RGBA << GLX_DEPTH_SIZE << depthbits
				<< GLX_STENCIL_SIZE << stencilbits;
			if(doublebuffering)
				attr << GLX_DOUBLEBUFFER;
			if(samples > 1)
				attr << GLX_SAMPLE_BUFFERS_ARB << 1 << GLX_SAMPLES_ARB << samples;
			attr << 0;
			samples >>= 1;
			glxvisualinfo = glXChooseVisual(glxdisplay, DefaultScreen(glxdisplay), attr);
		}
		while(!glxvisualinfo && samples > 0);
		
		if(glxvisualinfo) {
			glxcolormap = XCreateColormap(glxdisplay, RootWindow(glxdisplay, glxvisualinfo->screen), glxvisualinfo->visual, AllocNone);
			glxmastercontext = glXCreateContext(glxdisplay, glxvisualinfo, nullptr, GL_TRUE);
		}
	}
	
	if(!glxvisualinfo)
		return;

	XSetWindowAttributes swa;
	swa.colormap = glxcolormap;
	swa.border_pixel = 0;
	swa.event_mask = 0;

	win = XCreateWindow(glxdisplay, w, 0, 0, 1, 1, 0,
						glxvisualinfo->depth, InputOutput, glxvisualinfo->visual,
						CWBorderPixel | CWColormap | CWEventMask, &swa);
						
	if(win) {
		GLXContext ctx = glXCreateContext(glxdisplay, glxvisualinfo, glxmastercontext, GL_TRUE);
		if(ctx)
			glcontexts.Add(win, ctx);
	}

	visible = false;
	position = Null;
}

void SculptorCtrl::Destroy()
{
	if(win) {
		int idx = glcontexts.Find(win);
		if(idx >= 0) {
			glXDestroyContext(glxdisplay, glcontexts[idx]);
			glcontexts.Remove(idx);
		}
		XDestroyWindow(glxdisplay, win);
		win = 0;
	}
}

void SculptorCtrl::Sync()
{
	MemoryIgnoreLeaksBlock __;

	if(win) {
		Rect r = GetScreenView() - GetTopCtrl()->GetScreenRect().TopLeft();
		bool b = IsVisible() && r.GetWidth() > 0 && r.GetHeight() > 0;
		if(b != visible) {
			visible = b;
			position = Null;
			if(b)
				XMapWindow(glxdisplay, win);
			else
				XUnmapWindow(glxdisplay, win);
		}
		if(r != position && visible) {
			position = r;
			XMoveResizeWindow(glxdisplay, win, r.left, r.top, r.Width(), r.Height());
		}
	}
}

void SculptorCtrl::State(int reason)
{
	switch(reason) {
	case CLOSE:
		Destroy();
		break;
	case OPEN:
		Create();
	default:
		Sync();
		break;
	}
}

void SculptorCtrl::MakeGLContext()
{
}

void SculptorCtrl::CreateContext()
{
}

void SculptorCtrl::ExecuteGL(Event<> gl, bool swapbuffers)
{
	MemoryIgnoreLeaksBlock __;

	if(!win)
		return;

	int idx = glcontexts.Find(win);
	if(idx < 0)
		return;

	GLXContext ctx = glcontexts[idx];
	glXMakeCurrent(glxdisplay, win, ctx);

	ONCELOCK {
		glewInit();
	}

	gl();

	if(swapbuffers)
		glXSwapBuffers(glxdisplay, win);
	else
		glFlush();

	glXMakeCurrent(glxdisplay, None, nullptr);
}

void SculptorCtrl::Paint(Draw& w)
{
	Size sz = GetSize();
	if(sz.cx == 0 || sz.cy == 0 || !win)
		return;
		
	if(int i = glcontexts.Find(win); i >= 0)
		ExecuteGL([&] { DoRender(); }, doublebuffering);
}

#endif

}
#include "SculptorCtrl.h"

namespace Upp {

#ifdef PLATFORM_WIN32

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

static PIXELFORMATDESCRIPTOR glpfd;
static int                   glpixelformat = 0;
static HWND                  glmasterhwnd = nullptr;
static HDC                   glmasterhdc = nullptr;
static HGLRC                 glmastercontext = nullptr;

void SculptorCtrl::MakeGLContext()
{
	ONCELOCK {
		for(int pass = 0; pass < 2; pass++) {
			HWND hwnd = CreateWindow("UPP-CLASS-W", "Fake Window",
									WS_CAPTION | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
									0, 0, 1, 1, nullptr, nullptr,
									nullptr, nullptr);
			if(!hwnd)
				return;
			
			HDC hdc = ::GetDC(hwnd);
			if(!hdc)
				return;
			
			memset(&glpfd, 0, sizeof(glpfd));
			if(pass == 0) {
				glpfd.nSize = sizeof(glpfd);
				glpfd.nVersion = 1;
				glpfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_GENERIC_ACCELERATED | PFD_GENERIC_FORMAT | PFD_DOUBLEBUFFER_DONTCARE;
				glpfd.iPixelType = PFD_TYPE_RGBA;
				glpfd.cColorBits = 32;
				glpfd.cAlphaBits = 8;
				glpfd.cDepthBits = 24;
				glpfd.cStencilBits = 8;
				glpfd.iLayerType = PFD_MAIN_PLANE;
				glpixelformat = ChoosePixelFormat(hdc, &glpfd);
			}
			else {
				Vector<int> attr;
				attr << WGL_DRAW_TO_WINDOW_ARB << GL_TRUE
					 << WGL_SUPPORT_OPENGL_ARB << GL_TRUE
					 << WGL_DOUBLE_BUFFER_ARB << GL_TRUE
					 << WGL_PIXEL_TYPE_ARB << WGL_TYPE_RGBA_ARB
					 << WGL_ACCELERATION_ARB << WGL_FULL_ACCELERATION_ARB
					 << WGL_COLOR_BITS_ARB << 32
					 << WGL_ALPHA_BITS_ARB << 8
					 << WGL_DEPTH_BITS_ARB << depthbits
					 << WGL_STENCIL_BITS_ARB << stencilbits;
				
				if(msaasamples > 1 && WGLEW_ARB_multisample) {
					attr << WGL_SAMPLE_BUFFERS_ARB << GL_TRUE
						 << WGL_SAMPLES_ARB << msaasamples;
				}
				
				attr << 0;
			
				UINT numformats = 0;
				if(!wglChoosePixelFormatARB(hdc, attr, nullptr, 1, &glpixelformat, &numformats) || numformats == 0)
					return;
			}
			
			DescribePixelFormat(hdc, glpixelformat, sizeof(PIXELFORMATDESCRIPTOR), &glpfd);
			if(!SetPixelFormat(hdc, glpixelformat, &glpfd))
				return;
			
			if(pass == 0) {
				HGLRC tmprc = wglCreateContext(hdc);
				wglMakeCurrent(hdc, tmprc);
			
				glewInit();
				bool enhancedmode = glewIsSupported("GL_VERSION_2_1") != 0;
				if(enhancedmode) {
					wglewInit();                                 // Resolves wglChoosePixelFormatARB etc.
					enhancedmode = WGLEW_ARB_pixel_format != 0;  // Actual gate for pass 1, not GL version
				}
			
				wglMakeCurrent(nullptr, nullptr);
				wglDeleteContext(tmprc);
				ReleaseDC(hwnd, hdc);
				DestroyWindow(hwnd);
			
				if(!enhancedmode)
					break;
			}
			else {
				glmasterhwnd = hwnd;
				glmasterhdc = hdc;
				glmastercontext = wglCreateContext(hdc);
			}
		}
	}
}

void SculptorCtrl::CreateContext()
{
	MakeGLContext();
}

void SculptorCtrl::GLPane::Destroy()
{
	if(glcontext) {
		wglDeleteContext(glcontext);
		glcontext = nullptr;
	}
}

void SculptorCtrl::GLPane::State(int reason)
{
	DHCtrl::State(reason);
	
	if(reason == OPEN) {
		HWND hwnd = GetHWND();
		SculptorCtrl::CreateContext();
		HDC hdc = GetDC(hwnd);
		if(SetPixelFormat(hdc, glpixelformat, &glpfd)) {
			glcontext = wglCreateContext(hdc);
			if(glcontext && glmastercontext)
				wglShareLists(glmastercontext, glcontext);
		}
		ReleaseDC(hwnd, hdc);
	}
	else
	if(reason == CLOSE)
		Destroy();
}

void SculptorCtrl::GLPane::ExecuteGL(HDC hdc, Event<> render, bool swapbuffers)
{
	if(!glcontext)
		return;

	wglMakeCurrent(hdc, glcontext);
	render();
	if(swapbuffers)
		SwapBuffers(hdc);
	else
		glFlush();
	wglMakeCurrent(nullptr, nullptr);
}

void SculptorCtrl::GLPane::ExecuteGL(Event<> render, bool swapbuffers)
{
	HWND hwnd = GetHWND();
	SculptorCtrl::CreateContext();
	HDC hdc = GetDC(hwnd);
	ExecuteGL(hdc, render, swapbuffers);
	ReleaseDC(hwnd, hdc);
}

LRESULT SculptorCtrl::GLPane::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_PAINT && glcontext) {
		PAINTSTRUCT ps;
		HWND hwnd = GetHWND();
		BeginPaint(hwnd, &ps);
		ExecuteGL(ps.hdc, [&] { ctrl->DoRender(); }, ctrl->doublebuffering);
		ReleaseDC(hwnd, ps.hdc);
		EndPaint(hwnd, &ps);
		return 0;
	}
	
	return DHCtrl::WindowProc(message, wParam, lParam);
}

Image SculptorCtrl::GLPane::MouseEvent(int event, Point p, int zdelta, dword keyflags)
{
	p = p - GetScreenView().TopLeft() + ctrl->GetScreenView().TopLeft();
	return ctrl->MouseEvent(event, p, zdelta, keyflags);
}

void SculptorCtrl::GLPane::Pen(Point p, const PenInfo& pen, dword keyflags)
{
	(ctrl->mousetarget ? ctrl->mousetarget : ctrl)->Pen(p, pen, keyflags);
}

void SculptorCtrl::ExecuteGL(Event<> gl, bool swapbuffers)
{
	pane.ExecuteGL(gl, swapbuffers);
}

#endif

}
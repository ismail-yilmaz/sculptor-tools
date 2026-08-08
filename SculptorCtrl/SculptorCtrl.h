#ifndef _Upp_SculptorCtrl_h_
#define _Upp_SculptorCtrl_h_

#include <CtrlCore/CtrlCore.h>
#include <Xform3D/Xform3D.h>

#if defined(GUI_X11) || defined(GUI_GTK)
#define Time    XTime
#define Font    XFont
#define Display XDisplay
#define Picture XPicture
#define Status  int
#endif

#define GLEW_STATIC

#include <plugin/glew/glew.h>

#ifdef PLATFORM_WIN32
#include <plugin/glew/wglew.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#if defined(GUI_X11) || defined(GUI_GTK)
#include <GL/glx.h>
#undef Status
#undef Picture
#undef Time
#undef Font
#undef Display
#endif

namespace Upp {

#include "shaders.brc"

#include "Model.h"
#include "Pipeline.h"
#include "Scene.h"

class SculptorCtrl : public Ctrl {
public:
    typedef SculptorCtrl CLASSNAME;

    SculptorCtrl();
    SculptorCtrl(int depthbits, int stencilbits = 8, bool doublebuffer = true, int samples = 0);
    virtual ~SculptorCtrl()                              {}

    static void   SetDepthBits(int bits)                 { depthbits = bits; }
    static void   SetStencilBits(int bits)               { stencilbits = bits; }
    static void   SetDoubleBuffering(bool enable = true) { doublebuffering = enable; }
    static void   SetMSAA(int samples = 4)               { msaasamples = samples; }

    static void   CreateContext();
    static Size   CurrentViewport()                      { return currentviewport; }
    static void   SetCurrentViewport();

    virtual Image MouseEvent(int event, Point p, int zdelta, dword keyflags) override;
    SculptorCtrl& RedirectMouse(Ctrl *target)            { mousetarget = target; return *this; }
    void          ExecuteGL(Event<> gl, bool swapbuffers = false);

    void          SetShader(const String& name, const String& vshader, const String& fshader);
    void          AddShader(const String& name, const String& vshader, const String& fshader);
    void          RemoveShader(const String& name);
    void          ClearShaders();
    
    void          SetPostProcess(const String& fshader);
    void          ClearPostProcess();

	void          RemoveTexture(int64 tid);
	void          ClearTextures();

    virtual void  InitScene()                            { WhenInitScene();    }
    void          ClearScene()                           { glpipeline.Clear(); }
    virtual void  Render(GLScene& scene)                 { WhenRender(scene);  }
    
    Event<>         WhenInitScene;
    Event<GLScene&> WhenRender;

private:
    static void   MakeGLContext();
    void          DoRender();
    void          Init();

    static int    depthbits;
    static int    stencilbits;
    static bool   doublebuffering;
    static int    msaasamples;

    static Size   currentviewport;

    Ptr<Ctrl>     mousetarget;
    GLPipeline    glpipeline;
    bool          initialized = false;

#ifdef PLATFORM_WIN32
public:
    void          Refresh()                              { pane.Refresh(); }
    
private:
    struct GLPane : DHCtrl {
        friend class SculptorCtrl;
        SculptorCtrl *ctrl;
        HGLRC      glcontext = NULL;

        GLPane()                                         { NoWantFocus(); }
        ~GLPane()                                        { Destroy(); }

        virtual void    State(int reason) final;
        virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) final;
        virtual Image   MouseEvent(int event, Point p, int zdelta, dword keyflags) final;
        virtual void    Pen(Point p, const PenInfo& pen, dword keyflags) final;

        void            Destroy();
        void            ExecuteGL(HDC hdc, Event<> render, bool swapbuffers);
        void            ExecuteGL(Event<> render, bool swapbuffers);
    };

    friend struct GLPane;
    GLPane pane;
#endif

#ifdef PLATFORM_POSIX
private:
    unsigned long win = 0;
    bool          visible = false;
    Rect          position;

    void          Create();
    void          Sync();
    void          Destroy();

    virtual void  State(int reason) final;
    virtual void  Paint(Draw& w) final;
#endif
};

int64 GetCurrentGLContext();

}

#endif
#ifndef _SculptorCtrlDemo_h
#define _SculptorCtrlDemo_h

#include <CtrlLib/CtrlLib.h>
#include <SculptorCtrl/SculptorCtrl.h>

namespace Upp {

class ObjectViewer : public SculptorCtrl {
public:
    ObjectViewer();

    void            LoadModel();
    void            InitScene() override;
    void            Render(GLScene& gls) override;
    void            AddOverlay(GLScene& gls);
    
    bool            Key(dword key, int count) override;
    void            LeftDown(Point p, dword keyflags) override;
    void            LeftUp(Point p, dword keyflags) override;
    void            MiddleDown(Point p, dword keyflags) override;
    void            MiddleUp(Point p, dword keyflags) override;
    void            MouseWheel(Point p, int zdelta, dword keyflags) override;
    void            MouseMove(Point p, dword keyflags) override;

private:
    ModelGL         model;
    Vector<String>  files;
    
    int             modelindex    = 0;
    float           zoom          = 5.0f;
    float           goal          = 5.0f;
    
    Pointf          rot           = { 0.0f, 0.0f };
    Pointf          goalrot       = { 0.0f, 0.0f };
    Pointf          pan           = { 0.0f, 0.0f };
    Pointf          goalpan       = { 0.0f, 0.0f };
    
    float           gradbase      = 0.0f;
    float           gradspan      = 0.0f;
    bool            dragging      = false;
    bool            panning       = false;
    bool            wireframe     = false;
    bool            xray          = false;
    bool            alphablend    = false;
    bool            bilinear      = true;
    bool            mipmap        = true;
    bool            illum         = true;
    bool            trisample     = false;
    bool            vignette      = false;
    bool            gradient      = false;
    bool            rimshade      = false;
    bool            isometric     = false;
    
    Point           mouse         = { 0, 0 };
    Point           panmouse      = { 0, 0 };
    Pointf          mousenorm     = { 0.5f, 0.5f };
    Pointf          prevmousenorm = { -1.0f, -1.0f };
    RGBA            colorlo       = Color(40, 60, 160);
    RGBA            colorhi       = Color(255, 200, 120);
};

}

#endif
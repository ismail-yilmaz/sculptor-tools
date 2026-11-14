#ifndef _SculptorCtrlDemo_h
#define _SculptorCtrlDemo_h

#include <CtrlLib/CtrlLib.h>
#include <SculptorCtrl/SculptorCtrl.h>

namespace Upp {

struct SceneModel {
    ModelGL model;
    Point3D pos;
    Image   tag;
    String  name;
    float   gradbase = 0.0f;
    float   gradspan = 0.0f;
};

class ObjectViewer : public SculptorCtrl {
public:
    ObjectViewer();

    void     LoadModel();
    void     LoadModels();
    void     InitScene() override;

    MatrixGL GetModelTransform() const;
    MatrixGL GetModelTransform(int index) const;
    void     UpdateHover(const GLScene& gls, Size sz);
    void     PickAt(Point p);
    void     ClearPick();
    Image    GetHoverTag(int index, const String& name, const ModelGL& m, const Ray3D::Hit& hit, bool picked);
    void     RenderScene(GLScene& gls, Sizef sz);
    void     AddOverlay(GLScene& gls);
    void     Render(GLScene& gls) override;

    bool     Key(dword key, int count) override;
    void     LeftDown(Point p, dword keyflags) override;
    void     LeftUp(Point p, dword keyflags) override;
    void     MiddleDown(Point p, dword keyflags) override;
    void     MiddleUp(Point p, dword keyflags) override;
    void     MouseWheel(Point p, int zdelta, dword keyflags) override;
    void     MouseMove(Point p, dword keyflags) override;
    void     MouseLeave() override;
    Image    CursorImage(Point p, dword keyflags) override;

private:
    ModelGL            model;
    Image              modeltag;
    Image              hovermarker;
    Image              pickmarker;
    Image              hovertag;
    Image              hudimg;
    String             hudtxt;
    Vector<String>     files;
    Array<SceneModel>  models;

    Ray3D::Hit         hoverhit;
    Ray3D::Hit         pickhit;
    Point3D            hovertagpos    = Null;
    Point3D            hovertagnorm   = Null;

    int                modelindex     = 0;
    int                hoverindex     = -1;
    int                pickindex      = -1;
    int                hovertagidx    = -1;
    int                hovertagtri    = -1;
    float              hovertagdist   = -1.0f;
    bool               hovertagpicked = false;
    bool               pickpending    = false;
    bool               hoverdirty     = false;
    float              modeltagy      = 0.0f;
    float              zoom           = 5.0f;
    float              goal           = 5.0f;

    Pointf             rot            = { 0.0f, 0.0f };
    Pointf             goalrot        = { 0.0f, 0.0f };
    Pointf             pan            = { 0.0f, 0.0f };
    Pointf             goalpan        = { 0.0f, 0.0f };

    float              gradbase       = 0.0f;
    float              gradspan       = 0.0f;
    bool               dragging       = false;
    bool               panning        = false;
    bool               wireframe      = false;
    bool               xray           = false;
    bool               alphablend     = false;
    bool               bilinear       = true;
    bool               mipmap         = true;
    bool               illum          = true;
    bool               trisample      = false;
    bool               vignette       = false;
    bool               gradient       = false;
    bool               rimshade       = false;
    bool               isometric      = false;
    bool               multiobject    = false;

    Point              mouse          = Point(-1, -1);
    Point              clickmouse     = Point(-1, -1);
    Point              panmouse       = Point(0, 0);
    RGBA               colorlo        = Color(40, 60, 160);
    RGBA               colorhi        = Color(255, 200, 120);
};

}

#endif
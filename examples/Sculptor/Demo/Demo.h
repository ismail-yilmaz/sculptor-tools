#ifndef _SculptorDemo_h
#define _SculptorDemo_h

#include <CtrlLib/CtrlLib.h>
#include <Sculptor/Sculptor.h>

namespace Upp {

struct SceneModel {
    Model3D model;
    Point3D pos;
    Image   tag;
    String  name;
    float   gradbase = 0.0f;
    float   gradspan = 0.0f;
};

class ObjectViewer : public TopWindow {
public:
    ObjectViewer();
    
    void     LoadModel();
    void     LoadModels();
    void     SetGradient();

    Matrix4D GetModelTransform() const;
    Matrix4D GetModelTransform(int index) const;
    bool     UpdateHover();
    void     PickAt(Point p);
    void     ClearPick();
    Image    GetHoverTag(int index, const String& name, const Model3D& m, const Ray3D::Hit& hit, bool picked);
    void     RenderScene(Size sz);
    
    bool     Key(dword key, int count) override;
    void     LeftDown(Point p, dword keyflags) override;
    void     LeftUp(Point p, dword keyflags) override;
    void     MouseWheel(Point p, int zdelta, dword keyflags) override;
    void     MouseMove(Point p, dword keyflags) override;
    void     MouseLeave() override;
    Image    CursorImage(Point p, dword keyflags) override;

    void     PaintOverlay(Draw& w) const;
    void     Paint(Draw& w) override;

private:
    Sculptor          renderer;
    Model3D           model;
    Image             modeltag;
    Image             hovermarker;
    Image             pickmarker;
    Image             hovertag;
    mutable Image     hudimg;
    mutable String    hudtxt;
    Vector<String>    files;
    Array<SceneModel> models;

    Ray3D::Hit        hoverhit;
    Ray3D::Hit        pickhit;
    Point3D           hovertagpos    = Null;
    Point3D           hovertagnorm   = Null;

    int               modelindex     = 0;
    int               hoverindex     = -1;
    int               pickindex      = -1;
    int               hovertagidx    = -1;
    int               hovertagtri    = -1;
    float             hovertagdist   = -1.0f;
    bool              hovertagpicked = false;
    float             modeltagy      = 0.0f;
    float             zoom           = 5.0f;
    float             goal           = 5.0f;
    float             rotx           = 0.0f;
    float             roty           = 0.0f;
    float             goalx          = 0.0f;
    float             goaly          = 0.0f;
    bool              wireframe      = false;
    bool              bilinear       = true;
    bool              mipmap         = true;
    bool              gradient       = false;
    bool              illum          = true;
    bool              triplanar      = false;
    bool              rim            = false;
    bool              vignette       = false;
    bool              dragging       = false;
    bool              multiobject    = false;
    Point             mouse          = Point(-1, -1);
    Point             clickmouse     = Point(-1, -1);
};

// Shaders and post-processors
void ApplyGradient(Model3D& model, Color lo, Color hi);
RGBA TriplanarSampler(const Sculptor::Fragment& frag);
RGBA RimShader(const Sculptor::Fragment& frag, RGBA base);
void Vignette(ImageBuffer& cb, Buffer<float>& zb, Size sz);

}

#endif
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
    
    void LoadModel();
    void LoadModels();
    void SetGradient();
    
    bool Key(dword key, int count) override;
    void LeftDown(Point p, dword keyflags) override;
    void LeftUp(Point p, dword keyflags) override;
    void MouseWheel(Point p, int zdelta, dword keyflags) override;
    void MouseMove(Point p, dword keyflags) override;

    void PaintOverlay(Draw& w) const;
    void Paint(Draw& w) override;

private:
    Sculptor          renderer;
    Model3D           model;
    Vector<String>    files;
    Array<SceneModel> models;

    int       modelindex  = 0;
    float     zoom        = 5.0f;
    float     goal        = 5.0f;
    float     rotx        = 0.0f;
    float     roty        = 0.0f;
    float     goalx       = 0.0f;
    float     goaly       = 0.0f;
    bool      wireframe   = false;
    bool      bilinear    = true;
    bool      mipmap      = true;
    bool      gradient    = false;
    bool      illum       = true;
    bool      triplanar   = false;
    bool      rim         = false;
    bool      vignette    = false;
    bool      dragging    = false;
    bool      multiobject = false;
    Point     mouse       = Point(0, 0);
};

// Shaders and post-processors
void ApplyGradient(Model3D& model, Color lo, Color hi);
RGBA TriplanarSampler(const Sculptor::Fragment& frag);
RGBA RimShader(const Sculptor::Fragment& frag, RGBA base);
void Vignette(ImageBuffer& cb, Buffer<float>& zb, Size sz);

}

#endif
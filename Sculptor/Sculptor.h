#ifndef _Upp_Sculptor_h_
#define _Upp_Sculptor_h_

#include <Draw/Draw.h>
#include <Xform3D/Xform3D.h>

namespace Upp {

struct Model3D : Moveable<Model3D> {
public:
    struct Vertex : Moveable<Vertex> {
        Point3D              position;
        Point3D              normal;
        Pointf               texcoord;
        RGBA                 color;
        int64                textureid;
        Vertex();
    };
    
    struct Triangle : Moveable<Triangle> {
        int a;
        int b;
        int c;
        Triangle();
        Triangle(int pa, int pb, int pc);
    };

    Vector<Vertex>                  vertices;
    Vector<Triangle>                triangles;
    VectorMap<int64, Image>         textures;
    VectorMap<int64, Vector<Image>> mipmaps;
    RGBA                            defaultcolor;

    void                            Clear();
    
    void                            SetDefaultColor(const RGBA& c)                     { defaultcolor = c; }
    void                            Normalize(float radius);
    Model3D&                        AddVertex(const Vertex& v);
    Model3D&                        AddTriangle(int a, int b, int c);
    int64                           AddTexture(const Image& img);
    Model3D&                        AddBox(const Box3D& box, int64 tid = 0);
    Model3D&                        AddSphere(float radius, int slices, int stacks, int64 tid = 0);
    Box3D                           ComputeBounds() const;
    Point3D                         GetCenter() const;
    
    Model3D();
};

class Sculptor {
public:
    struct Primitive : Moveable<Primitive> {
        Pointf               screenpos[3];
        float                inversew[3];
        Point3D              viewpos[3];
        Pointf               texcoords[3];
        RGBA                 colors[3];
        Point3D              normals[3];
        const Image*         texture;
        const Vector<Image>* mipmaps;
        bool                 wireframe:1;
        bool                 translucent:1;
        bool                 shading:1;
    };

    struct Fragment : Moveable<Fragment> {
        Point3D               viewpos;
        Point3D               normal;
        Pointf                texcoord;
        RGBA                  color;
        const Image*          texture;
        const Vector<Image>*  mipmaps;
    };

public:
    Sculptor();
    Sculptor(Size sz, const RGBA& paper);
    virtual ~Sculptor()                                                                {}

    Sculptor&                Co(bool b = true)                                         { parallel = b; return *this; }
    bool                     IsCo() const                                              { return parallel; }
    
    Sculptor&                Cull(bool b = true)                                       { culling = b; return *this;  }
    bool                     IsCulling() const                                         { return culling; }
    
    Sculptor&                Wireframe(bool b = true)                                  { wireframe = b; return *this; }
    bool                     IsWireFrame() const                                       { return wireframe; }
    
    Sculptor&                Shading(bool b = true)                                    { shading = b; return *this; }
    bool                     IsShading() const                                         { return shading; }
    
    Sculptor&                AlphaBlend(bool b = true)                                 { alphablend = b; return *this; }
    bool                     IsAlphaBlending() const                                   { return alphablend; }
    
    Sculptor&                Bilinear(bool b = true)                                   { bilinear = b; return *this; }
    bool                     IsBilinear() const                                        { return bilinear; }
    
    Sculptor&                MipMap(bool b = true)                                     { mipmap = b; return *this; }
    bool                     IsMipMapping() const                                      { return mipmap; }
    
    Sculptor&                AmbientLight(const RGBA& c)                               { ambientlight = c; return *this; }
    
    Sculptor&                Scissor(const Rect& r)                                    { scissor = r; return *this; }
 
    void                     SetCanvas(Size sz, const RGBA& paper);
    void                     ClearCanvas(const RGBA& paper);
    void                     ClearColorBuffer(const RGBA& paper);
    void                     ClearDepthBuffer();
    
    void                     Perspective(float fov, float aspectratio, float znear, float zfar);
    void                     Orthographic(const Rectf& view, float znear, float zfar);
    void                     Frustum(const Rectf& view, float fnear, float ffar);
    void                     Isometric(const Rectf& view, float fnear, float ffar);
    void                     Isometric(float zoom, float aspectratio, float fnear, float ffar);

    void                     LookAt(const Point3D& eye, const Point3D& center, const Point3D& up);
    
    void                     PushMatrix();
    void                     PopMatrix();
    Sculptor&                Identity();
    Sculptor&                Translate(float x, float y, float z);
    Sculptor&                Translate(const Point3D& p)                               { return Translate(p.x, p.y, p.z); }
    Sculptor&                Rotate(float rx, float ry, float rz);
    Sculptor&                Rotate(const Point3D& p)                                  { return Rotate(p.x, p.y, p.z); }
    Sculptor&                Scale(float sx, float sy, float sz);
    Sculptor&                Scale(const Point3D& p)                                   { return Scale(p.x, p.y, p.z); }
    Sculptor&                Scale(float scale)                                        { return Scale(scale, scale, scale); }
                             
    void                     SetLight(const Point3D& dir, const RGBA& color);
    Sculptor&                AddLight(const Point3D& dir, const RGBA& color);
    void                     ClearLights();

    void                     SetNDCConverter(Function<Pointf(const Point3D&)> fn);
    
    void                     SetTextureSampler(Function<RGBA(const Fragment&)> fn);
    void                     SetPixelShader(Function<RGBA(const Fragment&, RGBA base)> fn);
    Sculptor&                AddPostFilter(Event<ImageBuffer&, Buffer<float>&, Size> fn);
    void                     ClearPostFilters();
                           
    const Vector<Primitive>& GetPrimitives() const;

    void                     Render(const Model3D& model);
    void                     ClearScene();

    void                     Rasterize();
    
    Image                    GetImage();
    operator                 Image()                                                   { return colorbuffer; }
    
    void                     Clear();

	// For convenience
	class Scope : NoCopy {
		Sculptor& scl;
	public:
		Scope(Sculptor& s) : scl(s)                                                    { scl.PushMatrix(); }
		~Scope()                                                                       { scl.PopMatrix();  }
	};

   
private:
    struct LightSource : Moveable<LightSource> {
        Point3D dir;
        RGBA    color;
    };

    Size                     size;
    Rect                     scissor;
    ImageBuffer              colorbuffer;
    Buffer<float>            zbuffer;
    Vector<Matrix4D>         matrixstack;
    Matrix4D                 projection;
    Matrix4D                 view;
    Vector<Primitive>        primitives;
    Function<Pointf(const Point3D&)> toscreen;
    Vector<LightSource>      lights;
    RGBA                     ambientlight;

    Function<RGBA(const Fragment&)>  texturesampler;
    Function<RGBA(const Fragment&, RGBA)> pixelshader;
    Vector<Event<ImageBuffer&, Buffer<float>&, Size>> postfilters;

    bool                     parallel:1;
    bool                     culling:1;
    bool                     wireframe:1;
    bool                     shading:1;
    bool                     alphablend:1;
    bool                     bilinear:1;
    bool                     mipmap:1;
                             
    void                     RasterizeTriangle(const Primitive& p, Size size, int miny, int maxy);
    void                     RasterizeLine(Size size, const Pointf& p0, float z0, const Pointf& p1, float z1, RGBA c);
};

bool LoadModel(Model3D& model, const String& data);
bool LoadModelFromFile(Model3D& model, const String& path);
}

#endif
class GLScene {
public:
    GLScene(GLPipeline& pipeline);

    GLScene&      Cull(bool b = true);
    bool          IsCulling() const;
                            
    GLScene&      Wireframe(bool b = true);
    bool          IsWireFrame() const;
                            
    GLScene&      Shading(bool b = true);
    bool          IsShading() const;
                            
    GLScene&      AlphaBlend(bool b = true);
    bool          IsAlphaBlending() const;
                            
    GLScene&      Bilinear(bool b = true);
    bool          IsBilinear() const;
                            
    GLScene&      MipMap(bool b = true);
    bool          IsMipMapping() const;
                            
    void          Perspective(float fov, float aspectratio, float znear, float zfar);
    void          Orthographic(const Rectf& view, float znear, float zfar);
    void          Frustum(const Rectf& view, float fnear, float ffar);
    void          Isometric(const Rectf& view, float fnear, float ffar);
    void          Isometric(float zoom, float aspectratio, float fnear, float ffar);
    
    void          LookAt(const Point3D& eye, const Point3D& center, const Point3D& up);
                            
    void          PushMatrix();
    void          PopMatrix();
    GLScene&      Identity();
    GLScene&      Translate(float x, float y, float z);
    GLScene&      Rotate(float rx, float ry, float rz);
    GLScene&      Scale(float sx, float sy, float sz);
    GLScene&      Scale(float scale);
                            
    void          SetLight(Point3D dir, const RGBA& color);
    GLScene&      AddLight(Point3D dir, const RGBA& color);
    void          ClearLights();
                            
    GLScene&      AddOverlay(Point pos, const Image& img);
	GLScene&      AddOverlay(int x, int y, const Image& img);
    void          ClearOverlays();
                            
    void          Uniform(const String& name, float v);
    void          Uniform(const String& name, const Point3D& v);
    void          Uniform(const String& name, const RGBA& v);
                            
    void          UseShader(const String& name);
                            
    void          ClearBuffer(const RGBA& paper = RGBAZero());
    void          Render(ModelGL& model);
                            
    Image         GetImage(Size sz);
                            
    void          Clear();
    void          Reset();
 
private:
    GLPipeline& glpipeline;
};
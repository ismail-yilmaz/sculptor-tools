class GLScene {
public:
	
	enum class Overlay {
		TopLeft,
		TopCenter,
		TopRight,
		Center,
		BottomLeft,
		BottomCenter,
		BottomRight,
	};
	
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
    GLScene&      Translate(const Point3D& p);
    GLScene&      Rotate(float rx, float ry, float rz);
    GLScene&      Rotate(const Point3D& p);
    GLScene&      Rotate(const Quaternion& q);
    GLScene&      Scale(float sx, float sy, float sz);
    GLScene&      Scale(const Point3D& p);
    GLScene&      Scale(float scale);
                            
    void          SetLight(const Point3D& dir, const RGBA& color);
    GLScene&      AddLight(const Point3D& dir, const RGBA& color);
    void          ClearLights();

    GLScene&      AddOverlay(const Point& pos, const Image& img, Overlay align = Overlay::TopLeft, float z = -1.0f);
    GLScene&      AddOverlay(int x, int y, const Image& img, Overlay align = Overlay::TopLeft, float z = -1.0f);
    GLScene&      AddOverlay(const Point3D& pos, Size viewsize, const Image& img, Overlay align = Overlay::Center);
    GLScene&      AddOverlay(float x, float y, float z, Size viewsize, const Image& img, Overlay align = Overlay::Center);
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

    // For convenience
    class Scope : NoCopy {
    public:
        Scope(GLScene& g) : gls(g) { gls.PushMatrix(); }
        ~Scope()                   { gls.PopMatrix();  }
        
    private:
        GLScene& gls;
    };
    
private:
    GLPipeline& glpipeline;
};
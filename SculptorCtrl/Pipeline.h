struct GLPipeline : Moveable<GLPipeline> {
    struct LightSource : Moveable<LightSource> {
        Point3D dir;
        RGBA    color;
    };
    
    struct OverlayItem : Moveable<OverlayItem> {
        Image img;
        Point pos;
    };

    Vector<LightSource>               lights;
    Vector<OverlayItem>               overlays;
    static VectorMap<int64, GLuint>   texturecache;
                               
    MatrixGL                          projection;
    MatrixGL                          modelview;
    Vector<MatrixGL>                  modelviewstack;
                               
    static GLuint                     defprogram;
    static VectorMap<String, GLuint>  userprograms;
    static GLuint                     postprogram;
    GLuint                            curprogram;
    GLuint                            fboid;
    GLuint                            fbotex;
    GLuint                            fbodepth;
    Size                              fbosize;
    GLuint                            quadvao;
    GLuint                            quadvbo;
                                      
    VectorMap<String, float>          floats;
    VectorMap<String, Point3D>        vec3s;
    VectorMap<String, RGBA>           rgbas;
                                      
    bool                              culling:1;
    bool                              wireframe:1;
    bool                              shading:1;
    bool                              alphablend:1;
    bool                              bilinear:1;
    bool                              mipmap:1;
                                      
    void                              Init();
    void                              Begin(Size viewsize);
    void                              End(Size viewsize);
    void                              Clear();
    void                              Reset();
    GLuint                            CompileProg(const String& vsh, const String& fsh);

    GLPipeline();
    ~GLPipeline();
};
struct GLPipeline : Moveable<GLPipeline> {
    struct LightSource : Moveable<LightSource> {
        Point3D dir;
        RGBA    color;
    };
    
    struct OverlayItem : Moveable<OverlayItem> {
        Image img;
        Point pos;
        float z;
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
    static bool                       defaultshadercache;
    static String                     defaultshadercachedir;

    int                               shadercache;
    String                            shadercachedir;
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
    bool                              IsShaderCacheEnabled() const;
    static void                       ClearDefaultShaderCache();
    void                              ClearShaderCache() const;
    GLuint                            CompileProg(const String& vsh, const String& fsh);
    GLuint                            CompileProg(const String& vsh, const String& fsh, bool cache);

    GLPipeline();
    ~GLPipeline();
};
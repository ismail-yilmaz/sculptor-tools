struct ModelGL {
public:
    struct Vertex : Moveable<Vertex> {
        Point3D position;
        Point3D normal;
        Pointf  texcoord;
        RGBA    color;
        int64   textureid;

        Vertex();
    };

    struct Triangle : Moveable<Triangle> {
        int a;
        int b;
        int c;

        Triangle();
        Triangle(int pa, int pb, int pc);
    };

    struct Batch : Moveable<Batch> {
        int64 textureid;
        int   offset;
        int   count;
    };

    Vector<Vertex>           vertices;
    Vector<Triangle>         triangles;
    VectorMap<int64, Image>  textures;
    RGBA                     defaultcolor;

    bool                     compiled;
    VectorMap<int64, GLuint> vaos;
    GLuint                   vertexbuffer;
    GLuint                   indexbuffer;
    Vector<Batch>            batches;

    void                     Clear();
    void                     SetDefaultColor(const RGBA& c)               { defaultcolor = c; }
    void                     Normalize(float target);
    ModelGL&                 AddVertex(const Vertex& v);
    ModelGL&                 AddTriangle(int a, int b, int c);
    int64                    AddTexture(const Image& img);
    ModelGL&                 AddPlane(const Rectf& r, int64 tid = 0);
    ModelGL&                 AddBox(const Box3D& box, int64 tid = 0);
    ModelGL&                 AddSphere(float radius, int slices, int stacks, int64 tid = 0);
    Box3D                    ComputeBounds() const;
    Point3D                  GetCenter() const                            { return ComputeBounds().Center(); }
                             
    void                     Compile();
    GLuint                   GetVAO();

    ModelGL();
    ~ModelGL();
};

bool LoadModel(ModelGL& model, const String& data);
bool LoadModelFromFile(ModelGL& model, const String& path);
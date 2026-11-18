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

    struct Bvh3 : Moveable<Bvh3> {
        struct Node : Moveable<Node> {
            Box3D bounds;
            int   left;
            int   right;
            int   first;
            int   count;
        };

        Vector<Node> nodes;
        Vector<int>  indices;
        int          vertexcount = 0;
        int          trianglecount = 0;
        bool         dirty = true;

        void Clear();
        void Invalidate()    { dirty = true; }
        bool IsEmpty() const { return nodes.IsEmpty(); }
        void Build(const ModelGL& model);
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
    mutable Bvh3             bvh;

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

    void                     InvalidateBvh()                              { bvh.Invalidate(); }
    void                     SyncBvh() const;

    Ray3D::Hit               Intersect(const Ray3D& ray, bool backface = false) const;
    Ray3D::Hit               Intersect(const Ray3D& ray, const MatrixGL& transform, bool backface = false) const;
    bool                     HitTest(const Ray3D& ray, bool backface = false) const;
    bool                     HitTest(const Ray3D& ray, const MatrixGL& transform, bool backface = false) const;
                             
    void                     Compile();
    GLuint                   GetVAO();

    ModelGL();
    ~ModelGL();
};

bool LoadModel(ModelGL& model, const String& data);
bool LoadModelFromFile(ModelGL& model, const String& path);
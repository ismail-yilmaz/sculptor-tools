#include "SculptorCtrl.h"

#define LLOG(x) // RLOG("ModelGL: " << x)

namespace Upp {

namespace {

struct GlVertex {
	float px, py, pz;
	float nx, ny, nz;
	float tx, ty;
	byte  r, g, b, a;
};

struct SortTri {
	int   a;
	int   b;
	int   c;
	int64 tex;
	bool operator<(const SortTri& o) const { return tex < o.tex; }
};

}

ModelGL::Vertex::Vertex()
: color(White())
, textureid(0)
{
}

ModelGL::Triangle::Triangle()
: a(0)
, b(0)
, c(0)
{
}

ModelGL::Triangle::Triangle(int pa, int pb, int pc)
: a(pa)
, b(pb)
, c(pc)
{
}

ModelGL::ModelGL()
: defaultcolor(White())
, compiled(false)
, vertexbuffer(0)
, indexbuffer(0)
{
}

ModelGL::~ModelGL()
{
	Clear();
}

void ModelGL::Clear()
{
	for(GLuint vao : vaos) {
		if(vao)
			glDeleteVertexArrays(1, &vao);
	}

	if(vertexbuffer)
		glDeleteBuffers(1, &vertexbuffer);

	if(indexbuffer)
		glDeleteBuffers(1, &indexbuffer);

	vertexbuffer = 0;
	indexbuffer = 0;

	vaos.Clear();
	batches.Clear();
	vertices.Clear();
	triangles.Clear();
	textures.Clear();
	compiled = false;
}

ModelGL& ModelGL::AddVertex(const Vertex& v)
{
	vertices.Add(v);
	compiled = false;
	return *this;
}

ModelGL& ModelGL::AddTriangle(int a, int b, int c)
{
	triangles.Add(Triangle(a, b, c));
	compiled = false;
	return *this;
}

int64 ModelGL::AddTexture(const Image& img)
{
	int64 id = img.GetSerialId();
	textures.GetAdd(id) = img;
	compiled = false;
	return id;
}

ModelGL& ModelGL::AddBox(const Box3D& box, int64 tid)
{
	const Point3D p[8] = {
		{ box.lo.x, box.lo.y, box.lo.z }, { box.hi.x, box.lo.y, box.lo.z },
		{ box.hi.x, box.hi.y, box.lo.z }, { box.lo.x, box.hi.y, box.lo.z },
		{ box.lo.x, box.lo.y, box.hi.z }, { box.hi.x, box.lo.y, box.hi.z },
		{ box.hi.x, box.hi.y, box.hi.z }, { box.lo.x, box.hi.y, box.hi.z }
	};

	const int f[6][4] = {
		{ 0, 3, 2, 1 }, { 1, 2, 6, 5 }, { 5, 6, 7, 4 },
		{ 4, 7, 3, 0 }, { 3, 7, 6, 2 }, { 4, 0, 1, 5 }
	};

	const Pointf uv[4] = { {0, 1}, {1, 1}, {1, 0}, {0, 0} };

	for(int i = 0; i < 6; i++) {
		Point3D n = Normal(p[f[i][0]], p[f[i][1]], p[f[i][2]]);
		int s = vertices.GetCount();

		for(int j = 0; j < 4; j++) {
			Vertex v;
			v.position = p[f[i][j]];
			v.normal = n;
			v.color = defaultcolor;
			v.texcoord = uv[j];
			v.textureid = tid;
			AddVertex(v);
		}

		AddTriangle(s, s + 2, s + 1);
		AddTriangle(s, s + 3, s + 2);
	}
	return *this;
}

ModelGL& ModelGL::AddSphere(float radius, int slices, int stacks, int64 tid)
{
    int v0 = vertices.GetCount();
    
    for(int i = 0; i <= stacks; i++) {
        float lat = (float) M_PI * i / stacks;
        float sinlat = sin(lat);
        float coslat = cos(lat);

        for(int j = 0; j <= slices; j++) {
            float lon = 2.0f * (float)M_PI * j / slices;
            float sinlon = sin(lon);
            float coslon = cos(lon);

            Point3D p(radius * sinlat * coslon, radius * coslat, radius * sinlat * sinlon);

            Vertex v;
            v.position = p;
            v.normal = p.Normalized();
            v.texcoord = Pointf((float)j / slices, (float)i / stacks);
            v.color = defaultcolor;
            v.textureid = tid;
            
            AddVertex(v);
        }
    }

    for(int i = 0; i < stacks; i++) {
        for(int j = 0; j < slices; j++) {
            int nextj = j + 1;
            
            int id0 = v0 + i * (slices + 1) + j;
            int id1 = v0 + (i + 1) * (slices + 1) + j;
            int id2 = v0 + (i + 1) * (slices + 1) + nextj;
            int id3 = v0 + i * (slices + 1) + nextj;

            AddTriangle(id0, id1, id2);
            AddTriangle(id0, id2, id3);
        }
    }
    
    return *this;
}

Box3D ModelGL::ComputeBounds() const
{
	if(vertices.IsEmpty())
		return Null;

	Box3D box;
	box.Clear();

	for(const Vertex& v : vertices)
		box.Expand(v.position);

	return box;
}

void ModelGL::Normalize(float target)
{
	if(vertices.IsEmpty())
		return;

	Box3D b = ComputeBounds();
	Point3D c = b.Center();
	Point3D d = b.hi - b.lo;

	float r = max(sqrt(d.x * d.x + d.y * d.y + d.z * d.z) * 0.5f, 0.001f);
	float s = target / r;

	for(Vertex& v : vertices) {
		v.position.x = (v.position.x - c.x) * s;
		v.position.y = (v.position.y - c.y) * s;
		v.position.z = (v.position.z - c.z) * s;
	}

	compiled = false;
}

GLuint ModelGL::GetVAO()
{
	int64 ctx = GetCurrentGLContext();
	if(!ctx)
		return 0;

	int i = vaos.Find(ctx);
	if(i >= 0)
		return vaos[i];

	if(!vertexbuffer || !indexbuffer)
		return 0;

	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);

	int st = sizeof(GlVertex);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, st, (void *) 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, st, (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, st, (void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE, st, (void *)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	vaos.Add(ctx, vao);
	return vao;
}

void ModelGL::Compile()
{
	if(compiled)
		return;

	for(int i = 0; i < vaos.GetCount(); i++) {
		GLuint vao = vaos[i];
		if(vao)
			glDeleteVertexArrays(1, &vao);
	}
	vaos.Clear();

	if(vertexbuffer)
		glDeleteBuffers(1, &vertexbuffer);

	if(indexbuffer)
		glDeleteBuffers(1, &indexbuffer);

	batches.Clear();

	for(int i = 0; i < textures.GetCount(); i++) {
		int64 id = textures.GetKey(i);
		const Image& img = textures[i];

		if(img.IsEmpty())
			continue; // Prevent OpenGL crash on empty images

		if(GLPipeline::texturecache.Find(id) < 0) {
			GLuint tid;
			glGenTextures(1, &tid);
			glBindTexture(GL_TEXTURE_2D, tid);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.GetWidth(), img.GetHeight(), 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, ~img);
			glGenerateMipmap(GL_TEXTURE_2D);
			GLPipeline::texturecache.Add(id, tid);
		}
	}

	Vector<GlVertex> vd;
	vd.Reserve(vertices.GetCount());

	for(const Vertex& v : vertices) {
		GlVertex& d = vd.Add();
		d.px = v.position.x;
		d.py = v.position.y;
		d.pz = v.position.z;
		d.nx = v.normal.x;
		d.ny = v.normal.y;
		d.nz = v.normal.z;
		d.tx = v.texcoord.x;
		d.ty = v.texcoord.y;
		d.r = v.color.r;
		d.g = v.color.g;
		d.b = v.color.b;
		d.a = v.color.a;
	}

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vd.GetCount() * sizeof(GlVertex), vd.Begin(), GL_STATIC_DRAW);

	Vector<SortTri> tri;
	tri.Reserve(triangles.GetCount());

	for(const Triangle& t : triangles) {
		SortTri& s = tri.Add();
		s.a = t.a;
		s.b = t.b;
		s.c = t.c;
		s.tex = vertices[s.a].textureid;
	}

	Sort(tri);

	Vector<int> id;
	id.Reserve(tri.GetCount() * 3);

	int64 ctex = -1;
	int bid = -1; // Safely track the index instead of a dangling pointer

	for(const SortTri& tr : tri) {
		if(tr.tex != ctex) {
			ctex = tr.tex;
			batches.Add();
			bid = batches.GetCount() - 1;
			batches[bid].textureid = ctex;
			batches[bid].offset = id.GetCount();
			batches[bid].count = 0;
		}

		id.Add(tr.a);
		id.Add(tr.b);
		id.Add(tr.c);

		if(bid >= 0)
			batches[bid].count += 3;
	}

	glGenBuffers(1, &indexbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, id.GetCount() * sizeof(int), id.Begin(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	compiled = true;

	GetVAO();
}

bool LoadModel(ModelGL& model, const String& data)
{
	if(data.IsEmpty())
		return false;

	model.Clear();

	Vector<Point3D> p;
	Vector<Pointf>  t;
	Vector<Point3D> n;

	CParser parser(data);
	parser.SkipSpaces();

	try {
		while(!parser.IsEof()) {
			if(parser.Id("vt")) {
				float x = (float) parser.ReadDouble();
				float y = (float) parser.ReadDouble();
				t.Add(Pointf(x, y));
			}
			else
			if(parser.Id("vn")) {
				float x = (float) parser.ReadDouble();
				float y = (float) parser.ReadDouble();
				float z = (float) parser.ReadDouble();
				n.Add(Point3D(x, y, z));
			}
			else
			if(parser.Id("v")) {
				float x = (float) parser.ReadDouble();
				float y = (float) parser.ReadDouble();
				float z = (float) parser.ReadDouble();
				p.Add(Point3D(x, y, z));
			}
			else
			if(parser.Id("f")) {
				Vector<int> fv;

				while(parser.IsInt()) {
					int vi = parser.ReadInt();
					int ti = -1;
					int ni = -1;

					if(parser.Char('/')) {
						if(parser.IsInt())
							ti = parser.ReadInt();
						if(parser.Char('/'))
							if(parser.IsInt())
								ni = parser.ReadInt();
					}

					ModelGL::Vertex v;
					v.color = model.defaultcolor;

					int i = (vi < 0) ? p.GetCount() + vi : vi - 1;
					if(i >= 0 && i < p.GetCount())
						v.position = p[i];

					if(ti != -1) {
						i = (ti < 0) ? t.GetCount() + ti : ti - 1;
						if(i >= 0 && i < t.GetCount()) {
							v.texcoord = t[i];
							v.textureid = 0;
						}
					}

					if(ni != -1) {
						i = (ni < 0) ? n.GetCount() + ni : ni - 1;
						if(i >= 0 && i < n.GetCount())
							v.normal = n[i];
					}

					int ii = model.vertices.GetCount();
					model.vertices.Add(v);
					fv.Add(ii);
				}

				if(fv.GetCount() >= 3) {
					for(int i = 1; i < fv.GetCount() - 1; i++)
						model.AddTriangle(fv[0], fv[i + 1], fv[i]);
				}
			}
			else
				parser.SkipLine();
		}
	}
	catch(const CParser::Error& e) {
		LLOG("OBJ model parsing error: " << e);
		return false;
	}

	return true;
}

bool LoadModelFromFile(ModelGL& model, const String& path)
{
	return LoadModel(model, LoadFile(path));
}

}
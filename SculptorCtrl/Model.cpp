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

inline float Axis(const Point3D& p, int ax)
{
	return ax == 0 ? p.x : (ax == 1 ? p.y : p.z);
}

float BoxArea(const Box3D& b)
{
	Point3D d = b.hi - b.lo;
	return 2.0f * (max(d.x, 0.0f) * max(d.y, 0.0f)
				+  max(d.y, 0.0f) * max(d.z, 0.0f)
				+  max(d.z, 0.0f) * max(d.x, 0.0f));
}

void ExpandBox(Box3D& dst, const Box3D& src)
{
	dst.Expand(src.lo);
	dst.Expand(src.hi);
}

int BuildBvhNode(Vector<ModelGL::Bvh3::Node>& nodes, Vector<int>& indices, const Vector<Box3D>& tb,
													const Vector<Point3D>& tc, int beg, int end, int depth)
{
	constexpr int MAXLEAF  = 4;
	constexpr int MAXDEPTH = 60;
	constexpr int BINS     = 8;

	int ni = nodes.GetCount();
	nodes.Add();

	Box3D bx;
	bx.Clear();
	for(int i = beg; i < end; i++)
		ExpandBox(bx, tb[indices[i]]);
	nodes[ni].bounds = bx;

	int cnt = end - beg;
	if(cnt <= MAXLEAF || depth >= MAXDEPTH) {
		nodes[ni].left  = -1;
		nodes[ni].right = -1;
		nodes[ni].first = beg;
		nodes[ni].count = cnt;
		return ni;
	}

	Box3D cb;
	cb.Clear();
	for(int i = beg; i < end; i++)
		cb.Expand(tc[indices[i]]);

	int   bax = -1;
	int   bbn = -1;
	float bst = FLT_MAX;

	for(int ax = 0; ax < 3; ax++) {
		float lo = Axis(cb.lo, ax);
		float hi = Axis(cb.hi, ax);
		float sp = hi - lo;
		if(sp <= 1e-6f)
			continue;

		Box3D bbx[BINS];
		int   bc[BINS];
		for(int b = 0; b < BINS; b++) {
			bbx[b].Clear();
			bc[b] = 0;
		}

		float inv = BINS / sp;
		for(int i = beg; i < end; i++) {
			int ti = indices[i];
			int b  = clamp((int)((Axis(tc[ti], ax) - lo) * inv), 0, BINS - 1);
			ExpandBox(bbx[b], tb[ti]);
			bc[b]++;
		}

		Box3D lb[BINS - 1];
		Box3D rb[BINS - 1];
		int   lc[BINS - 1];
		int   rc[BINS - 1];

		Box3D acc;
		acc.Clear();
		int sum = 0;
		for(int b = 0; b < BINS - 1; b++) {
			if(bc[b] > 0)
				ExpandBox(acc, bbx[b]);
			sum += bc[b];
			lb[b] = acc;
			lc[b] = sum;
		}

		acc.Clear();
		sum = 0;
		for(int b = BINS - 1; b > 0; b--) {
			if(bc[b] > 0)
				ExpandBox(acc, bbx[b]);
			sum += bc[b];
			rb[b - 1] = acc;
			rc[b - 1] = sum;
		}

		for(int b = 0; b < BINS - 1; b++) {
			if(lc[b] == 0 || rc[b] == 0)
				continue;
			float cst = BoxArea(lb[b]) * lc[b] + BoxArea(rb[b]) * rc[b];
			if(cst < bst) {
				bst = cst;
				bax = ax;
				bbn = b;
			}
		}
	}

	int mid = beg;
	if(bax >= 0) {
		float lo  = Axis(cb.lo, bax);
		float inv = BINS / (Axis(cb.hi, bax) - lo);
		int   i   = beg;
		int   j   = end - 1;
		while(i <= j) {
			int b = clamp((int)((Axis(tc[indices[i]], bax) - lo) * inv), 0, BINS - 1);
			if(b <= bbn)
				i++;
			else {
				Swap(indices[i], indices[j]);
				j--;
			}
		}
		mid = i;
	}

	if(mid <= beg || mid >= end)
		mid = (beg + end) / 2;

	int li = BuildBvhNode(nodes, indices, tb, tc, beg, mid, depth + 1);
	int ri = BuildBvhNode(nodes, indices, tb, tc, mid, end, depth + 1);

	nodes[ni].left  = li;
	nodes[ni].right = ri;
	nodes[ni].first = -1;
	nodes[ni].count = 0;
	return ni;
}

}

void ModelGL::Bvh3::Clear()
{
	nodes.Clear();
	indices.Clear();
	vertexcount = 0;
	trianglecount = 0;
	dirty = true;
}

void ModelGL::Bvh3::Build(const ModelGL& model)
{
	Clear();

	int vc = model.vertices.GetCount();
	int tc = model.triangles.GetCount();
	vertexcount = vc;
	trianglecount = tc;
	dirty = false;

	if(vc == 0 || tc == 0)
		return;

	Vector<Box3D>   tb;
	Vector<Point3D> tpc;
	tb.SetCount(tc);
	tpc.SetCount(tc);
	indices.Reserve(tc);

	for(int i = 0; i < tc; i++) {
		const ModelGL::Triangle& t = model.triangles[i];
		if(t.a < 0 || t.a >= vc || t.b < 0 || t.b >= vc || t.c < 0 || t.c >= vc)
			continue;

		const Point3D& p0 = model.vertices[t.a].position;
		const Point3D& p1 = model.vertices[t.b].position;
		const Point3D& p2 = model.vertices[t.c].position;

		Box3D bx;
		bx.Clear();
		bx.Expand(p0);
		bx.Expand(p1);
		bx.Expand(p2);

		tb[i]  = bx;
		tpc[i] = bx.Center();
		indices.Add(i);
	}

	if(indices.IsEmpty())
		return;

	nodes.Reserve(indices.GetCount() * 2);
	BuildBvhNode(nodes, indices, tb, tpc, 0, indices.GetCount(), 0);
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
	bvh.Clear();
	compiled = false;
}

ModelGL& ModelGL::AddVertex(const Vertex& v)
{
	vertices.Add(v);
	bvh.Invalidate();
	compiled = false;
	return *this;
}

ModelGL& ModelGL::AddTriangle(int a, int b, int c)
{
	triangles.Add(Triangle(a, b, c));
	bvh.Invalidate();
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

ModelGL& ModelGL::AddPlane(const Rectf& r, int64 tid)
{
	int i = vertices.GetCount();

	auto addv = [&](float x, float y, float u, float v) {
		Vertex &vert = vertices.Add();
		vert.position = Point3D(x, y, 0.0f);
		vert.normal   = Point3D(0.0f, 0.0f, 1.0f);
		vert.texcoord = Pointf(u, v);
		vert.color    = defaultcolor;
		vert.textureid = tid;
	};

	addv(r.left,  r.bottom, 0.0f, 0.0f);
	addv(r.right, r.bottom, 1.0f, 0.0f);
	addv(r.right, r.top,    1.0f, 1.0f);
	addv(r.left,  r.top,    0.0f, 1.0f);

	AddTriangle(i, i + 1, i + 2);
	AddTriangle(i, i + 2, i + 3);

	return *this;
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
			float lon = 2.0f * (float) M_PI * j / slices;
			float sinlon = sin(lon);
			float coslon = cos(lon);

			Point3D p(radius * sinlat * coslon, radius * coslat, radius * sinlat * sinlon);

			Vertex v;
			v.position = p;
			v.normal = p.Normalized();
			v.texcoord = Pointf((float) j / slices, (float) i / stacks);
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

void ModelGL::SyncBvh() const
{
	if(!bvh.dirty && bvh.vertexcount == vertices.GetCount() && bvh.trianglecount == triangles.GetCount())
		return;

	bvh.Build(*this);
}

Ray3D::Hit ModelGL::Intersect(const Ray3D& ray, bool backface) const
{
	Ray3D::Hit best;
	int vc = vertices.GetCount();
	if(IsNull(ray) || vc == 0 || triangles.IsEmpty())
		return best;

	SyncBvh();
	if(bvh.IsEmpty())
		return best;

	int stk[64];
	int sp = 0;
	stk[sp++] = 0;

	while(sp > 0) {
		const Bvh3::Node& nd = bvh.nodes[stk[--sp]];
		if(!ray.Intersects(nd.bounds, best.distance))
			continue;

		if(nd.count > 0) {
			for(int i = 0; i < nd.count; i++) {
				int ti = bvh.indices[nd.first + i];
				const Triangle& t = triangles[ti];

				Ray3D::Hit h = ray.IntersectTriangle(
					vertices[t.a].position,
					vertices[t.b].position,
					vertices[t.c].position,
					backface,
					best.distance
				);
				if(h < best) {
					best = h;
					best.triangle = ti;
					Point3D n = vertices[t.a].normal * h.barycentric.x
							+ vertices[t.b].normal * h.barycentric.y
							+ vertices[t.c].normal * h.barycentric.z;
					if(n.Squared() > 1e-8f)
						best.normal = n.Normalized();
				}
			}
		}
		else
		if(nd.left >= 0 && nd.right >= 0) {
			float d1 = (bvh.nodes[nd.left].bounds.Center() - ray.origin).Squared();
			float d2 = (bvh.nodes[nd.right].bounds.Center() - ray.origin).Squared();
			if(d1 < d2) {
				if(sp < 64) stk[sp++] = nd.right;
				if(sp < 64) stk[sp++] = nd.left;
			}
			else {
				if(sp < 64) stk[sp++] = nd.left;
				if(sp < 64) stk[sp++] = nd.right;
			}
		}
		else
		if(nd.left >= 0) {
			if(sp < 64) stk[sp++] = nd.left;
		}
		else
		if(nd.right >= 0) {
			if(sp < 64) stk[sp++] = nd.right;
		}
	}

	return best;
}

Ray3D::Hit ModelGL::Intersect(const Ray3D& ray, const MatrixGL& transform, bool backface) const
{
	MatrixGL inv = transform.Inverse();
	if(IsNull(inv))
		return Null;

	Ray3D lray = ray * inv;
	if(IsNull(lray))
		return Null;

	return Intersect(lray, backface).Transformed(transform, ray.origin);
}

bool ModelGL::HitTest(const Ray3D& ray, bool backface) const
{
	int vc = vertices.GetCount();
	if(IsNull(ray) || vc == 0 || triangles.IsEmpty())
		return false;

	SyncBvh();
	if(bvh.IsEmpty())
		return false;

	int stk[64];
	int sp = 0;
	stk[sp++] = 0;

	while(sp > 0) {
		const Bvh3::Node& nd = bvh.nodes[stk[--sp]];
		if(!ray.Intersects(nd.bounds))
			continue;

		if(nd.count > 0) {
			for(int i = 0; i < nd.count; i++) {
				const Triangle& t = triangles[bvh.indices[nd.first + i]];
				if(ray.IntersectsTriangle(vertices[t.a].position, vertices[t.b].position, vertices[t.c].position, backface))
					return true;
			}
		}
		else {
			if(nd.right >= 0 && sp < 64)
				stk[sp++] = nd.right;
			if(nd.left >= 0 && sp < 64)
				stk[sp++] = nd.left;
		}
	}

	return false;
}

bool ModelGL::HitTest(const Ray3D& ray, const MatrixGL& transform, bool backface) const
{
	MatrixGL inv = transform.Inverse();
	if(IsNull(inv))
		return false;

	Ray3D lray = ray * inv;
	if(IsNull(lray))
		return false;

	return HitTest(lray, backface);
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

	bvh.Invalidate();
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
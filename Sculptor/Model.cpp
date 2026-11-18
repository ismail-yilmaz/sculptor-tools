#include "Sculptor.h"

#define LLOG(x) // RLOG("Model3D: " << x)

namespace Upp {

namespace {

inline float Axis(const Point3D& p, int ax)
{
	return ax == 0 ? p.x : (ax == 1 ? p.y : p.z);
}

float BoxArea(const Box3D& b)
{
	Point3D d = b.hi - b.lo;
	return 2.0f * (max(d.x, 0.0f) * max(d.y, 0.0f)
	             + max(d.y, 0.0f) * max(d.z, 0.0f)
	             + max(d.z, 0.0f) * max(d.x, 0.0f));
}

void ExpandBox(Box3D& dst, const Box3D& src)
{
	dst.Expand(src.lo);
	dst.Expand(src.hi);
}

int BuildBvhNode(Vector<Model3D::Bvh3::Node>& nodes, Vector<int>& indices,
                 const Vector<Box3D>& tb, const Vector<Point3D>& tc,
                 int beg, int end, int depth)
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

void Model3D::Bvh3::Clear()
{
	nodes.Clear();
	indices.Clear();
	vertexcount = 0;
	trianglecount = 0;
	dirty = true;
}

void Model3D::Bvh3::Build(const Model3D& model)
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
		const Model3D::Triangle& t = model.triangles[i];
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

Model3D::Vertex::Vertex()
: color(White())
, textureid(0)
{
}

Model3D::Triangle::Triangle()
: a(0)
, b(0)
, c(0)
{
}

Model3D::Triangle::Triangle(int pa, int pb, int pc)
: a(pa)
, b(pb)
, c(pc)
{
}

Model3D::Model3D()
: defaultcolor(White())
{
}

void Model3D::Clear()
{
	vertices.Clear();
	triangles.Clear();
	textures.Clear();
	mipmaps.Clear();
	bvh.Clear();
}

Model3D& Model3D::AddVertex(const Vertex& v)
{
	vertices.Add(v);
	bvh.Invalidate();
	return *this;
}

Model3D& Model3D::AddTriangle(int a, int b, int c)
{
	triangles.Add(Triangle(a, b, c));
	bvh.Invalidate();
	return *this;
}

Model3D& Model3D::AddPlane(const Rectf& r, int64 tid)
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

Model3D& Model3D::AddBox(const Box3D& box, int64 tid)
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
		AddTriangle(s, s + 1, s + 2);
		AddTriangle(s, s + 2, s + 3);
	}

	return *this;
}

Model3D& Model3D::AddSphere(float radius, int slices, int stacks, int64 tid)
{
	int s0 = vertices.GetCount();

	for(int i = 0; i <= stacks; i++) {
		float v = (float)i / stacks;
		float phi = v * M_PI;
		for(int j = 0; j <= slices; j++) {
			float u = (float)j / slices;
			float theta = u * 2.0 * M_PI;

			float x = cos(theta) * sin(phi);
			float y = cos(phi);
			float z = sin(theta) * sin(phi);

			Vertex vx;
			vx.position = Point3D(x * radius, y * radius, z * radius);
			vx.normal = Point3D(x, y, z);
			vx.color = defaultcolor;
			vx.texcoord = Pointf(u, v);
			vx.textureid = tid;
			AddVertex(vx);
		}
	}

	for(int i = 0; i < stacks; i++) {
		for(int j = 0; j < slices; j++) {
			int f0 = s0 + i * (slices + 1) + j;
			int f1 = s0 + (i + 1) * (slices + 1) + j;
			int f2 = s0 + (i + 1) * (slices + 1) + (j + 1);
			int f3 = s0 + i * (slices + 1) + (j + 1);

			AddTriangle(f0, f1, f2);
			AddTriangle(f0, f2, f3);
		}
	}
	return *this;
}

int64 Model3D::AddTexture(const Image& img)
{
	if(IsNull(img))
		return 0;

	int64 id = img.GetSerialId();
	textures.GetAdd(id) = img;

	Vector<Image>& mips = mipmaps.GetAdd(id);
	mips.Trim(0);
	mips.Add(img);

	Image cur = img;
	while(cur.GetWidth() > 1 || cur.GetHeight() > 1) {
		Size sz = cur.GetSize();
		Size nsz(max(1, sz.cx / 2), max(1, sz.cy / 2));
		ImageBuffer ib(nsz);

		for(int y = 0; y < nsz.cy; y++) {
			for(int x = 0; x < nsz.cx; x++) {
				int sx = x * 2;
				int sy = y * 2;

				RGBA c00 = cur[sy][sx];
				RGBA c10 = sx + 1 < sz.cx ? cur[sy][sx + 1] : c00;
				RGBA c01 = sy + 1 < sz.cy ? cur[sy + 1][sx] : c00;
				RGBA c11 = (sx + 1 < sz.cx && sy + 1 < sz.cy) ? cur[sy + 1][sx + 1] : c00;

				ib[y][x].r = (c00.r + c10.r + c01.r + c11.r) / 4;
				ib[y][x].g = (c00.g + c10.g + c01.g + c11.g) / 4;
				ib[y][x].b = (c00.b + c10.b + c01.b + c11.b) / 4;
				ib[y][x].a = (c00.a + c10.a + c01.a + c11.a) / 4;
			}
		}
		cur = ib;
		mips.Add(cur);
	}
	
	return id;
}

Box3D Model3D::ComputeBounds() const
{
	Box3D bounds;
	if(vertices.GetCount() > 0) {
		bounds.lo = vertices[0].position;
		bounds.hi = vertices[0].position;
		for(int i = 1; i < vertices.GetCount(); i++)
			bounds.Expand(vertices[i].position);
	}
	else
		bounds.Clear();

	return bounds;
}

Point3D Model3D::GetCenter() const
{
	return ComputeBounds().Center();
}

void Model3D::Normalize(float target)
{
	Box3D b = ComputeBounds();
	Point3D c = b.Center();
	Point3D d = b.hi - b.lo;

	float r = max(sqrt(d.x * d.x + d.y * d.y + d.z * d.z) * 0.5, 0.001);
	float s = target / r;

	for(Vertex& v : vertices) {
		v.position.x = (v.position.x - c.x) * s;
		v.position.y = (v.position.y - c.y) * s;
		v.position.z = (v.position.z - c.z) * s;
	}

	bvh.Invalidate();
}

void Model3D::SyncBvh() const
{
	if(!bvh.dirty && bvh.vertexcount == vertices.GetCount() && bvh.trianglecount == triangles.GetCount())
		return;

	bvh.Build(*this);
}

Ray3D::Hit Model3D::Intersect(const Ray3D& ray, bool backface) const
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

Ray3D::Hit Model3D::Intersect(const Ray3D& ray, const Matrix4D& transform, bool backface) const
{
	if(IsNull(ray) || IsNull(transform))
		return Null;

	Matrix4D inv = transform.Inverse();
	if(IsNull(inv))
		return Null;

	Ray3D lray = ray * inv;
	if(IsNull(lray))
		return Null;

	return Intersect(lray, backface).Transformed(transform, ray.origin);
}

bool Model3D::HitTest(const Ray3D& ray, bool backface) const
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
				if(ray.IntersectsTriangle(
					vertices[t.a].position,
					vertices[t.b].position,
					vertices[t.c].position,
					backface
					))
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

bool Model3D::HitTest(const Ray3D& ray, const Matrix4D& transform, bool backface) const
{
	if(IsNull(ray) || IsNull(transform))
		return false;

	Matrix4D inv = transform.Inverse();
	if(IsNull(inv))
		return false;

	Ray3D lray = ray * inv;
	if(IsNull(lray))
		return false;

	return HitTest(lray, backface);
}

bool LoadModel(Model3D& model, const String& data)
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
				float x = parser.ReadDouble();
				float y = parser.ReadDouble();
				t.Add(Pointf(x, y));
			}
			else
			if(parser.Id("vn")) {
				float x = parser.ReadDouble();
				float y = parser.ReadDouble();
				float z = parser.ReadDouble();
				n.Add(Point3D(x, y, z));
			}
			else
			if(parser.Id("v")) {
				float x = parser.ReadDouble();
				float y = parser.ReadDouble();
				float z = parser.ReadDouble();
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
					Model3D::Vertex v;
					v.color = model.defaultcolor;
					int i = (vi < 0) ? p.GetCount() + vi : vi - 1;
					if(i >= 0 && i < p.GetCount()){
						v.position = p[i];
					}
					if(ti != -1) {
						i = (ti < 0) ? t.GetCount() + ti : ti - 1;
						if(i >= 0 && i < t.GetCount()) {
							v.texcoord = t[i];
							v.textureid = 0;
						}
					}
					if(ni != -1) {
						i = (ni < 0) ? n.GetCount() + ni : ni - 1;
						if(i >= 0 && i < n.GetCount()) {
							v.normal = n[i];
						}
					}
					int ii = model.vertices.GetCount();
					model.vertices.Add(v);
					fv.Add(ii);
				}
				if(fv.GetCount() >= 3) {
					for(int i = 1; i < fv.GetCount() - 1; i++)
						model.AddTriangle(fv[0], fv[i], fv[i + 1]);
				}
			}
			else
				parser.SkipLine();
		}
	}
	catch(const CParser::Error& e) {
		RLOG("OBJ Parse Error: " << e);
		return false;
	}

	return true;
}

bool LoadModelFromFile(Model3D& model, const String& path)
{
	return LoadModel(model, LoadFile(path));
}

}
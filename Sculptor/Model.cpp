#include "Sculptor.h"

#define LLOG(x) // RLOG("Model3D: " << x)

namespace Upp {

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
}

Model3D& Model3D::AddVertex(const Vertex& v)
{
	vertices.Add(v);
	return *this;
}

Model3D& Model3D::AddTriangle(int a, int b, int c)
{
	triangles.Add(Triangle(a, b, c));
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
#include <CtrlLib/CtrlLib.h>
#include <Sculptor/Sculptor.h>

// A tribute to the legendary 1984 Amiga Boing Ball demo originally created by Dale Luck and R.J. Mical.

using namespace Upp;

extern Image CreateBoingTexture(), CreateGridTexture();

void CreateScene(Model3D& room, Model3D& ball)
{
	int64 balltid = ball.AddTexture(CreateBoingTexture());
	ball.AddSphere(2.0f, 32, 32, balltid);
	
	int64 roomtid = room.AddTexture(CreateGridTexture());
	auto AddPlane = [&](Point3D p0, Point3D p1, Point3D p2, Point3D p3, float u0, float v0, float u1, float v1, Point3D n) {
		int i = room.vertices.GetCount();
		auto addv = [&](const Point3D& p, float u, float v) {
			Model3D::Vertex vert;
			vert.position = p;
			vert.normal = n;
			vert.texcoord = Pointf(u, v);
			vert.color = White();
			vert.textureid = roomtid;
			room.AddVertex(vert);
		};

		addv(p0, u0, v0);
		addv(p1, u1, v0);
		addv(p2, u1, v1);
		addv(p3, u0, v1);

		room.AddTriangle(i, i + 1, i + 2);
		room.AddTriangle(i, i + 2, i + 3);
	};

	// Floor
	AddPlane(
		Point3D(-20, -4.5f,  10.0f), Point3D( 20, -4.5f,  10.0f),
		Point3D( 20, -4.5f, -5.0f),  Point3D(-20, -4.5f, -5.0f),
		0.0f, 0.0f, 20.0f, 7.5f, Point3D(0, 1, 0)
	);

	// Wall
	AddPlane(
		Point3D(-20, -4.5f, -5.0f), Point3D( 20, -4.5f, -5.0f),
		Point3D( 20,  15.5f, -5.0f), Point3D(-20,  15.5f, -5.0f),
		0.0f, 7.5f, 20.0f, 17.5f, Point3D(0, 0, 1)
	);
}

struct BoingDemo : TopWindow {
	Model3D  boing, room;
	TimeStop timer;

	BoingDemo()
	{
		Title("Boing Ball");
		Sizeable().Zoomable().CenterScreen().SetRect(0, 0, 800, 600);

		CreateScene(room, boing);
		SetTimeCallback(-15, [=] { Refresh(); });
	}

	void Paint(Draw& w) override
	{
		Sizef sz = GetSize();
		Sculptor renderer(sz, Black());
		renderer.Perspective(M_PI / 3.0f, sz.cx / sz.cy, 0.1f, 100.0f);
		renderer.LookAt(Point3D(0, 0, 10), Point3D(0, 0, 0), Point3D(0, 1, 0));
		renderer.AddLight(Point3D(-1.0f, 1.2f, -1.5f), White());
		RenderRoom(renderer);
		RenderBall(renderer);
		renderer.Rasterize();
		w.DrawImage(0, 0, renderer.GetImage());
	}
	
	void RenderRoom(Sculptor& sc)
	{
		Sculptor::Scope __(sc);
		sc.Shading(false).Render(room);
	}
	
	void RenderBall(Sculptor& sc)
	{
		float t = (float) timer.Seconds();
		float bx = 0.0f;
		float by = abs(sin(t * 3.0f)) * 4.5f - 2.5f;
		float bz = 0.0f;
		float spiny = t * -3.0f;
		float tiltx = 0.35f;

		Sculptor::Scope __(sc);
		sc.Translate(bx, by, bz);
		sc.Rotate(tiltx, spiny, 0);
		sc.Shading().Render(boing);
	}
};

GUI_APP_MAIN
{
	BoingDemo().Run();
}
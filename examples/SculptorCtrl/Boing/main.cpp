#include <CtrlLib/CtrlLib.h>
#include <SculptorCtrl/SculptorCtrl.h>

// A tribute to the legendary 1984 Amiga Boing Ball demo originally created by Dale Luck and R.J. Mical.
// See SculptorCtrl tutorial docs for step-by-step explanation.

using namespace Upp;

void CreateScene(ModelGL& room, ModelGL& ball)
{
	ball.AddSphere(2.0f, 32, 32);

	auto AddPlane = [&](Point3D p0, Point3D p1, Point3D p2, Point3D p3, float u0, float v0, float u1, float v1, Point3D n) {
		int i = room.vertices.GetCount();
		auto addv = [&](const Point3D& p, float u, float v) {
			ModelGL::Vertex vert;
			vert.position = p;
			vert.normal = n;
			vert.texcoord = Pointf(u, v);
			vert.color = White();
			vert.textureid = 0;
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

struct BoingView : public SculptorCtrl {
	ModelGL   boing, room;
	TimeStop  timer;

	BoingView()
	{
		CreateScene(room, boing);
		SetTimeCallback(-15, [this] { Refresh(); });
	}

	void InitScene() override
	{
		extern const char *grid_vshader, *grid_fshader;
		extern const char *boing_vshader, *boing_fshader;

		AddShader("grid-shader", grid_vshader, grid_fshader);
		AddShader("boing-shader", boing_vshader, boing_fshader);
	}
	
	void RenderRoom(GLScene& gls)
	{
		GLScene::Scope __(gls); // Protect the matrix stack

		gls.UseShader("grid-shader");
		gls.Cull(false).Render(room);
	}
	
	void RenderBall(GLScene& gls)
	{
		GLScene::Scope __(gls);
		
		float t = (float) timer.Seconds();

		// For the sake of simplicity, we dont simulate horizontal movement/bounce
		float bx = 0.0f;
		float by = abs(sin(t * 3.0f)) * 4.5f - 2.5f;
		float bz = 0.0f;
		float spiny = t * -3.0f;
		float tiltx = 0.35f;

		gls.Translate(bx, by, bz);
		gls.Rotate(tiltx, spiny, 0);

		gls.UseShader("boing-shader");
		gls.Cull().Render(boing);
	}
	
	void Render(GLScene& gls) override
	{
		Sizef sz = GetSize();
		gls.ClearBuffer(Black());
		gls.Perspective(M_PI / 3.0f, sz.cx / sz.cy, 0.1f, 100.0f);
		gls.LookAt(Point3D(0, 0, 10), Point3D(0, 0, 0), Point3D(0, 1, 0));
		gls.AddLight(Point3D(0.5, 1, 1), White());
		RenderRoom(gls);
		RenderBall(gls);
	}
};

GUI_APP_MAIN
{
	TopWindow w;
	w.Title("Boing Ball");
	w.Sizeable().Zoomable().CenterScreen().SetRect(0, 0, 800, 600);

	BoingView view;
	w.Add(view.SizePos());
	w.Run();
}
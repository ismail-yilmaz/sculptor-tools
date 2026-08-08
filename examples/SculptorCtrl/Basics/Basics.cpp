#include <CtrlLib/CtrlLib.h>
#include <SculptorCtrl/SculptorCtrl.h>

using namespace Upp;

struct UppBox : SculptorCtrl {
	ModelGL box;
	float angle = 0;

	UppBox()
	{
		int64 tid = box.AddTexture(StreamRaster::LoadFileAny(GetDataFile("Upp.png")));
		box.AddBox(Box3D(Point3D(-1, -1, -1), Point3D(1, 1, 1)), tid);
		SetTimeCallback(-16, [=] { angle += 0.02; Refresh(); });
	}

	void InitScene() override
	{
		// Shader and post-processor initialization should be done here.
		extern const char *vertex_shader, *fragment_shader;
		SetShader("crt-effect", vertex_shader, fragment_shader);
	}
	
	void Render(GLScene& gls) override
	{
		Sizef sz = GetSize();
		gls.ClearBuffer(Black());
		gls.Perspective(M_PI / 3.0f, sz.cx / sz.cy, 0.01f, 100.0f);
		gls.LookAt(Point3D(0, 0, 5), Point3D(0, 0, 0), Point3D(0, 1, 0));
		gls.AddLight(Point3D(-1, -1, 1), White());
		gls.AddLight(Point3D(1, 1, 1), Color(100, 100, 255));
		gls.UseShader("crt-effect");
		gls.Uniform("uTime", angle);
		gls.Rotate(angle * 0.7, angle, angle * 1.3);
		gls.Render(box);
	}
};

GUI_APP_MAIN
{
	TopWindow win;
	win.Title("SculptorCtrl Basic Example");
	win.Sizeable().Zoomable().CenterScreen().SetRect(0, 0, 600, 600);

	UppBox view;
	win.Add(view.SizePos());
	
	win.Run();
}
#include <CtrlLib/CtrlLib.h>
#include <Sculptor/Sculptor.h>

using namespace Upp;

struct UppBox : TopWindow {
	Model3D box;
	float   angle = 0;

	UppBox()
	{
		Title("Sculptor Basic Example");
		SetRect(0, 0, 600, 600);
		Sizeable().Zoomable();

		int64 tid = box.AddTexture(StreamRaster::LoadFileAny(GetDataFile("Upp.png")));
		box.AddBox(Box3D(Point3D(-1, -1, -1), Point3D(1, 1, 1)), tid);
		SetTimeCallback(-16, [=] { angle += 0.02f; Refresh(); });
	}

	void Paint(Draw& w) override
	{
		Sizef sz = GetSize();

		Sculptor renderer(sz, Black());
		renderer.Perspective(M_PI / 3.0f, sz.cx / sz.cy, 0.1, 100.0);
		renderer.LookAt(Point3D(0, 0, 5), Point3D(0, 0, 0), Point3D(0, 1, 0));
		renderer.AddLight(Point3D(-1, -1, 1), White());
		renderer.AddLight(Point3D(1, 1, 1),  { 100, 100, 255, 255 });
		renderer.Rotate(angle * 0.7f, angle, angle * 1.3f);
		renderer.Render(box);
		renderer.Rasterize();
		
		w.DrawImage(0, 0, renderer.GetImage());
	}
};

GUI_APP_MAIN
{
	UppBox().Run();
}
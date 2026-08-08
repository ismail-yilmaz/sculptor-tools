#include <Sculptor/Sculptor.h>
#include <plugin/png/png.h> // For PNG encoding.

using namespace Upp;

CONSOLE_APP_MAIN
{
	Model3D model;
	model.AddBox(Box3D(Point3D(-1, -1, -1), Point3D(1, 1, 1)),
				 model.AddTexture(StreamRaster::LoadFileAny(GetDataFile("Upp.png"))));
	
	const Sizef size(600, 600);

	Sculptor renderer(size, Black());
	renderer.Perspective(M_PI / 3.0f, size.cx / size.cy, 0.1, 100.0);
	renderer.LookAt(Point3D(0, 0, 5), Point3D(0, 0, 0), Point3D(0, 1, 0));
	renderer.AddLight(Point3D(-1, -1, 1), White());
	renderer.AddLight(Point3D(1, 1, 1), Blue());
	renderer.Rotate(M_PI_4, M_PI_4, M_PI_4); // 45 degrees per axis
	renderer.Render(model);
	renderer.Rasterize();

	PNGEncoder().SaveFile(GetHomeDirFile("uppbox-snapshot.png"), renderer.GetImage());

}
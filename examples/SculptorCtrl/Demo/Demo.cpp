#include "Demo.h"

using namespace Upp;

ObjectViewer::ObjectViewer()
{
	WantFocus();
	files = { "teapot.obj", "skull.obj", "bunny.obj" };

	SetTimeCallback(-16, [=] {
		if(abs(goalx - rotx) > 0.001f || abs(goaly - roty) > 0.001f || abs(goal - zoom) > 0.001f) {
			rotx += (goalx - rotx) * 0.15f;
			roty += (goaly - roty) * 0.15f;
			zoom += (goal - zoom)  * 0.45f;
			Refresh();
		}
	});
}

void ObjectViewer::LoadModel()
{
	ClearScene();
	
	if(!LoadModelFromFile(model, GetDataFile(files[modelindex]))) {
		Exclamation(Format("Could not load object file: %s", files[modelindex]));
		return;
	}

	model.Normalize(2.5f);
	
	Box3D bounds = model.ComputeBounds();
	gradbase = bounds.lo.y;
	gradspan = max(bounds.hi.y - bounds.lo.y, 0.001f);

	int64 tid = model.AddTexture(CreateImage(Size(1, 1), White()));
	for(ModelGL::Vertex& v : model.vertices)
		v.textureid = tid;
}

void ObjectViewer::InitScene()
{
	LoadModel();
	
	extern const char *pixelshader, *vertexshader, *fragmentshader;

	SetShader("model-shader", vertexshader, fragmentshader);
	SetPostProcess(pixelshader);
}

void ObjectViewer::Render(GLScene& gls)
{
	Sizef sz = GetSize();
	
	gls.ClearBuffer(Color(100, 115, 200));
	
	gls.Cull(!wireframe)
		.Wireframe(wireframe)
		.Shading(true)
		.Bilinear(bilinear)
		.MipMap(mipmap)
		.AlphaBlend(false);

	gls.Perspective(M_PI / 3.0f, sz.cx / sz.cy, 0.1f, 1000.0f);
	gls.LookAt(Point3D(0, 0, zoom), Point3D(0, 0, 0), Point3D(0, 1, 0));

	gls.ClearLights();
	gls.AddLight(Point3D(0.6f, 1.0f, 0.5f), White());
	if(illum)
		gls.AddLight(Point3D(-1.0f, -0.2f, -0.5f), Color(255, 50, 50));
	
	gls.UseShader("model-shader");

	gls.Uniform("uUseGradient", gradient ? 1.0f : 0.0f);
	gls.Uniform("uGradBase", gradbase);
	gls.Uniform("uGradSpan", gradspan);
	gls.Uniform("uColorLo", colorlo);
	gls.Uniform("uColorHi", colorhi);
	gls.Uniform("uTriSample", trisample ? 1.0f : 0.0f);
	gls.Uniform("uRimShade", rimshade ? 1.0f : 0.0f);
	gls.Uniform("uVignette", vignette ? 1.0f : 0.0f);
	
	AddOverlay(gls);

	gls.Rotate(rotx, roty, 0);
	gls.Render(model);

}

void ObjectViewer::AddOverlay(GLScene& gls)
{
	WString txt = Format(
		"Model: %s\n"
		"Zoom:  %.1f\n"
		"==================\n"
		"[W] Wireframe  %[1:On;Off]s\n"
		"[B] Bilinear   %[1:On;Off]s\n"
		"[M] Mipmap     %[1:On;Off]s\n"
		"[G] Gradient   %[1:On;Off]s\n"
		"[T] Triplanar  %[1:On;Off]s\n"
		"[P] Rim shader %[1:On;Off]s\n"
		"[F] Vignette   %[1:On;Off]s\n"
		"[L] Lights     %d",
		files[modelindex],
		zoom,
		wireframe,
		bilinear,
		mipmap,
		gradient,
		trisample,
		rimshade,
		vignette,
		illum ? 2 : 1
	).ToWString();

	constexpr int PAD = 10;
	Size tsz = GetTLTextSize(txt, Monospace(12));
	Size bsz = tsz + Size(PAD, PAD) * 2;
	
	ImagePainter w(bsz);
	w.RoundedRectangle(0, 0, bsz.cx, bsz.cy, 8);
	w.Fill(RGBA { 16, 16, 22, 200 });

	DrawTLText(w, PAD, PAD, bsz.cx, txt, Monospace(12), LtGray());
	
	gls.AddOverlay(GetSize().cx - bsz.cx - 12, 12, w);
}

bool ObjectViewer::Key(dword key, int count)
{
	switch(key) {
	case 'w':
	case 'W':
		wireframe ^= 1;
		break;
	case 'b':
	case 'B':
		bilinear ^= 1;
		break;
	case 'g':
	case 'G':
		gradient ^= 1;
		break;
	case 'm':
	case 'M':
		mipmap ^= 1;
		break;
	case 'l':
	case 'L':
		illum ^= 1;
		break;
	case 't':
	case 'T':
		trisample ^= 1;
		break;
	case 'p':
	case 'P':
		rimshade ^= 1;
		break;
	case 'f':
	case 'F':
		vignette ^= 1;
		break;
	case K_TAB:
		modelindex = (modelindex + 1) % files.GetCount();
		LoadModel();
		break;
	case K_UP:
	case K_ADD:
	case '+':
		goal = max(0.5f, goal - 1.0f);
		break;
	case K_DOWN:
	case K_SUBTRACT:
	case '-':
		goal += 1.0f;
		break;
	default:
		return false;
	}

	Refresh();
	return true;
}

void ObjectViewer::LeftDown(Point p, dword keyflags)
{
	SetFocus();
	mouse = p;
	dragging = true;
	SetCapture();
}

void ObjectViewer::LeftUp(Point p, dword keyflags)
{
	dragging = false;
	ReleaseCapture();
}

void ObjectViewer::MouseWheel(Point p, int zdelta, dword keyflags)
{
	goal = max(0.5f, goal - (zdelta / 100.0f));
	Refresh();
}

void ObjectViewer::MouseMove(Point p, dword keyflags)
{
	if(dragging) {
		goaly -= (p.x - mouse.x) * 0.15f;
		goalx += (p.y - mouse.y) * 0.15f;
		mouse = p;
	}
}

GUI_APP_MAIN
{
	Ctrl::GlobalBackBuffer();

	ObjectViewer viewer;
	
	TopWindow w;
	w.Title("SculptorCtrl Demo");
	w.Sizeable().Zoomable().CenterScreen().SetRect(0, 0, 800, 600);
	w.Add(viewer.SizePos());
	w.Run();
}
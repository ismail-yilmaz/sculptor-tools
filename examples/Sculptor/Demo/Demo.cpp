#include "Demo.h"

using namespace Upp;

Image MakeTag(const String& text)
{
	Font fnt = Arial(11).Bold();
	Size tsz = GetTextSize(text, fnt);
	Size isz = tsz + Size(12, 6);

	ImagePainter p(isz);
	p.Clear(RGBAZero());
	p.RoundedRectangle(0, 0, isz.cx, isz.cy, 4).Fill(RGBA {20, 20, 30, 220}).Stroke(1, LtGray());
	p.DrawText(6, 3, text, fnt, White());

	return p;
}

ObjectViewer::ObjectViewer()
{
	Title("Sculptor Demo");
	Sizeable().Zoomable().CenterScreen().SetRect(0, 0, 800, 600);

	files = { "teapot.obj", "skull.obj", "bunny.obj" };

	LoadModel();
	LoadModels();

	SetTimeCallback(-16, [=] {
		if(abs(goalx - rotx) > 0.001f || abs(goaly - roty) > 0.001f || abs(goal - zoom) > 0.001f) {
			rotx += (goalx - rotx) * 0.15f;
			roty += (goaly - roty) * 0.15f;
			zoom += (goal - zoom)  * 0.45f;
			Refresh();
		}
	});
}

void ObjectViewer::SetGradient()
{
	Color lo = White(), hi = White();
	if(gradient) {
		lo = Color(40, 60, 160);
		hi = Color(255, 200, 120);
	}
	ApplyGradient(model, lo, hi);
	for(SceneModel& sm : models) {
		ApplyGradient(sm.model, lo, hi);
	}
}

void ObjectViewer::LoadModel()
{
	if(!LoadModelFromFile(model, GetDataFile(files[modelindex]))) {
		Exclamation(Format("Could not load object file: %s", files[modelindex]));
		return;
	}

	model.Normalize(2.5f);

	SetGradient();
	int64 tid = model.AddTexture(CreateImage(Size(1, 1), White()));
	for(Model3D::Vertex& v : model.vertices)
		v.textureid = tid;
}

void ObjectViewer::LoadModels()
{
	models.Clear();
	float radius = 3.5f;

	Color lo = White(), hi = White();
	if(gradient) {
		lo = Color(40, 60, 160);
		hi = Color(255, 200, 120);
	}

	for(int i = 0; i < files.GetCount(); i++) {
		SceneModel& sm = models.Add();
		sm.name = GetFileTitle(files[i]);
		sm.tag = MakeTag(sm.name);

		float angle = (float)i * (float)(2.0 * M_PI / files.GetCount());
		sm.pos = Point3D(radius * cos(angle), 0.0f, radius * sin(angle));

		if(!LoadModelFromFile(sm.model, GetDataFile(files[i]))) {
			Exclamation(Format("Could not load object file: %s", files[i]));
			continue;
		}

		sm.model.Normalize(2.5f);
		ApplyGradient(sm.model, lo, hi);

		Box3D bounds = sm.model.ComputeBounds();
		sm.gradbase = bounds.lo.y;
		sm.gradspan = max(bounds.hi.y - bounds.lo.y, 0.001f);

		int64 tid = sm.model.AddTexture(CreateImage(Size(1, 1), White()));
		for(Model3D::Vertex& v : sm.model.vertices)
			v.textureid = tid;
	}
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
		SetGradient();
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
		triplanar ^= 1;
		break;
	case 'p':
	case 'P':
		rim ^= 1;
		break;
	case 'f':
	case 'F':
		vignette ^= 1;
		break;
	case 'o':
	case 'O':
		multiobject ^= 1;
		if(multiobject)
			goal = max(goal, 12.0f);
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

void ObjectViewer::PaintOverlay(Draw& w) const
{
	String modelname = multiobject ? "Multi-Object" : files[modelindex];
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
		"[O] Multi-Obj  %[1:On;Off]s\n"
		"[L] Lights     %d",
		modelname,
		zoom,
		wireframe,
		bilinear,
		mipmap,
		gradient,
		triplanar,
		rim,
		vignette,
		multiobject,
		illum ? 2 : 1
	).ToWString();

	constexpr int PAD = 10;
	Size tsz = GetTLTextSize(txt, Monospace(12));
	Size bsz = tsz + Size(PAD, PAD) * 2;

	ImagePainter ip(bsz);
	ip.RoundedRectangle(0, 0, bsz.cx, bsz.cy, 8);
	ip.Fill(RGBA{16, 16, 22, 200});
	DrawTLText(ip, PAD, PAD, tsz.cx, txt, Monospace(12), LtGray());
	w.DrawImage(GetSize().cx - bsz.cx - 12, 12, ip);
}

void ObjectViewer::Paint(Draw& w)
{
	Sizef sz = GetSize();

	Sculptor renderer(sz, Color(100, 115, 200));

	renderer.Co()
			.Cull(!wireframe)
			.Wireframe(wireframe)
			.Shading(true)
			.Bilinear(bilinear)
			.MipMap(mipmap)
			.AlphaBlend(false);

	renderer.Perspective(M_PI / 3.0f, sz.cx / sz.cy, 0.1f, 1000.0f);
	renderer.LookAt(Point3D(0, 0, zoom), Point3D(0, 0, 0), Point3D(0, 1, 0));
	renderer.AddLight(Point3D(0.6f, 1.0f, 0.5f), White());

	if(illum)
		renderer.AddLight(Point3D(-1.0f, -0.2f, -0.5f), Color(255, 50, 50));

	if(triplanar)
		renderer.SetTextureSampler(TriplanarSampler);

	if(rim)
		renderer.SetPixelShader(RimShader);

	if(vignette)
		renderer.AddPostFilter(Vignette);

	renderer.Rotate(rotx, roty, 0);

	if(multiobject) {
		for(const SceneModel& sm : models) {
			Sculptor::Scope __(renderer);
			renderer.Translate(sm.pos);
			renderer.Render(sm.model);
			float tagy = sm.gradbase + sm.gradspan + 0.5f;
			renderer.AddOverlay(Point3D(0.0f, tagy, 0.0f), sz, sm.tag, Sculptor::Overlay::BottomCenter);
		}
	}
	else {
		renderer.Render(model);
	}

	renderer.Rasterize();

	w.DrawImage(0, 0, renderer.GetImage());

	PaintOverlay(w);
}

GUI_APP_MAIN
{
	ObjectViewer().Run();
}
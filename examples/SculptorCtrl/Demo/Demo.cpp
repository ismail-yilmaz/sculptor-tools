#include "Demo.h"

using namespace Upp;

constexpr float DEGRAD = (float)(M_PI / 180.0);

Image MakeTag(const String& text)
{
	Font fnt = Arial(11).Bold();
	Size tsz = GetTextSize(text, fnt);
	Size isz = tsz + Size(12, 6);

	ImagePainter p(isz);
	p.Clear(RGBAZero());
	p.RoundedRectangle(0, 0, isz.cx, isz.cy, 4)
	.Fill(RGBA { 20, 20, 30, 220 })
	.Stroke(1, LtGray());
	p.DrawText(6, 3, text, fnt, White());

	return p;
}

Image MakeHoverTag(const String& name, const ModelGL& m, const Ray3D::Hit& hit, bool picked)
{
	Font titlefnt = Arial(11).Bold();
	Font infofnt = Monospace(10);

	String status = picked ? " [Pick]" : " [Hover]";
	String info;
	info << Format("Tri #%d (d=%.2f)\n", hit.triangle, hit.distance);
	if(hit.triangle >= 0 && hit.triangle < m.triangles.GetCount()) {
		const ModelGL::Triangle& t = m.triangles[hit.triangle];
		info << Format("Verts: %d, %d, %d\n", t.a, t.b, t.c);
	}
	info << Format("Pos:   %.2f, %.2f, %.2f\n", hit.point.x, hit.point.y, hit.point.z)
		<< Format("Norm:  %.2f, %.2f, %.2f", hit.normal.x, hit.normal.y, hit.normal.z);

	Vector<String> lns = Split(info, '\n');

	Size namesz = GetTextSize(name, titlefnt);
	int w = namesz.cx + GetTextSize(status, titlefnt).cx;
	for(const String& ln : lns)
		w = max(w, GetTextSize(ln, infofnt).cx);

	int titleh = titlefnt.GetCy();
	int h = titleh + 5 + lns.GetCount() * infofnt.GetCy();
	Size isz(w + 12, h + 12);

	RGBA accent = picked ? LtGreen() : LtGray();
	RGBA textcol = picked ? Color(255, 225, 160) : LtGray();

	ImagePainter p(isz);
	p.Clear(RGBAZero());
	p.RoundedRectangle(0, 0, isz.cx, isz.cy, 4)
	.Fill(RGBA { 24, 28, 42, 235 })
	.Stroke(1, accent);
	p.DrawText(6, 3, name, titlefnt, White());
	p.DrawText(6 + namesz.cx, 3, status, titlefnt, accent);

	int y = 6 + titleh + 2;
	for(const String& ln : lns) {
		p.DrawText(6, y, ln, infofnt, textcol);
		y += infofnt.GetCy();
	}

	return p;
}

Image MakeHitMarker(Color accent)
{
	RGBA r = accent; r.a = 255;
	ImagePainter p(16, 16);
	p.Clear(RGBAZero());
	p.Circle(8, 8, 6).Stroke(2, RGBA { 20, 20, 30, 220 });
	p.Circle(8, 8, 5).Stroke(1.5, r);
	p.Circle(8, 8, 2).Fill(White());
	return p;
}

ObjectViewer::ObjectViewer()
{
	WantFocus();
	files = { "teapot.obj", "skull.obj", "bunny.obj" };
	hovermarker = MakeHitMarker(Color(130, 210, 255));
	pickmarker = MakeHitMarker(Color(255, 200, 120));

	SetTimeCallback(-16, [=] {
		bool redraw = false;

		if(abs(goalrot.x - rot.x) > 0.001f
		|| abs(goalrot.y - rot.y) > 0.001f
		|| abs(goal - zoom) > 0.001f
		|| abs(goalpan.x - pan.x) > 0.001f
		|| abs(goalpan.y - pan.y) > 0.001f) {
			rot += (goalrot - rot) * 0.30f;

			if(panning)
				pan = goalpan;
			else
				pan += (goalpan - pan) * 0.45f;

			zoom += (goal - zoom) * 0.45f;
			redraw = true;
		}

		if(hoverdirty) {
			hoverdirty = false;
			redraw = true;
		}

		if(redraw)
			Refresh();
	});
}

void ObjectViewer::LoadModel()
{
	ClearScene();
	ClearPick();
	hoverhit.Clear();
	hoverindex = -1;
	hovertag.Clear();
	hovertagidx = -1;

	if(!LoadModelFromFile(model, GetDataFile(files[modelindex]))) {
		Exclamation(Format("Could not load object file: %s", files[modelindex]));
		return;
	}

	model.Normalize(2.5f);

	Box3D bounds = model.ComputeBounds();
	gradbase = bounds.lo.y;
	gradspan = max(bounds.hi.y - bounds.lo.y, 0.001f);
	modeltagy = bounds.hi.y + 0.5f;
	modeltag = MakeTag(GetFileTitle(files[modelindex]));

	int64 tid = model.AddTexture(CreateImage(Size(1, 1), White()));
	for(ModelGL::Vertex& v : model.vertices)
		v.textureid = tid;
}

void ObjectViewer::LoadModels()
{
	models.Clear();
	float radius = 3.5f;

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

		Box3D bounds = sm.model.ComputeBounds();
		sm.gradbase = bounds.lo.y;
		sm.gradspan = max(bounds.hi.y - bounds.lo.y, 0.001f);

		int64 tid = sm.model.AddTexture(CreateImage(Size(1, 1), White()));
		for(ModelGL::Vertex& v : sm.model.vertices)
			v.textureid = tid;
	}
}

void ObjectViewer::InitScene()
{
	LoadModel();
	LoadModels();

	extern const char *pixelshader, *vertexshader, *fragmentshader;

	SetShader("model-shader", vertexshader, fragmentshader);
	SetPostProcess(pixelshader);
}

MatrixGL ObjectViewer::GetModelTransform() const
{
	return MatrixGL::Rotation(rot.x * DEGRAD, rot.y * DEGRAD, 0.0f);
}

MatrixGL ObjectViewer::GetModelTransform(int index) const
{
	return MatrixGL::Rotation(rot.x * DEGRAD, rot.y * DEGRAD, 0.0f) * MatrixGL::Translation(models[index].pos);
}

void ObjectViewer::UpdateHover(const GLScene& gls, Size sz)
{
	if(dragging || panning || sz.cx <= 0 || sz.cy <= 0 || !Rect(sz).Contains(mouse)) {
		hoverindex = -1;
		hoverhit.Clear();
		pickpending = false;
		return;
	}

	bool backface = wireframe || xray;
	if(multiobject) {
		int bestidx = -1;
		Ray3D::Hit besthit;
		for(int i = 0; i < models.GetCount(); i++) {
			Ray3D::Hit hit = gls.Pick(mouse, sz, models[i].model, GetModelTransform(i), backface);
			if(hit < besthit) {
				besthit = hit;
				bestidx = i;
			}
		}
		hoverindex = bestidx;
		hoverhit = besthit;
	}
	else {
		hoverhit = gls.Pick(mouse, sz, model, GetModelTransform(), backface);
		hoverindex = hoverhit ? modelindex : -1;
	}

	if(pickpending) {
		pickpending = false;
		if(hoverhit) {
			pickhit = hoverhit;
			pickindex = hoverindex;
		}
		else {
			pickhit.Clear();
			pickindex = -1;
		}
		hovertag.Clear();
		hovertagidx = -1;
	}
}

void ObjectViewer::PickAt(Point p)
{
	mouse = p;
	pickpending = true;
	Refresh();
}

void ObjectViewer::ClearPick()
{
	pickpending = false;
	if(!pickhit && pickindex == -1)
		return;

	pickhit.Clear();
	pickindex = -1;
	hovertag.Clear();
	hovertagidx = -1;
	Refresh();
}

Image ObjectViewer::GetHoverTag(int index, const String& name, const ModelGL& m, const Ray3D::Hit& hit, bool picked)
{
	if(!IsNull(hovertag)
	&& hovertagidx == index
	&& hovertagtri == hit.triangle
	&& hovertagpicked == picked
	&& abs(hit.distance - hovertagdist) < 0.005f
	&& !IsNull(hovertagpos) && IsEpsqual(hit.point, hovertagpos, 0.005f)
	&& !IsNull(hovertagnorm) && IsEpsqual(hit.normal, hovertagnorm, 0.005f))
		return hovertag;

	hovertagidx = index;
	hovertagtri = hit.triangle;
	hovertagpicked = picked;
	hovertagdist = hit.distance;
	hovertagpos = hit.point;
	hovertagnorm = hit.normal;
	hovertag = MakeHoverTag(name, m, hit, picked);
	return hovertag;
}

void ObjectViewer::RenderScene(GLScene& gls, Sizef sz)
{
	GLScene::Scope __(gls);
	gls.Rotate(rot.x * DEGRAD, rot.y * DEGRAD, 0.0f);

	if(multiobject) {
		for(int i = 0; i < models.GetCount(); i++) {
			SceneModel& sm = models[i];
			GLScene::Scope ___(gls);
			gls.Translate(sm.pos);
			gls.Uniform("uGradBase", sm.gradbase);
			gls.Uniform("uGradSpan", sm.gradspan);
			gls.Render(sm.model);
			float tagy = sm.gradbase + sm.gradspan + 0.5f;
			bool hovered = (i == hoverindex && hoverhit);
			Image tag = sm.tag;
			if(hovered) {
				bool picked = pickhit && pickindex == i && pickhit.triangle == hoverhit.triangle;
				tag = GetHoverTag(i, sm.name, sm.model, hoverhit, picked);
			}
			gls.AddOverlay(Point3D(0.0f, tagy, 0.0f), sz, tag, GLScene::Overlay::BottomCenter, !hovered);
		}
	}
	else {
		gls.Render(model);
		bool hovered = (bool)hoverhit;
		Image tag = modeltag;
		if(hovered) {
			bool picked = pickhit && pickhit.triangle == hoverhit.triangle;
			tag = GetHoverTag(modelindex, GetFileTitle(files[modelindex]), model, hoverhit, picked);
		}
		gls.AddOverlay(Point3D(0.0f, modeltagy, 0.0f), sz, tag, GLScene::Overlay::BottomCenter, !hovered);
	}
}

void ObjectViewer::AddOverlay(GLScene& gls)
{
	String modelname = multiobject ? "Multi-Object" : files[modelindex];

	String txt = Format(
		"Model: %s\n"
		"Zoom:  %.1f\n"
		"==================\n"
		"[W] Wireframe (CAD) %[1:On;Off]s\n"
		"[X] X-Ray           %[1:On;Off]s\n"
		"[A] Transparency    %[1:On;Off]s\n"
		"[B] Bilinear        %[1:On;Off]s\n"
		"[M] Mipmap          %[1:On;Off]s\n"
		"[G] Gradient        %[1:On;Off]s\n"
		"[T] Triplanar       %[1:On;Off]s\n"
		"[P] Rim shader      %[1:On;Off]s\n"
		"[F] Vignette        %[1:On;Off]s\n"
		"[I] Isometric       %[1:On;Off]s\n"
		"[O] Multi-Object    %[1:On;Off]s\n"
		"[L] Lights          %d",
		modelname,
		zoom,
		wireframe,
		xray,
		alphablend,
		bilinear,
		mipmap,
		gradient,
		trisample,
		rimshade,
		vignette,
		isometric,
		multiobject,
		illum ? 2 : 1
	);

	if(txt != hudtxt || IsNull(hudimg)) {
		hudtxt = txt;
		WString wtxt = txt.ToWString();
		constexpr int PAD = 10;
		Size tsz = GetTLTextSize(wtxt, Monospace(12));
		Size bsz = tsz + Size(PAD, PAD) * 2;

		ImagePainter w(bsz);
		w.RoundedRectangle(0, 0, bsz.cx, bsz.cy, 8);
		w.Fill(RGBA { 16, 16, 22, 200 });

		DrawTLText(w, PAD, PAD, bsz.cx, wtxt, Monospace(12), LtGray());
		hudimg = w;
	}

	gls.AddOverlay(GetSize().cx - hudimg.GetWidth() - 12, 12, hudimg);
}

void ObjectViewer::Render(GLScene& gls)
{
	Size sz = GetSize();
	if(sz.cx <= 0 || sz.cy <= 0)
		return;

	Sizef fsz = sz;

	gls.ClearBuffer(Color(100, 115, 200));

	gls.Cull(!wireframe && !xray)
	 .Wireframe(wireframe)
	 .Shading(true)
	 .Bilinear(bilinear)
	 .MipMap(mipmap)
	 .AlphaBlend(alphablend);

	if(isometric) {
		gls.Isometric(zoom, fsz.cx / fsz.cy, -1000.0f, 1000.0f);
		gls.LookAt(Point3D(pan.x, pan.y, 0.0f), Point3D(pan.x, pan.y, -1.0f), Point3D(0, 1, 0));
	}
	else {
		gls.Perspective(M_PI / 3.0f, fsz.cx / fsz.cy, 0.1f, 1000.0f);
		gls.LookAt(Point3D(pan.x, pan.y, zoom), Point3D(pan.x, pan.y, 0.0f), Point3D(0, 1, 0));
	}

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
	gls.Uniform("uAlphaBase", alphablend ? 0.4f : 1.0f);
	gls.Uniform("uXRay", xray ? 1.0f : 0.0f);

	gls.Uniform("uResolution", Point3D(fsz.cx, fsz.cy, 0.0f));

	UpdateHover(gls, sz);
	RenderScene(gls, fsz);

	if(hoverhit) {
		bool picked = pickhit && pickindex == hoverindex && pickhit.triangle == hoverhit.triangle;
		gls.AddOverlay(hoverhit.point, fsz, picked ? pickmarker : hovermarker, GLScene::Overlay::Center, false);
	}

	AddOverlay(gls);
}

bool ObjectViewer::Key(dword key, int count)
{
	switch(key) {
	case 'w':
	case 'W':
		wireframe ^= 1;
		break;
	case 'x':
	case 'X':
		xray ^= 1;
		break;
	case 'a':
	case 'A':
		alphablend ^= 1;
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
	case 'i':
	case 'I':
		isometric ^= 1;
		if(isometric) {
			goalrot = rot = Pointf(0.0f, 0.0f);
		}
		break;
	case 'o':
	case 'O':
		multiobject ^= 1;
		ClearPick();
		break;
	case K_TAB:
		modelindex = (modelindex + 1) % files.GetCount();
		LoadModel();
		break;
	case K_ESCAPE:
		ClearPick();
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
	mouse = clickmouse = p;
	dragging = true;
	SetCapture();
}

void ObjectViewer::LeftUp(Point p, dword keyflags)
{
	dragging = false;
	ReleaseCapture();

	if(abs(p.x - clickmouse.x) <= 3 && abs(p.y - clickmouse.y) <= 3) {
		PickAt(p);
	}
	else {
		mouse = p;
		Refresh();
	}
}

void ObjectViewer::MiddleDown(Point p, dword keyflags)
{
	SetFocus();
	panmouse = p;
	panning = true;
	hoverindex = -1;
	hoverhit.Clear();
	SetCapture();
	Refresh();
}

void ObjectViewer::MiddleUp(Point p, dword keyflags)
{
	pan = goalpan;
	mouse = p;
	panning = false;
	ReleaseCapture();
	Refresh();
}

void ObjectViewer::MouseWheel(Point p, int zdelta, dword keyflags)
{
	goal = max(0.5f, goal - (zdelta / 100.0f));
	Refresh();
}

void ObjectViewer::MouseMove(Point p, dword keyflags)
{
	Size sz = GetSize();

	if(dragging) {
		Pointf d((float)(p.x - mouse.x), (float)(p.y - mouse.y));
		goalrot.y -= d.x * 0.5f;
		goalrot.x += d.y * 0.5f;
		if((abs(p.x - clickmouse.x) > 3 || abs(p.y - clickmouse.y) > 3) && (hoverindex >= 0 || hoverhit)) {
			hoverindex = -1;
			hoverhit.Clear();
		}
	}

	mouse = p;

	// Here we convert screen-space mouse movement into world-space pan. Unlike perspective
	// mode, the isometric projection rotates the view by -45 degree around Y and 35.264 degree
	// around X before applying the orthographic projection. Consequently, screen
	// X and Y are not aligned with the world-space pan axes.
	//
	// Inverting this projection gives the required pan displacement. The scale
	// is 2 * zoom / viewportwidth, while the -tan(pitch) term compensates for
	// the horizontal displacement introduced by vertical panning.
	//
	// This keeps horizontal and vertical mouse drags aligned with the screen,
	// independently of the viewport aspect ratio.

	if(panning) {
		Pointf d((float)(p.x - panmouse.x), (float)(p.y - panmouse.y));
		if(isometric && sz.cx > 0) {
			const float pitch = 35.264f * DEGRAD;
			const float s = 2.0f * zoom / sz.cx;

			goalpan.x += (1.41421356f * d.x - tan(pitch) * d.y) * s;
			goalpan.y += (d.y / cos(pitch)) * s;
		}
		else {
			float s = zoom * 0.002f;
			goalpan.x += d.x * s;
			goalpan.y += d.y * s;
		}
		panmouse = p;
	}

	if(!dragging && !panning)
		hoverdirty = true;
}

void ObjectViewer::MouseLeave()
{
	mouse = Point(-1, -1);
	hoverdirty = false;
	if(hoverindex >= 0 || hoverhit) {
		hoverindex = -1;
		hoverhit.Clear();
		Refresh();
	}
}

Image ObjectViewer::CursorImage(Point p, dword keyflags)
{
	if(!dragging && !panning && hoverhit)
		return Image::Hand();
	return SculptorCtrl::CursorImage(p, keyflags);
}

GUI_APP_MAIN
{
	Ctrl::GlobalBackBuffer();

	ObjectViewer viewer;

	TopWindow w;
	w.Title("SculptorCtrl Demo");
	w.Sizeable().Zoomable().CenterScreen().SetRect(0, 0, 1024, 768);
	w.Add(viewer.SizePos());

	w.Run();
}
#include "Demo.h"

using namespace Upp;

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

Image MakeHoverTag(const String& name, const Model3D& m, const Ray3D::Hit& hit, bool picked)
{
	Font titlefnt = Arial(11).Bold();
	Font infofnt = Monospace(10);

	String status = picked ? " [Pick]" : " [Hover]";
	String info;
	info << Format("Tri #%d (d=%.2f)\n", hit.triangle, hit.distance);
	if(hit.triangle >= 0 && hit.triangle < m.triangles.GetCount()) {
		const Model3D::Triangle& t = m.triangles[hit.triangle];
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

	Color accent = picked ? LtGreen() : LtGray();
	Color textcol = picked ? Color(255, 225, 160) : LtGray();

	ImagePainter p(isz);
	p.Clear(RGBAZero());
	p.RoundedRectangle(0, 0, isz.cx, isz.cy, 4)
	 .Fill(RGBA { 24, 28, 42, 235 })
	 .Stroke(1, accent);
	p.DrawText(6, 3, name, titlefnt, White());
	p.DrawText(6 + namesz.cx, 3, status, titlefnt, accent);

	int y = 6 + titleh + 2;
	y += 3;
	for(const String& ln : lns) {
		p.DrawText(6, y, ln, infofnt, textcol);
		y += infofnt.GetCy();
	}

	return p;
}
Image MakeHitMarker(Color accent)
{
	ImagePainter p(16, 16);
	p.Clear(RGBAZero());
	p.Circle(8, 8, 6).Stroke(2, RGBA { 20, 20, 30, 220 });
	p.Circle(8, 8, 5).Stroke(1.5, accent);
	p.Circle(8, 8, 2).Fill(White());
	return p;
}

ObjectViewer::ObjectViewer()
{
	Title("Sculptor Demo");
	Sizeable().Zoomable().CenterScreen().SetRect(0, 0, 800, 600);

	files = { "teapot.obj", "skull.obj", "bunny.obj" };
	hovermarker = MakeHitMarker(Color(130, 210, 255));
	pickmarker = MakeHitMarker(Color(255, 200, 120));

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
	modeltagy = model.ComputeBounds().hi.y + 0.5f;
	modeltag = MakeTag(GetFileTitle(files[modelindex]));

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

Matrix4D ObjectViewer::GetModelTransform() const
{
	return Matrix4D::Rotation(rotx, roty, 0);
}

Matrix4D ObjectViewer::GetModelTransform(int index) const
{
	return Matrix4D::Translation(models[index].pos) * Matrix4D::Rotation(rotx, roty, 0);
}

bool ObjectViewer::UpdateHover()
{
	int previdx = hoverindex;
	Ray3D::Hit prevhit = hoverhit;

	Size sz = GetSize();
	if(dragging || sz.cx <= 0 || sz.cy <= 0 || !Rect(sz).Contains(mouse)) {
		hoverindex = -1;
		hoverhit.Clear();
	}
	else
	if(multiobject) {
		int bestindex = -1;
		Ray3D::Hit besthit;
		for(int i = 0; i < models.GetCount(); i++) {
			Ray3D::Hit hit = renderer.Pick(mouse, sz, models[i].model, GetModelTransform(i), wireframe);
			if(hit < besthit) {
				besthit = hit;
				bestindex = i;
			}
		}
		hoverindex = bestindex;
		hoverhit = besthit;
	}
	else {
		hoverhit = renderer.Pick(mouse, sz, model, GetModelTransform(), wireframe);
		hoverindex = hoverhit ? modelindex : -1;
	}

	return hoverindex != previdx
	    || bool(hoverhit) != bool(prevhit)
	    || hoverhit.triangle != prevhit.triangle
	    || (hoverhit && (!IsEpsqual(hoverhit.point, prevhit.point, 0.005f)
	                  || !IsEpsqual(hoverhit.normal, prevhit.normal, 0.005f)));
}

void ObjectViewer::PickAt(Point p)
{
	mouse = p;
	UpdateHover();

	if(hoverhit) {
		pickhit = hoverhit;
		pickindex = hoverindex;
		hovertag.Clear();
		hovertagidx = -1;
		Refresh();
	}
	else {
		ClearPick();
	}
}

void ObjectViewer::ClearPick()
{
	if(!pickhit && pickindex == -1)
		return;

	pickhit.Clear();
	pickindex = -1;
	hovertag.Clear();
	hovertagidx = -1;
	Refresh();
}

Image ObjectViewer::GetHoverTag(int index, const String& name, const Model3D& m, const Ray3D::Hit& hit, bool picked)
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

void ObjectViewer::RenderScene(Size sz)
{
	Sculptor::Scope __(renderer);
	renderer.Rotate(rotx, roty, 0);

	if(multiobject) {
		for(int i = 0; i < models.GetCount(); i++) {
			const SceneModel& sm = models[i];
			Sculptor::Scope ___(renderer);
			renderer.Translate(sm.pos);
			renderer.Render(sm.model);
			float tagy = sm.gradbase + sm.gradspan + 0.5f;
			bool hovered = (i == hoverindex && hoverhit);
			Image tag = sm.tag;
			if(hovered) {
				bool picked = pickhit && pickindex == i && pickhit.triangle == hoverhit.triangle;
				tag = GetHoverTag(i, sm.name, sm.model, hoverhit, picked);
			}
			renderer.AddOverlay(Point3D(0.0f, tagy, 0.0f), sz, tag, Sculptor::Overlay::BottomCenter, !hovered);
		}
	}
	else {
		renderer.Render(model);
		bool hovered = (bool)hoverhit;
		Image tag = modeltag;
		if(hovered) {
			bool picked = pickhit && pickhit.triangle == hoverhit.triangle;
			tag = GetHoverTag(modelindex, GetFileTitle(files[modelindex]), model, hoverhit, picked);
		}
		renderer.AddOverlay(Point3D(0.0f, modeltagy, 0.0f), sz, tag, Sculptor::Overlay::BottomCenter, !hovered);
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
		ClearPick();
		if(multiobject)
			goal = max(goal, 12.0f);
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
		if(UpdateHover())
			Refresh();
	}
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
		if((abs(p.x - clickmouse.x) > 3 || abs(p.y - clickmouse.y) > 3) && (hoverindex >= 0 || hoverhit)) {
			hoverindex = -1;
			hoverhit.Clear();
		}
	}
	else {
		mouse = p;
		if(UpdateHover())
			Refresh();
	}
}

void ObjectViewer::MouseLeave()
{
	mouse = Point(-1, -1);
	if(hoverindex >= 0 || hoverhit) {
		hoverindex = -1;
		hoverhit.Clear();
		Refresh();
	}
}

Image ObjectViewer::CursorImage(Point p, dword keyflags)
{
	if(!dragging && hoverhit)
		return Image::Hand();
	return TopWindow::CursorImage(p, keyflags);
}

void ObjectViewer::PaintOverlay(Draw& w) const
{
	String modelname = multiobject ? "Multi-Object" : files[modelindex];
	String txt = Format(
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
	);

	if(txt != hudtxt || IsNull(hudimg)) {
		hudtxt = txt;
		WString wtxt = txt.ToWString();
		constexpr int PAD = 10;
		Size tsz = GetTLTextSize(wtxt, Monospace(12));
		Size bsz = tsz + Size(PAD, PAD) * 2;

		ImagePainter ip(bsz);
		ip.RoundedRectangle(0, 0, bsz.cx, bsz.cy, 8);
		ip.Fill(RGBA { 16, 16, 22, 200 });
		DrawTLText(ip, PAD, PAD, tsz.cx, wtxt, Monospace(12), LtGray());
		hudimg = ip;
	}

	w.DrawImage(GetSize().cx - hudimg.GetWidth() - 12, 12, hudimg);
}

void ObjectViewer::Paint(Draw& w)
{
	Size sz = GetSize();
	if(sz.cx <= 0 || sz.cy <= 0)
		return;

	renderer.ClearScene();
	renderer.ClearLights();
	renderer.ClearPostFilters();
	renderer.SetTextureSampler({});
	renderer.SetPixelShader({});
	renderer.SetCanvas(sz, Color(100, 115, 200));

	renderer.Co()
			.Cull(!wireframe)
			.Wireframe(wireframe)
			.Shading(true)
			.Bilinear(bilinear)
			.MipMap(mipmap)
			.AlphaBlend(false);

	renderer.Perspective(M_PI / 3.0f, (float)sz.cx / sz.cy, 0.1f, 1000.0f);
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

	UpdateHover();
	RenderScene(sz);

	if(hoverhit) {
		bool picked = pickhit && pickindex == hoverindex && pickhit.triangle == hoverhit.triangle;
		renderer.AddOverlay(hoverhit.point, sz, picked ? pickmarker : hovermarker, Sculptor::Overlay::Center);
	}

	renderer.Rasterize();

	w.DrawImage(0, 0, renderer.GetImage());

	PaintOverlay(w);
}

GUI_APP_MAIN
{
	ObjectViewer().Run();
}
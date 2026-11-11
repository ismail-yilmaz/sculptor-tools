#include "Demo.h"

using namespace Upp;

ObjectViewer::ObjectViewer()
{
    WantFocus();
    files = { "teapot.obj", "skull.obj", "bunny.obj" };

	SetTimeCallback(-16, [=] {
	    bool redraw = false;
	
	    if(abs(goalrot.x - rot.x) > 0.001f
	    || abs(goalrot.y - rot.y) > 0.001f
	    || abs(goal - zoom) > 0.001f
	    || abs(goalpan.x - pan.x) > 0.001f
	    || abs(goalpan.y - pan.y) > 0.001f) {
	        rot += (goalrot - rot) * 0.15f;
	
	        if(panning)
	            pan = goalpan;
	        else
	            pan += (goalpan - pan) * 0.45f;
	
	        zoom += (goal - zoom) * 0.45f;
	        redraw = true;
	    }
	
	    if(mousenorm != prevmousenorm) {
	        prevmousenorm = mousenorm;
	        redraw = true;
	    }
	
	    if(redraw)
	        Refresh();
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

    gls.Cull(!wireframe && !xray)
    .Wireframe(wireframe)
    .Shading(true)
    .Bilinear(bilinear)
    .MipMap(mipmap)
    .AlphaBlend(alphablend);

    if(isometric) {
        gls.Isometric(zoom, sz.cx / sz.cy, -1000.0f, 1000.0f);
        gls.LookAt(Point3D(pan.x, pan.y, 0.0f), Point3D(pan.x, pan.y, -1.0f), Point3D(0, 1, 0));
    }
    else {
        gls.Perspective(M_PI / 3.0f, sz.cx / sz.cy, 0.1f, 1000.0f);
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

    gls.Uniform("uMouseUV", Point3D(mousenorm.x, mousenorm.y, 0.0f));
    gls.Uniform("uResolution", Point3D(sz.cx, sz.cy, 0.0f));

    AddOverlay(gls);

    gls.Rotate(rot.x, rot.y, 0);

    gls.Render(model);
}

void ObjectViewer::AddOverlay(GLScene& gls)
{
    WString txt = Format(
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
        "[L] Lights          %d",
        files[modelindex],
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

void ObjectViewer::MiddleDown(Point p, dword keyflags)
{
    SetFocus();
    panmouse = p;
    panning = true;
    SetCapture();
}

void ObjectViewer::MiddleUp(Point p, dword keyflags)
{
    pan = goalpan;
    panning = false;
    ReleaseCapture();
}

void ObjectViewer::MouseWheel(Point p, int zdelta, dword keyflags)
{
    goal = max(0.5f, goal - (zdelta / 100.0f));
}

void ObjectViewer::MouseMove(Point p, dword keyflags)
{
    if(dragging) {
        Pointf d((float)(p.x - mouse.x), (float)(p.y - mouse.y));
        goalrot.y -= d.x * 0.15f;
        goalrot.x += d.y * 0.15f;
        mouse = p;
    }

    Size sz = GetSize();

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
            const float pitch = 35.264f * (float)(M_PI / 180.0);
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

    if(sz.cx > 0 && sz.cy > 0) {
        mousenorm.x = (float)p.x / sz.cx;
        mousenorm.y = 1.0f - ((float)p.y / sz.cy);
    }
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
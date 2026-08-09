#include "Labyrinth.h"

using namespace Upp;

LabyrinthGame::LabyrinthGame()
: boardtiltx(0)
, boardtilty(0)
, ballrotx(0)
, ballroty(0)
, camdist(20.0f)
, camyaw(0.0f)
, campitch(1.38f)
, tiltleft(false)
, tiltright(false)
, tiltup(false)
, tiltdown(false)
, camleft(false)
, camright(false)
, zoomin(false)
, zoomout(false)
, mousedrag(false)
{
	WantFocus();
	Title("Labyrinth Game (Soft-3D version, using Sculptor)");
	Sizeable().Zoomable().CenterScreen().SetRect(0, 0, 1024, 768);

	BuildModels();
	BuildMap();
	SetTimeCallback(-16, [this] { Update(); });
}

void LabyrinthGame::BuildModels()
{
	ballmodel.SetDefaultColor(Yellow());
	ballmodel.AddSphere(0.5f, 16, 16);

	boardmodel.Clear();
	boardmodel.SetDefaultColor(Gray());
	boardmodel.AddBox({ -10, -10, -0.5f, 10, 10, 0 });

	wallmodel.SetDefaultColor(Color(80, 80, 80));
	wallmodel.AddBox({ -0.5f, -0.5f, 0, 0.5f, 0.5f, 1.0f });

	goalmodel.SetDefaultColor(Red());
	goalmodel.AddBox({ -0.8f, -0.8f, 0, 0.8f, 0.8f, 0.2f });
}

void LabyrinthGame::BuildMap()
{
	walls.Clear();

	static const Vector<String> map = {
		"####################",
		"#S        #        #",
		"# ##### # ##### ## #",
		"# #   # #     #    #",
		"# # # ##### # #### #",
		"#   #     # #      #",
		"##### ### # ###### #",
		"#     #   #        #",
		"# ##### ##### #### #",
		"# #     #     #    #",
		"# # ##### ##### ## #",
		"# #     #   #      #",
		"# ##### ### ###### #",
		"# #     #          #",
		"# # ##### ######## #",
		"# # #     #        #",
		"# # ##### # ###### #",
		"# #       #        #",
		"# ############### G#",
		"####################"
	};

	float cellsize = 1.0f;
	float startx = -10.0f;
	float starty = 10.0f;

	for(int r = 0; r < map.GetCount(); r++) {
		const String& row = map[r];
		for(int c = 0; c < row.GetCount(); c++) {
			int chr = row[c];
			float x = startx + c * cellsize + cellsize * 0.5f;
			float y = starty - r * cellsize - cellsize * 0.5f;
			if(chr == '#') {
				walls.Add() = Box3D(Point3D(x - cellsize * 0.5f, y - cellsize * 0.5f, 0),
									Point3D(x + cellsize * 0.5f, y + cellsize * 0.5f, 1.0f));
			}
			else
			if(chr == 'S') {
				ballpos = Point3D(x, y, 0.5f);
				ballvel = Point3D(0, 0, 0);
			}
			else
			if(chr == 'G')
				goalpos = Point3D(x, y, 0);
		}
	}

}

void LabyrinthGame::Update()
{
	float dt = 0.016f;
	float tiltspeed = 1.5f;
	float maxtilt = 0.4f;

	camyaw += (camright - camleft) * 1.5f * dt;
	camdist = clamp(camdist + (zoomout - zoomin) * 15.0f * dt, 10.0f, 50.0f);

	float tiltforward = (tiltup - tiltdown) * tiltspeed * dt;
	float tiltside    = (tiltleft - tiltright) * tiltspeed * dt;

	boardtiltx += tiltforward * cos(camyaw) - tiltside * sin(camyaw);
	boardtilty += tiltforward * sin(camyaw) + tiltside * cos(camyaw);

	if(!tiltup && !tiltdown && !tiltleft && !tiltright && !mousedrag) {
		boardtiltx *= 0.92f;
		boardtilty *= 0.92f;
	}

	boardtiltx = clamp(boardtiltx, -maxtilt, maxtilt);
	boardtilty = clamp(boardtilty, -maxtilt, maxtilt);

	float gravity = 120.0f;
	ballvel.x += sin(boardtilty) * gravity * dt;
	ballvel.y -= sin(boardtiltx) * gravity * dt;

	ballvel.x *= 0.98f;
	ballvel.y *= 0.98f;

	float oldx = ballpos.x;
	float oldy = ballpos.y;

	ballpos.x += ballvel.x * dt;
	ballpos.y += ballvel.y * dt;

	float visualradius = 0.5f;
	float collisionradius = 0.4f;

	// Iterative geometric physics resolution for smooth sliding and corner bouncing
	for(int iter = 0; iter < 3; iter++) {
		for(const auto& wall : walls) {
			float cx = clamp(ballpos.x, wall.lo.x, wall.hi.x);
			float cy = clamp(ballpos.y, wall.lo.y, wall.hi.y);
			float dx = ballpos.x - cx;
			float dy = ballpos.y - cy;
			float distsq = sqr(dx) + sqr(dy);

			if(distsq < sqr(collisionradius)) {
				float dist = sqrt(distsq);
				if(dist > 1e-4f) {
					float overlap = collisionradius - dist;
					float nx = dx / dist;
					float ny = dy / dist;

					// Push the ball exactly out of the wall
					ballpos.x += nx * overlap;
					ballpos.y += ny * overlap;

					// Reflect velocity along the collision normal
					float dot = ballvel.x * nx + ballvel.y * ny;
					if(dot < 0) {
						float bounce = 0.3f; // Dampened bounce
						ballvel.x -= (1.0f + bounce) * dot * nx;
						ballvel.y -= (1.0f + bounce) * dot * ny;
					}
				}
			}
		}
	}

	ballpos.z = 0.5f;

	ballrotx -= (ballpos.y - oldy) / visualradius;
	ballroty += (ballpos.x - oldx) / visualradius;

	float boardlimit = 9.5f - visualradius;

	if(abs(ballpos.x) > boardlimit) {
		ballpos.x = clamp(ballpos.x, -boardlimit, boardlimit);
		ballvel.x *= -0.4f;
	}
	if(abs(ballpos.y) > boardlimit) {
		ballpos.y = clamp(ballpos.y, -boardlimit, boardlimit);
		ballvel.y *= -0.4f;
	}

	if(ballpos.Distance(goalpos) < 1.0f) {
		BuildMap();
		ballvel = Point3D(0, 0, 0);
	}

	Refresh();
}

void LabyrinthGame::Paint(Draw& w)
{
	Sizef sz = GetSize();
	if(sz.cy == 0)
		return;

	Sculptor renderer(sz, Black());

	renderer.Perspective(M_PI / 3.0f, sz.cx / sz.cy, 0.1f, 100.0f);

	Point3D camera = {
		 camdist * cos(campitch) * sin(camyaw),
		-camdist * cos(campitch) * cos(camyaw),
		 camdist * sin(campitch)
	};

	renderer.LookAt(camera, Point3D(0, 0, 0), Point3D(0, 0, 1));

	renderer.ClearLights();
	renderer.AddLight(Point3D(-5, -10, 15), White());
	renderer.AddLight(Point3D(5, 10, 15), Gray());
	renderer.AddLight(Point3D(0, 0, -5), LtGray());

	renderer.Shading();
	renderer.Cull();
	renderer.Co(); // Parallelize rasterization

	Sculptor::Scope __scene(renderer);
	renderer.Rotate(boardtiltx, boardtilty, 0);
	renderer.Render(boardmodel);
	{
		Sculptor::Scope __goal(renderer);
		renderer.Translate(goalpos);
		renderer.Render(goalmodel);
	}
	for(const auto& wall : walls) {
		Sculptor::Scope __wall(renderer);
		renderer.Translate(wall.Center());
		renderer.Scale(wall.Size());
		renderer.Render(wallmodel);
	}
	{
		Sculptor::Scope __ball(renderer);
		renderer.Translate(ballpos);
		renderer.Rotate(ballrotx, ballroty, 0);
		renderer.Render(ballmodel);
	}
	renderer.Rasterize();
	w.DrawImage(0, 0, renderer.GetImage());
}

bool LabyrinthGame::Key(dword key, int count)
{
	bool down = !(key & K_KEYUP);
	dword k = key & ~(K_KEYUP | K_ALT | K_CTRL | K_SHIFT);

	if(key & K_CTRL) {
		switch(k) {
			case K_LEFT:
				camleft = down;
				break;
			case K_RIGHT:
				camright = down;
				break;
			case K_UP:
				zoomin = down;
				break;
			case K_DOWN:
				zoomout = down;
				break;
			default:
				break;
		}
	}
	else {
		switch(k) {
			case K_LEFT:
				tiltleft = down;
				break;
			case K_RIGHT:
				tiltright = down;
				break;
			case K_UP:
				tiltdown = down;
				break;
			case K_DOWN:
				tiltup = down;
				break;
			default:
				break;
		}
	}

	Refresh();
	return true;
}

void LabyrinthGame::MouseWheel(Point p, int zdelta, dword keyflags)
{
	camdist = clamp(camdist - zdelta * 0.005f, 10.0f, 50.0f);
	Refresh();
}

void LabyrinthGame::LeftDown(Point p, dword keyflags)
{
	mousedrag = true;
	mousep = p;
	SetCapture();
}

void LabyrinthGame::LeftUp(Point p, dword keyflags)
{
	mousedrag = false;
	ReleaseCapture();
}

void LabyrinthGame::MouseMove(Point p, dword keyflags)
{
	if(mousedrag) {
		float dx = (float)(p.x - mousep.x);
		float dy = (float)(p.y - mousep.y);
		mousep = p;

		float sensitivity = 0.002f;

		float tiltforward = dy * sensitivity;
		float tiltside    = -dx * sensitivity;

		boardtiltx += tiltforward * cos(camyaw) - tiltside * sin(camyaw);
		boardtilty += tiltforward * sin(camyaw) + tiltside * cos(camyaw);
	}
}

GUI_APP_MAIN
{
	LabyrinthGame().Run();
}
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

	// Iterate through the grid and convert characters into 3D world coordinates
	for(int r = 0; r < map.GetCount(); r++) {
		const String& row = map[r];
		int startc = -1; // Tracks the start index of a contiguous wall segment
		for(int c = 0; c <= row.GetCount(); c++) {
			bool iswall = (c < row.GetCount() && row[c] == '#');
			if(iswall && startc == -1) {
				// Start a new wall segment
				startc = c;
			}
			else
			if(!iswall && startc != -1) {
				// End of the segment: create one continuous Box3D
				float xlo = startx + startc * cellsize;
				float xhi = startx + c * cellsize;
				float yhi = starty - r * cellsize;
				float ylo = yhi - cellsize;
				walls.Add() = Box3D(Point3D(xlo, ylo, 0), Point3D(xhi, yhi, 1.0f));
				// Reset tracker
				startc = -1;
			}

			if(c < row.GetCount()) {
				float px = startx + c * cellsize + cellsize * 0.5f;
				float py = starty - r * cellsize - cellsize * 0.5f;
				if(row[c] == 'S') {
					ballpos = Point3D(px, py, 0.5f);
					ballvel = Point3D(0, 0, 0);
				}
				else
				if(row[c] == 'G') {
					goalpos = Point3D(px, py, 0.0f);
				}
			}
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

	renderer.Co(); // Parallelize
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

	RenderBoard(renderer);
	RenderGoal(renderer);
	RenderWalls(renderer);
	RenderBall(renderer);

	renderer.Rasterize();
	w.DrawImage(0, 0, renderer.GetImage());
}

void LabyrinthGame::RenderBoard(Sculptor& sc)
{
	sc.Rotate(boardtiltx, boardtilty, 0);
	sc.Render(boardmodel);
}

void LabyrinthGame::RenderGoal(Sculptor& sc)
{
	Sculptor::Scope __(sc);
	sc.Translate(goalpos);
	sc.Render(goalmodel);
}

void LabyrinthGame::RenderWalls(Sculptor& sc)
{
	for(const auto& wall : walls) {
		Sculptor::Scope __(sc);
		sc.Translate(wall.Center());
		sc.Scale(wall.Size());
		sc.Render(wallmodel);
	}
}

void LabyrinthGame::RenderBall(Sculptor& sc)
{
	Sculptor::Scope __(sc);
	sc.Translate(ballpos);
	sc.Rotate(ballrotx, ballroty, 0);
	sc.Render(ballmodel);
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
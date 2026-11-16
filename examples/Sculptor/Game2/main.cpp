#include "Asteroids.h"

using namespace Upp;

inline float randf() { return (Random() % 10000) / 10000.0f; }

AsteroidsEngine::AsteroidsEngine()
: targetindex(-1)
, shipangle(0)
, shootcooldown(0)
, turningleft(false)
, turningright(false)
, thrusting(false)
, shooting(false)
{
    Title("Asteroids Clone");
    Sizeable().Zoomable().CenterScreen().SetRect(0, 0, 1024, 768);
    BackPaint();

    BuildModels();
    SpawnAsteroids(8);
    SetTimeCallback(-16, [=] { Update(); });
}

void AsteroidsEngine::BuildModels()
{
    shipmodel.SetDefaultColor(White());
    shipmodel.AddBox({ -0.6f, -1.0f, -0.2f, 0.6f, -0.2f, 0.2f });
    shipmodel.AddBox({ -0.2f, -1.0f, -0.4f, 0.2f,  1.5f, 0.4f });

    asteroidmodel.SetDefaultColor(Gray());
    asteroidmodel.AddSphere(1.0f, 8, 8);

    targetmodel.SetDefaultColor(LtRed());
    targetmodel.AddSphere(1.0f, 8, 8);

    bulletmodel.SetDefaultColor(Yellow());
    bulletmodel.AddBox({ -0.1f, -0.1f, -0.1f, 0.1f, 0.1f, 0.1f });
}

void AsteroidsEngine::SpawnAsteroids(int count)
{
    for(int i = 0; i < count; i++) {
        Asteroid& a = asteroids.Add();
        a.position = Pointf((randf() - 0.5f) * 30.0f, (randf() - 0.5f) * 20.0f);
        a.velocity = Pointf((randf() - 0.5f) * 10.0f, (randf() - 0.5f) * 10.0f);

        Point3D axis(randf() - 0.5f, randf() - 0.5f, randf() - 0.5f);
        a.rotationaxis = Nvl(axis.Normalized(), Point3D(0, 0, 1));

        a.spinrate = randf() * 3.0f;
        a.angle = 0;
        a.radius = 2.0f;
    }
}

bool AsteroidsEngine::WrapPosition(Pointf& p)
{
    Sizef sz = GetSize();
    if(sz.cy == 0)
        return false;

    float ratio = sz.cx / sz.cy;
    float viewh = 15.0f;
    float vieww = viewh * ratio;

    float bx = vieww + 1.0f;
    float by = viewh + 1.0f;
    bool wrapped = false;

    if(p.x > bx) {
        p.x = -bx;
        wrapped = true;
    }
    else
    if(p.x < -bx) {
        p.x = bx;
        wrapped = true;
    }

    if(p.y > by) {
        p.y = -by;
        wrapped = true;
    }
    else
    if(p.y < -by) {
        p.y = by;
        wrapped = true;
    }

    return wrapped;
}

Matrix4D AsteroidsEngine::GetAsteroidTransform(const Asteroid& a) const
{
	// Unlike SculptorCtrl, we use row-major matrix for Sculptor (Matrix4D)
    return Matrix4D::Scale(a.radius)
         * Matrix4D::Rotation(a.rotationaxis.x * a.angle, a.rotationaxis.y * a.angle, a.rotationaxis.z * a.angle)
         * Matrix4D::Translation(a.position.x, a.position.y, 0.0f);
}

void AsteroidsEngine::UpdateTarget()
{
    Pointf dir = Polar(shipangle + (float)M_PI_2);
    Ray3D ray(Point3D(shipposition.x, shipposition.y, 0.0f), Point3D(dir.x, dir.y, 0.0f));

    targetindex = -1;
    targethit.Clear();

    for(int i = 0; i < asteroids.GetCount(); i++) {
        const Asteroid& a = asteroids[i];
        Ray3D::Hit hit = asteroidmodel.Intersect(ray, GetAsteroidTransform(a));
        if(hit < targethit) {
            targethit = hit;
            targetindex = i;
        }
    }
}

void AsteroidsEngine::Update()
{
    float dt = 0.016f;

    if(turningleft)
        shipangle += 4.0f * dt;
    
    if(turningright)
        shipangle -= 4.0f * dt;
    
    if(thrusting)
        shipvelocity = Polar(shipvelocity, 20.0f * dt, shipangle + (float)M_PI_2);

    shipvelocity *= 0.99f;
    shipposition += shipvelocity * dt;
    WrapPosition(shipposition);

    if(shootcooldown > 0)
        shootcooldown -= dt;

    if(shooting && shootcooldown <= 0) {
        Bullet& b = bullets.Add();
        b.position = shipposition;
        b.velocity = Polar(shipvelocity, 35.0f, shipangle + (float)M_PI_2);
        b.life = 1.5f;
        shootcooldown = 0.15f;
    }

    Vector<Pointf> prevbulletpos;
    Vector<bool> bulletwrapped;
    prevbulletpos.SetCount(bullets.GetCount());
    bulletwrapped.SetCount(bullets.GetCount());

    for(int i = bullets.GetCount() - 1; i >= 0; i--) {
        Pointf oldpos = bullets[i].position;
        bullets[i].position += bullets[i].velocity * dt;
        bool wrapped = WrapPosition(bullets[i].position);
        bullets[i].life -= dt;

        if(bullets[i].life <= 0) {
            bullets.Remove(i);
            prevbulletpos.Remove(i);
            bulletwrapped.Remove(i);
        }
        else {
            prevbulletpos[i] = oldpos;
            bulletwrapped[i] = wrapped;
        }
    }

    for(int i = asteroids.GetCount() - 1; i >= 0; i--) {
        Asteroid& a = asteroids[i];
        a.position += a.velocity * dt;
        WrapPosition(a.position);
        a.angle += a.spinrate * dt;

        Matrix4D xform = GetAsteroidTransform(a);
        bool hit = false;

        for(int j = bullets.GetCount() - 1; j >= 0; j--) {
            const Bullet& b = bullets[j];
            if(!bulletwrapped[j]) {
                Pointf dp = b.position - prevbulletpos[j];
                float len = Length(dp);
                float broad = a.radius + len;
                if(SquaredDistance(a.position, prevbulletpos[j]) <= broad * broad && len > 0.0001f) {
                    Ray3D ray(Point3D(prevbulletpos[j].x, prevbulletpos[j].y, 0.0f), Point3D(dp.x / len, dp.y / len, 0.0f));
                    Ray3D::Hit rhit = asteroidmodel.Intersect(ray, xform);
                    if(rhit && rhit.distance <= len) {
                        bullets.Remove(j);
                        prevbulletpos.Remove(j);
                        bulletwrapped.Remove(j);
                        hit = true;
                        break;
                    }
                }
            }
            else if(SquaredDistance(a.position, b.position) < (a.radius * a.radius)) {
                bullets.Remove(j);
                prevbulletpos.Remove(j);
                bulletwrapped.Remove(j);
                hit = true;
                break;
            }
        }

        if(hit) {
            float oldrad = a.radius;
            Pointf oldpos = a.position;
            Pointf oldvel = a.velocity;
            asteroids.Remove(i);
            if(oldrad > 0.75f) {
                for(int k = 0; k < 2; k++) {
                    Asteroid& child = asteroids.Add();
                    child.position = oldpos;
                    child.radius = oldrad * 0.5f;
                    child.velocity = oldvel + Pointf(randf() - 0.5f, randf() - 0.5f) * 15.0f;

                    Point3D axis(randf() - 0.5f, randf() - 0.5f, randf() - 0.5f);
                    child.rotationaxis = Nvl(axis.Normalized(), Point3D(0, 0, 1));

                    child.spinrate = (randf() + 1.0f) * 3.0f;
                    child.angle = 0;
                }
            }
            continue;
        }

        float dist = Distance(shipposition, a.position);
        float combined = a.radius + 0.6f;

        if(dist < combined && dist > 0.001f) {
            Pointf n = Normalize(shipposition - a.position);
            float overlap = combined - dist;

            shipposition += n * (overlap * 0.5f);
            a.position -= n * (overlap * 0.5f);

            Pointf dv = shipvelocity - a.velocity;
            float dot = dv.x * n.x + dv.y * n.y;

            if(dot < 0) {
                float mship = 1.0f;
                float mast = a.radius;
                float restitution = 0.8f;
                float impulse = -(1.0f + restitution) * dot / (1.0f / mship + 1.0f / mast);
                shipvelocity += n * (impulse / mship);
                a.velocity -= n * (impulse / mast);
            }
        }
    }

    float speed = Length(shipvelocity);
    if(speed > 40.0f)
        shipvelocity *= (40.0f / speed);

    WrapPosition(shipposition);

    if(asteroids.GetCount() == 0)
        SpawnAsteroids(8);

    UpdateTarget();
    Refresh();
}

void AsteroidsEngine::Paint(Draw& w)
{
    Sizef sz = GetSize();
    if(sz.cx <= 0 || sz.cy <= 0)
        return;
    
    Sculptor renderer(sz, Black());

    float h = 15.0f;
    float v = h * (sz.cx / sz.cy);

    renderer.Orthographic(Rectf(-v, -h, v, h), -10.0f, 10.0f);
    renderer.LookAt(Point3D(0, 0, 5), Point3D(0, 0, 0), Point3D(0, 1, 0));
    
    renderer.ClearLights();
    renderer.AddLight(Point3D(-1, -1, 1), Gray());
    renderer.AddLight(Point3D(1, 1, 1), Gray());
    renderer.AddLight(Point3D(0, 0, -1), LtGray());
    
    renderer.Shading();
    renderer.Cull(false);

    RenderShip(renderer);
    RenderBullet(renderer);
    RenderAsteroids(renderer);
    
    renderer.Rasterize();
    w.DrawImage(0, 0, renderer.GetImage());
}

void AsteroidsEngine::RenderShip(Sculptor& gls)
{
    Sculptor::Scope __(gls);
    gls.Translate(shipposition.x, shipposition.y, 0);
    gls.Rotate(0, 0, shipangle);
    gls.Render(shipmodel);
}

void AsteroidsEngine::RenderBullet(Sculptor& gls)
{
    for(const Bullet& b : bullets) {
        Sculptor::Scope __(gls);
        gls.Translate(b.position.x, b.position.y, 0);
        gls.Render(bulletmodel);
    }
}

void AsteroidsEngine::RenderAsteroids(Sculptor& gls)
{
    for(int i = 0; i < asteroids.GetCount(); i++) {
        const Asteroid& a = asteroids[i];
        Sculptor::Scope __(gls);
        gls.Translate(a.position.x, a.position.y, 0);
        gls.Rotate(a.rotationaxis.x * a.angle, a.rotationaxis.y * a.angle, a.rotationaxis.z * a.angle);
        gls.Scale(a.radius);
        gls.Render(i == targetindex ? targetmodel : asteroidmodel);
    }
}

bool AsteroidsEngine::Key(dword key, int count)
{
    bool down = !(key & K_KEYUP);

    switch(key & ~K_KEYUP) {
    case K_LEFT:
        turningleft = down;
        break;
    case K_RIGHT:
        turningright = down;
        break;
    case K_UP:
        thrusting = down;
        break;
    case K_SPACE:
        shooting = down;
        break;
    default:
        return false;
    }

    return true;
}

GUI_APP_MAIN
{
    AsteroidsEngine game;
    game.Run();
}
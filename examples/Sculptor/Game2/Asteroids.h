#ifndef _SculptorCtrl_Asteroids_h_
#define _SculptorCtrl_Asteroids_h_

#include <CtrlLib/CtrlLib.h>
#include <Sculptor/Sculptor.h>

using namespace Upp;

struct AsteroidsEngine : TopWindow {

	AsteroidsEngine();
	
	void             BuildModels();
	void             SpawnAsteroids(int count);
	void             WrapPosition(Pointf& p);
	void             Update();
	
	void             Paint(Draw& w) final;
	void             RenderShip(Sculptor& gls);
	void             RenderBullet(Sculptor& gls);
	void             RenderAsteroids(Sculptor& gls);
	
	bool             Key(dword key, int count) final;
	
    struct Bullet : Moveable<Bullet> {
        Pointf       position;
        Pointf       velocity;
        float        life;
    };

    struct Asteroid : Moveable<Asteroid> {
        Pointf       position;
        Pointf       velocity;
        Point3D      rotationaxis;
        float        angle;
        float        spinrate;
        float        radius;
    };

    Model3D          shipmodel;
    Model3D          asteroidmodel;
    Model3D          bulletmodel;

    Vector<Bullet>   bullets;
    Vector<Asteroid> asteroids;

    Pointf           shipposition;
    Pointf           shipvelocity;
    float            shipangle;
    float            shootcooldown;
                     
    bool             turningleft:1;
    bool             turningright:1;
    bool             thrusting:1;
    bool             shooting:1;
};


#endif

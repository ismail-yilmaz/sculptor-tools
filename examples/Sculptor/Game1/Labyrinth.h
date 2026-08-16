#ifndef _LabyrinthGame_h_
#define _LabyrinthGame_h_

#include <CtrlLib/CtrlLib.h>
#include <Sculptor/Sculptor.h>

namespace Upp {

struct LabyrinthGame : TopWindow {
    LabyrinthGame();
    
    void    BuildModels();
    void    BuildMap();
    void    Update();
    
    void    Paint(Draw& w) final;
    void    RenderBoard(Sculptor& sc);
    void    RenderGoal(Sculptor& sc);
    void    RenderWalls(Sculptor& sc);
    void    RenderBall(Sculptor& sc);

    bool    Key(dword key, int count) final;
    void    MouseWheel(Point p, int zdelta, dword keyflags) final;
    void    LeftDown(Point p, dword keyflags) final;
    void    LeftUp(Point p, dword keyflags) final;
    void    MouseMove(Point p, dword keyflags) final;

    Model3D ballmodel;
    Model3D boardmodel;
    Model3D wallmodel;
    Model3D goalmodel;

    Point3D ballpos;
    Point3D ballvel;

    float   boardtiltx;
    float   boardtilty;
    float   ballrotx;
    float   ballroty;
    
    // Camera state variables
    float   camdist;
    float   camyaw;
    float   campitch;

    bool    tiltleft:1;
    bool    tiltright:1;
    bool    tiltup:1;
    bool    tiltdown:1;
    bool    camleft:1;
    bool    camright:1;
    bool    zoomin:1;
    bool    zoomout:1;
    bool    mousedrag:1;
    
    Point   mousep;
    
    Vector<Box3D> walls;
    Point3D       goalpos;
};

}

#endif
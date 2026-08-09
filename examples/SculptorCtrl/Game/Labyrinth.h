#ifndef _LabyrinthGame_h_
#define _LabyrinthGame_h_

#include <CtrlLib/CtrlLib.h>
#include <SculptorCtrl/SculptorCtrl.h>

namespace Upp {

struct LabyrinthScene : SculptorCtrl {
    LabyrinthScene();
    
    void    BuildModels();
    void    BuildMap();
    void    Update();
    
    void    Render(GLScene& gls) final;

    bool    Key(dword key, int count) final;
    void    MouseWheel(Point p, int zdelta, dword keyflags) final;
    void    LeftDown(Point p, dword keyflags) final;
    void    LeftUp(Point p, dword keyflags) final;
    void    MouseMove(Point p, dword keyflags) final;

    ModelGL ballmodel;
    ModelGL boardmodel;
    ModelGL wallmodel;
    ModelGL goalmodel;

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
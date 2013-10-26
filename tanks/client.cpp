#include "world.h"

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

class TankHandler : public osgGA::GUIEventHandler 
{
public: 

  TankHandler(World* world)
    : _world(world)
  {}

  ~TankHandler() {}

  bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
  {
    osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
    if (!viewer) return false;

    switch(ea.getEventType())
    {
      case(osgGA::GUIEventAdapter::KEYDOWN):
        _world->GetKeyMap()[ea.getKey()] = 1;
      break;
      case(osgGA::GUIEventAdapter::KEYUP):
        _world->GetKeyMap()[ea.getKey()] = 2;
      break;
    }

    return false;
  }
protected:
  World* _world;
};

void main()
{
  osgViewer::Viewer viewer;

  osgViewer::StatsHandler* sh = new osgViewer::StatsHandler;
  sh ->setKeyEventTogglesOnScreenStats(osgGA::GUIEventAdapter::KEY_F1);
  viewer.addEventHandler(sh);

  viewer.setThreadSafeReferenceCounting(true);
  viewer.setUpViewInWindow(200, 200, 1280, 720);

  osg::Camera* camera = new osg::Camera;
  camera->setProjectionMatrix(osg::Matrix::ortho2D(0, 1280, 0, 720));
  camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
  camera->setViewMatrix(osg::Matrix::identity());
  camera->setClearMask(GL_DEPTH_BUFFER_BIT);
  camera->setRenderOrder(osg::Camera::POST_RENDER);
  camera->setAllowEventFocus(false);

  World* world = new World();
  camera->addChild(world);
  viewer.setSceneData(camera);

  viewer.addEventHandler(new TankHandler(world));

  viewer.run();
}
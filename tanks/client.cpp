#include <osgViewer/Viewer>
#include <osg/MatrixTransform>
#include <osg/Texture2D>
#include <osgDB/ReadFile>

class Tank : public osg::MatrixTransform
{
public:
  Tank(osg::Geode* tankGeod)
  {
    addChild(tankGeod);
    _angle = 0;
    _up.set(0.0, 1.0, 0.0);
    _position.set(10.0, 10.0, 0.0);
    setMatrix(osg::Matrix::translate(_position));
  }

  void Up()
  {
    _position += _up;
    setMatrix(osg::Matrix::rotate(osg::DegreesToRadians(_angle), 0.0, 0.0, 1.0) * osg::Matrix::translate(_position));
  }

  void Down()
  {
    _position -= _up;
    setMatrix(osg::Matrix::rotate(osg::DegreesToRadians(_angle), 0.0, 0.0, 1.0) * osg::Matrix::translate(_position));
  }

  void Left()
  {
    _angle += 2.0;
    _up = osg::Vec3d(0.0, 1.0, 0.0) * osg::Matrix::rotate(osg::DegreesToRadians(_angle), 0.0, 0.0, 1.0);
    setMatrix(osg::Matrix::rotate(osg::DegreesToRadians(_angle), 0.0, 0.0, 1.0) * osg::Matrix::translate(_position));
  }

  void Right()
  {
    _angle -= 2.0;
    _up = osg::Vec3d(0.0, 1.0, 0.0) * osg::Matrix::rotate(osg::DegreesToRadians(_angle), 0.0, 0.0, 1.0);
    setMatrix(osg::Matrix::rotate(osg::DegreesToRadians(_angle), 0.0, 0.0, 1.0) * osg::Matrix::translate(_position));
  }

  ~Tank()
  {}
protected:
  osg::Vec3d _position;
  osg::Vec3d _up;
  double _angle;
};

class TankHandler : public osgGA::GUIEventHandler 
{
public: 

  TankHandler(Tank* tank)
    : _tank(tank)
    , _left(false)
    , _right(false)
    , _up(false)
    , _down(false)
  {}

  ~TankHandler() {}

  bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
  {
    osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
    if (!viewer) return false;

    switch(ea.getEventType())
    {
      case(osgGA::GUIEventAdapter::KEYDOWN):
      {
        if     (ea.getKey() == osgGA::GUIEventAdapter::KEY_Up)
          _up = true;
        else if(ea.getKey() == osgGA::GUIEventAdapter::KEY_Down)
          _down = true;
        else if(ea.getKey() == osgGA::GUIEventAdapter::KEY_Left)
          _left = true;
        else if(ea.getKey() == osgGA::GUIEventAdapter::KEY_Right)
          _right = true;
        else if(ea.getKey() == osgGA::GUIEventAdapter::KEY_Space)
        {
        }
      }
      break;
      case(osgGA::GUIEventAdapter::KEYUP):
      {
        if     (ea.getKey() == osgGA::GUIEventAdapter::KEY_Up)
          _up = false;
        else if(ea.getKey() == osgGA::GUIEventAdapter::KEY_Down)
          _down = false;
        else if(ea.getKey() == osgGA::GUIEventAdapter::KEY_Left)
          _left = false;
        else if(ea.getKey() == osgGA::GUIEventAdapter::KEY_Right)
          _right = false;
        else if(ea.getKey() == osgGA::GUIEventAdapter::KEY_Space)
        {
        }
      }
      break;
    }

    if(_up)
      _tank->Up();

    if(_down)
      _tank->Down();

    if(_left)
      _tank->Left();

    if(_right)
      _tank->Right();

    return false;
  }
protected:
  Tank* _tank;
  bool _left, _right, _up, _down;
};

class World : public osg::Group
{
public:
  World() : osg::Group()
  {
    osg::Geometry* tank = osg::createTexturedQuadGeometry(osg::Vec3d(-20.0, -30.0, 0.0), osg::Vec3d(40.0, 0.0, 0.0), osg::Vec3d(0.0, 60.0, 0.0), 1.0, 0.0, 0.0, 1.0);

    osg::Image* tankImg = osgDB::readImageFile("res/tank.jpg");
    osg::Texture2D* tankTex = new osg::Texture2D(tankImg);
    tankTex->setFilter(osg::Texture::MIN_FILTER , osg::Texture::LINEAR);
    tankTex->setFilter(osg::Texture::MAG_FILTER , osg::Texture::LINEAR);
    tank->getOrCreateStateSet()->setTextureAttributeAndModes(0, tankTex, osg::StateAttribute::ON);
    tank->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED);

    _tankGeode = new osg::Geode();
    _tankGeode->addDrawable(tank);

    addChild(_tank = new Tank(_tankGeode));
  }

  Tank* GetTank() { return _tank; }

  ~World()
  {
  }
protected:
  osg::ref_ptr<osg::Geode> _tankGeode;
  Tank* _tank;
};

void main()
{
  osgViewer::Viewer viewer;
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

  viewer.addEventHandler(new TankHandler(world->GetTank()));

  viewer.run();
}
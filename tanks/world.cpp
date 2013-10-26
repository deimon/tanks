#include "world.h"

#include <osg/Texture2D>
#include <osg/NodeCallback>
#include <osgDB/ReadFile>
#include <osgViewer/ViewerEventHandlers>

Tank::Tank(osg::Geode* tankGeod, osg::Vec3d position)
: _position(position)
{
  addChild(tankGeod);
  _angle = 0;
  _up.set(0.0, 1.0, 0.0);
  setMatrix(osg::Matrix::translate(_position));
}

void Tank::Up()
{
  _position += _up;
  setMatrix(osg::Matrix::rotate(osg::DegreesToRadians(_angle), 0.0, 0.0, 1.0) * osg::Matrix::translate(_position));
}

void Tank::Down()
{
  _position -= _up;
  setMatrix(osg::Matrix::rotate(osg::DegreesToRadians(_angle), 0.0, 0.0, 1.0) * osg::Matrix::translate(_position));
}

void Tank::Left()
{
  _angle += 2.0;
  _up = osg::Vec3d(0.0, 1.0, 0.0) * osg::Matrix::rotate(osg::DegreesToRadians(_angle), 0.0, 0.0, 1.0);
  setMatrix(osg::Matrix::rotate(osg::DegreesToRadians(_angle), 0.0, 0.0, 1.0) * osg::Matrix::translate(_position));
}

void Tank::Right()
{
  _angle -= 2.0;
  _up = osg::Vec3d(0.0, 1.0, 0.0) * osg::Matrix::rotate(osg::DegreesToRadians(_angle), 0.0, 0.0, 1.0);
  setMatrix(osg::Matrix::rotate(osg::DegreesToRadians(_angle), 0.0, 0.0, 1.0) * osg::Matrix::translate(_position));
}

class WorldCallback
  : public osg::NodeCallback
{
public:
  WorldCallback(World *world): _world(world) {};
  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
  {
    _world->update(nv->getFrameStamp()->getReferenceTime());
    traverse(node, nv);
  }

private:
  World *_world;
};

World::World()
: osg::Group()
, _colision(false)
, _autopilot(false)
{
  setUpdateCallback(new WorldCallback(this));

  osg::Geometry* tank = osg::createTexturedQuadGeometry(osg::Vec3d(-20.0, -30.0, 0.0),
                                                        osg::Vec3d(40.0, 0.0, 0.0),
                                                        osg::Vec3d(0.0, 60.0, 0.0), 1.0, 0.0, 0.0, 1.0);

  osg::Image* tankImg = osgDB::readImageFile("res/tank.jpg");
  osg::Texture2D* tankTex = new osg::Texture2D(tankImg);
  tankTex->setFilter(osg::Texture::MIN_FILTER , osg::Texture::LINEAR);
  tankTex->setFilter(osg::Texture::MAG_FILTER , osg::Texture::LINEAR);
  tank->getOrCreateStateSet()->setTextureAttributeAndModes(0, tankTex, osg::StateAttribute::ON);
  tank->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED);

  _tankGeode = new osg::Geode();
  _tankGeode->addDrawable(tank);

  addChild(_tank = new Tank(_tankGeode, osg::Vec3d(50.0, 50.0, 0.0)));

  _bloks.push_back(Block(0, 20, 0, 720));
  _bloks.push_back(Block(1260, 1280, 0, 720));
  _bloks.push_back(Block(0, 1280, 0, 20));
  _bloks.push_back(Block(0, 1280, 700, 720));

  _bloks.push_back(Block(200, 500, 180, 200));
  _bloks.push_back(Block(800, 820, 80, 500));

  osg::Geode* bloksGeode = new osg::Geode();
  for(int i = 0; i < _bloks.size(); i++)
  {
    bloksGeode->addDrawable(osg::createTexturedQuadGeometry(osg::Vec3d(_bloks[i]._left, _bloks[i]._bottom, 0.0),
                                                            osg::Vec3d(_bloks[i]._right - _bloks[i]._left, 0.0, 0.0),
                                                            osg::Vec3d(0.0, _bloks[i]._top - _bloks[i]._bottom, 0.0), 1.0, 0.0, 0.0, 1.0));
  }

  addChild(bloksGeode);
}

bool World::collision(osg::Vec3d pos)
{
  for(int i = 0; i < _bloks.size(); i++)
  {
    if(pos.x() >= _bloks[i]._left - 20 && pos.x() <= _bloks[i]._right + 20
    && pos.y() >= _bloks[i]._bottom - 20 && pos.y() <= _bloks[i]._top + 20)
      return true;
  }

  return false;
}

void World::autopilot()
{
  if(_colision)
    _key[osgGA::GUIEventAdapter::KEY_Left] = 1;
  else
    _key[osgGA::GUIEventAdapter::KEY_Left] = 0;
}

void World::update(double time)
{
  if(_key[osgGA::GUIEventAdapter::KEY_Up] == 1)
  {
    _tank->Up();
    if(_colision = collision(_tank->GetPosition()))
      _tank->Down();
  }
  else if(_key[osgGA::GUIEventAdapter::KEY_Up] == 2)
    _key[osgGA::GUIEventAdapter::KEY_Up] = 0;

  if(_key[osgGA::GUIEventAdapter::KEY_Down] == 1)
  {
    _tank->Down();
    if(collision(_tank->GetPosition()))
      _tank->Up();
  }
  else if(_key[osgGA::GUIEventAdapter::KEY_Down] == 2)
    _key[osgGA::GUIEventAdapter::KEY_Down] = 0;

  if(_key[osgGA::GUIEventAdapter::KEY_Left] == 1)
    _tank->Left();

  if(_key[osgGA::GUIEventAdapter::KEY_Right] == 1)
    _tank->Right();

  if(_key[osgGA::GUIEventAdapter::KEY_A] == 2)
  {
    _autopilot = !_autopilot;
    _key[osgGA::GUIEventAdapter::KEY_A] = 0;

    if(_autopilot)
      _key[osgGA::GUIEventAdapter::KEY_Up] = 1;
    else
      _key[osgGA::GUIEventAdapter::KEY_Up] = 0;
  }

  if(_autopilot)
    autopilot();
}
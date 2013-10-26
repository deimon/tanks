#include <osg/MatrixTransform>

class Tank : public osg::MatrixTransform
{
public:
  Tank(osg::Geode* tankGeod, osg::Vec3d position = osg::Vec3d(0.0, 0.0, 0.0));
  ~Tank() {}

  void Up();
  void Down();
  void Left();
  void Right();

  const osg::Vec3d& GetPosition() { return _position; }
  const double& GetAngle() { return _angle; }

protected:
  osg::Vec3d _position;
  osg::Vec3d _up;
  double _angle;
};

struct Block
{
  Block(float left, float right, float bottom, float top)
    : _left(left), _right(right), _bottom(bottom), _top(top)
  {}

  float _left, _right, _top, _bottom;
};

class World : public osg::Group
{
public:
  World();

  ~World() {}

  bool collision(osg::Vec3d pos);

  void autopilot();

  void update(double time);

  Tank* GetTank() { return _tank; }

  std::map<unsigned int, char>& GetKeyMap() { return _key; }

protected:
  std::map<unsigned int, char> _key;
  bool _colision;
  bool _autopilot;

  osg::ref_ptr<osg::Geode> _tankGeode;
  Tank* _tank;

  std::vector<Block> _bloks;
};
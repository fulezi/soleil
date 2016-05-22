
# include <osg/Group>


#include "Level.hpp"

namespace Soleil
{
  const char *NextLevelZone::ClassName = "Soleil:Nextlevelzone";
  
  Level::Level(osg::ref_ptr<osg::Group> root)
    : _root(root)
  {
  }
  
  Level::~Level(void)
  {
  }

  osg::Vec3	Level::startingPosition() const
  {
    // TODO throw if not set
    return _startingPosition;
  }

  osg::Vec3	Level::startingOrientation() const
  {
    // TODO throw if not set
    return _startingOrientation;
  }

  
}  // Soleil

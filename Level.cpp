
# include <osg/Group>


#include "Level.hpp"

namespace Soleil
{
  
  Level::Level()
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

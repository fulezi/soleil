
#ifndef SOLEIL_FLOOR__HPP
#define SOLEIL_FLOOR__HPP

#include <osg/Texture2D>
#include <osg/Geometry>
#include <osgDB/ReadFile>

#include "Mesh.hpp"

namespace Soleil
{
  class Floor : public Mesh
  {
  public:
    Floor(bool );
    virtual ~Floor(void);
    
    
  };

};

#endif

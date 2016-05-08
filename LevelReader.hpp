

#ifndef SOLEIL_LEVELREADER__HPP
#define SOLEIL_LEVELREADER__HPP

#include <string>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgDB/Registry>

#include "Level.hpp"

/**
 * Basic Donjon LevelReader
 */

namespace Soleil
{
  class LevelChunk
  {
  public:
    LevelChunk() {};
    virtual ~LevelChunk(void) {};

    osg::ref_ptr<osg::Geometry>  toGeometry(void) const;
    
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array();
    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
    unsigned int textureSlot = 0;
    std::string texturePath = "media/textures/stone_3_2048x2048.jpg";
  };

  class LevelReader //: public osgDB::ReaderWriter
  {
  public:
    LevelReader();
    virtual ~LevelReader(void);

  public:
    virtual osg::ref_ptr<Level> readYAML(const std::string &file) const;
    virtual osg::ref_ptr<Level> readFile(const std::string&) const;
    virtual osg::ref_ptr<Level> readStream(std::istream&) const;


  protected:
    void createCube(LevelChunk *chunk,
		    float posx, float posy, float endx, float endy) const;
  };

  
  
};


#endif



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
  class LevelReader //: public osgDB::ReaderWriter
  {
  public:
    LevelReader();
    virtual ~LevelReader(void);

  public:
    virtual osg::ref_ptr<Level> readFile(const std::string&) const;
    virtual osg::ref_ptr<Level> readStream(std::istream&) const;


  protected:
    void createCube(Soleil::Level *level,
		    float posx, float posy, float endx, float endy) const;
  };

  
  
};


#endif





#ifndef SOLEIL_LEVEL__HPP
#define SOLEIL_LEVEL__HPP
# include <iostream>

# include <osg/Group>
# include <osg/Geometry>

# include "Sound.hpp"

namespace Soleil
{
  class LevelReader;
 
  class NextLevelZone : public osg::Geometry
  {
  public:
    static const char *ClassName;
    
  public:
    NextLevelZone(std::string nextZone) : _nextZone(nextZone) {}
    virtual ~NextLevelZone(void) {}

    virtual const char *className() const override
    {
      return NextLevelZone::ClassName;
    }

    virtual const std::string &nextZone(void) const noexcept
    {
      return _nextZone;
    }

  protected:
    std::string		_nextZone;
    
  };

  
  class Level : public osg::Group
  {
  public:
    Level(osg::ref_ptr<osg::Group> root);
    virtual ~Level(void);

  public:
    osg::Vec3	startingPosition() const;
    osg::Vec3	startingOrientation() const;
    bool	nextLevel(osg::Node *node) const
    {
      for (auto it = _nextLevelZones.cbegin(); it != _nextLevelZones.cend(); ++it) {
	if (*it == node)
	  {
	    std::cout << "VERS LA SORTIEEEEEE !"  << "\n";
	    return true;
	  }
      }
      return false;
    }

    void stop()
    {
      /* TODO horrible hack because there is a memleak and the sound not stop: */
      for (auto it = _sounds.cbegin(); it != _sounds.cend(); ++it) {
	it->get()->stop();
      }
    }


  protected:
    osg::ref_ptr<osg::Group>	_root;
    osg::Vec3			_startingPosition;
    osg::Vec3			_startingOrientation;
    
  protected:
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array();
    osg::ref_ptr<osg::Vec2Array> texcoords2 = new osg::Vec2Array();
    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
    osg::ref_ptr<osg::Group> scene = new osg::Group;

    std::vector<osg::ref_ptr<osg::Geometry>>	_nextLevelZones;

    std::vector<osg::ref_ptr<UpdateSoundCallBack>>	_sounds;

    
    friend Soleil::LevelReader;
  };

  
  
}  // Soleil

#endif





#ifndef SOLEIL_LEVEL__HPP
#define SOLEIL_LEVEL__HPP


namespace Soleil
{
  class LevelReader;

  
  class Level : public osg::Group
  {
  public:
    Level();
    virtual ~Level(void);

  public:
    osg::Vec3	startingPosition() const;
    osg::Vec3	startingOrientation() const;


  protected:
    osg::Vec3		_startingPosition;
    osg::Vec3		_startingOrientation;
    
  protected:
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array();
    osg::ref_ptr<osg::Vec2Array> texcoords2 = new osg::Vec2Array();
    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
    osg::ref_ptr<osg::Group> scene = new osg::Group;

    friend Soleil::LevelReader;
  };

  
  
}  // Soleil

#endif


#ifndef SOLEIL_COLLISIONUTIL__HPP
#define SOLEIL_COLLISIONUTIL__HPP

# include <osg/Geometry>
# include <osg/Geode>
# include <osgUtil/LineSegmentIntersector>


namespace Soleil
{

  class CollisionUtil
  {
  public:
    CollisionUtil(const osg::Vec3d &start, const osg::Vec3d &end, osg::Vec4 color, osg::ref_ptr<osg::Group> lineHolder
		  , bool forceCollision = false)
      : _lineIntersection(new osgUtil::LineSegmentIntersector(
							      //osgUtil::Intersector::CoordinateFrame::MODEL,
							      start, end))
      , _draw(true)
      , _color(color)
      , _lineHolder(lineHolder)
      , _forceCollision(forceCollision)
    {
      _line = new osg::Geometry;    
      osg::ref_ptr<osg::Vec3Array> points = new osg::Vec3Array;
      points->push_back(start);
      points->push_back(end);
      osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
      colors->push_back(color);
      _line->setVertexArray(points.get());
      _line->setColorArray(colors.get());
      _line->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
      _line->addPrimitiveSet(new osg::DrawArrays(GL_LINES,0,2));
    }

    CollisionUtil(const osg::Vec3d &start, const osg::Vec3d &end, bool forceCollision = false)
      : _lineIntersection(new osgUtil::LineSegmentIntersector(
							      //osgUtil::Intersector::CoordinateFrame::MODEL,
							      start, end))
      , _draw(false)
      , _color()
      , _lineHolder(nullptr)
      , _forceCollision(forceCollision)
    {
    }

    
    virtual ~CollisionUtil(void) {}

    bool compute(osg::Node &node)
    {
      if (_draw)
      	{
      	  _lineHolder->addChild(_line);
      	  // osg::ref_ptr<osg::Geode> g = new osg::Geode;
      	  // g->addDrawable(new osg::Geometry);
      	  // _lineHolder->addChild(g);
      	}

      if (_forceCollision)
      	{
      	  return true;
      	}


      osgUtil::IntersectionVisitor  v(_lineIntersection);
      //v.setTraversalMode( osg::NodeVisitor::TRAVERSE_ALL_CHILDREN );
      v.setTraversalMask(~0x1);
    
      node.accept(v);

      return _lineIntersection->containsIntersections();
    }
    
  protected:
    osg::ref_ptr<osgUtil::LineSegmentIntersector>	_lineIntersection;

    bool			_draw;
    osg::Vec4			_color;
    osg::ref_ptr<osg::Geometry>	_line;
    osg::ref_ptr<osg::Group>	_lineHolder;


    bool			_forceCollision;
  };

  
};

#endif

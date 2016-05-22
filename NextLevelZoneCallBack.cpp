
#include <iostream>

#include <osgUtil/LineSegmentIntersector>
#include <osgUtil/IntersectionVisitor>

#include "NextLevelZoneCallBack.hpp"
#include "NPC.hpp"


namespace Soleil
{
  NextLevelZoneCallBack::NextLevelZoneCallBack(const Level &level, const osg::PositionAttitudeTransform &playerPosition
					       , osgViewer::Viewer &viewer
					       )
    : _level(level)
    , _playerPosition(playerPosition)
    , _viewer(viewer)
  {
  }
  
  NextLevelZoneCallBack::~NextLevelZoneCallBack(void)
  {
  }

  void NextLevelZoneCallBack::operator()(osg::Node *node, osg::NodeVisitor *nv)
  {
    /* Is, one unit front the player, a next-level-zone? */
    /* how to determine a next-level-zone? */

    osg::Vec3d collision = (_playerPosition.getAttitude() * osg::Vec3d(0.0, 0, -0.3)) + _playerPosition.getPosition();

    osg::ref_ptr<osgUtil::LineSegmentIntersector>
      intersector =
      new osgUtil::LineSegmentIntersector(
					  osgUtil::Intersector::CoordinateFrame::MODEL,
					  _playerPosition.getPosition(), collision
					  );

    // Do not use if we add objects:
    intersector->setIntersectionLimit(osgUtil::Intersector::IntersectionLimit::LIMIT_NEAREST);
    osgUtil::IntersectionVisitor iv(intersector.get());
    iv.setTraversalMask(~0x1);
    
    node->accept(iv);


    for (auto it = intersector->getIntersections().cbegin(); it != intersector->getIntersections().cend(); ++it)
      {
	std::cout <<  "Front of the player is: " << it->drawable->getName() << ":" << it->drawable->className()  << "\n";
	if (it->drawable->className() == NextLevelZone::ClassName)
	  {
	    std::cout << "DOOOOOOOOOOOOOOOOONNNNNNNNNNNNNEEEEEEEEEEEE"  << "\n";
	    _viewer.setDone(true);
	  }
	else 
	  {
	    for (osg::Node *n : it->nodePath)
	      {
	    	std::cout << "Back: " << n->getName()  << "\n";
		if (n->className() == NPC::ClassName)
		  {
		    std::cout << "HAHAHAHAHAHAH!"  << "\n";
		    _viewer.setDone(true);
		  }
	      }

	  }

	// _level.nextLevel(node);
      }

    traverse(node, nv);
  }
  
  
}  // Soleil


#include <iostream>

#include <osgUtil/LineSegmentIntersector>
#include <osgUtil/IntersectionVisitor>

#include "NextLevelZoneCallBack.hpp"
#include "NPC.hpp"


// TEST
#include "LevelReader.hpp"
#include "FirstPersonManipulator.hpp"
#include "UpdateNPCVisitor.hpp"
#include "NextLevelZoneCallBack.hpp"

namespace Soleil
{
  NextLevelZoneCallBack::NextLevelZoneCallBack(Level &level, const osg::PositionAttitudeTransform &playerPosition
					       , GameInstance &viewer
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
	    _nextZone =  static_cast<NextLevelZone*>(it->drawable.get())->nextZone(); //"media/sub.level";
	  }
	else 
	  {
	    for (osg::Node *n : it->nodePath)
	      {
	    	std::cout << "Back: " << n->getName()  << "\n";
		if (n->className() == NPC::ClassName)
		  {
		    std::cout << "HAHAHAHAHAHAH!"  << "\n";
		    _viewer.gameOver();
		    _level.stop();
		    //_viewer.setDone(true);
		    /* TEST */
		    // {
		    //   Soleil::LevelReader l;
		      
		    //   osg::ref_ptr<osg::Group> root = new osg::Group;
		    //   root->setName("rootNode");
  
		    //   osg::ref_ptr<Soleil::Level>  level = l.readYAML("media/sub.level", root);

		    //   root->addUpdateCallback(new Soleil::UpdateNPCNodeCallBack(root));
		    //   //root->addUpdateCallback(new Soleil::NextLevelZoneCallBack(*level, *f, *viewer));

			 
		    //   root->addChild(level);
		    //   _viewer.setSceneData(root);
		    // }
		    //_viewer.setDone(true);
		  }
	      }

	  }

	// _level.nextLevel(node);
      }

    traverse(node, nv);
  }
  
  
}  // Soleil

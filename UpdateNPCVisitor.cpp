
#include <iostream>
#include <osg/io_utils>

#include "NPC.hpp"
#include "UpdateNPCVisitor.hpp"

#include "CollisionUtil.hpp"


namespace Soleil
{

  UpdateNPCVisitor::UpdateNPCVisitor(osg::ref_ptr<osg::Group> node)
    : _root(node)
  {
  }
  
  UpdateNPCVisitor::~UpdateNPCVisitor(void)
  {
  }

  bool UpdateNPCVisitor::isPossible(const osg::Vec3d &position, const osg::Vec3d &translation)
  {    
    return CollisionUtil(position, translation).compute(*_root) == false;
  }


  // TODO Add time so the move is independent of the fps
  void UpdateNPCVisitor::apply(osg::Node &node)
  {
    //std::cout << "-----------------" << node.getName() << ":" << node.className() << "\n";
    if (node.className() == NPC::ClassName) 
      {
	//std::cout << "\t-----------------" << node.getName() << "\n";

	//std::cout << "Callback"  << "\n";
	NPC &npc = static_cast<NPC&>(node);
	// if (npc->isAnimated() == false)
	//   {
	/*
	 * move		is the vector representing the step.
	 * position	is the current position
	 * front	is the next position to calculate collision
	 * translation	is the next position with the speed.
	 * --
	 *  'position' and 'front' have a value on z otherwise it would collide with the floor but not with the objects.
	 */
	osg::Vec3d move(0, 0, -.5);
	osg::Vec3d position = npc.getPosition();
	position.z() += 0.1;
	osg::Vec3d front = (npc.getAttitude() * move) + npc.getPosition();
	front.z() += 0.1;
	osg::Vec3d translation = (npc.getAttitude() * (move/10)) + npc.getPosition();
	// osg::Vec3 front_(npc->getPosition());
	// front_.z() += 2.0;
	// osg::Vec3 front =  npc->getAttitude() * front_;


	// osg::Vec3d apos(1, 0, 0);
	// osg::Quat adir(osg::DegreesToRadians(90.0), osg::Vec3(0, 0, 1));
	    
	// std::cout << "Collision between position [" << npc->getPosition() << "] and front [" << front << "]"
	// 	      << "Translation: " << (move/10) << "\n"
	// 	      << "Position: [" << apos << "] * direction [" << adir << "] = [" << adir * apos << "]"
	      
	// 	      << "\n\n";
	//   ;
	    
	if (isPossible(position, front))
	  {
	    //std::cout << "GO " << node.getName() << "!"  << "\n";
	    // npc->think();
	    npc.setPosition(translation);
	  }
	else
	  {
	    osg::Vec3d left	= (npc.getAttitude() * osg::Vec3d( 1, 0,  0)) + position;
	    osg::Vec3d right	= (npc.getAttitude() * osg::Vec3d(-1, 0,  0)) + position;
	    osg::Vec3d back	= (npc.getAttitude() * osg::Vec3d( 0, 0,  1)) + position;

	    int possibilities = 0;
	    int directions[3];
		
	    if (isPossible(position, left))
	      {
	    	directions[possibilities] = 1;
	    	possibilities++;
		    
	    	//npc->setAttitude( npc->getAttitude() * osg::Quat(osg::DegreesToRadians(90.0), osg::Vec3(0, 0, 1)));
	      }
		
	    if (isPossible(position, right))
	      {
	    	directions[possibilities] = -1;
	    	possibilities++;
	      }

	    if (isPossible(position, back))
	      {
	    	directions[possibilities] = 2;
	    	possibilities++;
	      }

	    if (possibilities > 0)
	      {
	    	float rotation = 90;
	    	if (possibilities > 1)
	    	  {
	    	    int solution = std::rand() % possibilities;
	    	    rotation *= directions[solution];
	    	  }
	    	else
	    	  rotation *= directions[0];
		
	    	npc.setAttitude( npc.getAttitude() * osg::Quat(osg::DegreesToRadians(rotation), osg::Vec3(0, 0, 1)));
	      }
	    
	    //std::cout << node.getName() << " Is colliding!"  << "\n";
	    //npc->turnRight(NPC::Cardinal::NE, NPC::Direction::left);
	    // :) --> npc->setAttitude( npc->getAttitude() * osg::Quat(osg::DegreesToRadians(90.0), osg::Vec3(0, 0, 1)));
	  }
	//   }
	// else
	//   std::cout << "Wait, " << node.getName() << " Is animated!"  << "\n";

	//if 
      }
    //std::cout << "---->" <<  node.className() << " : " << node.getName() << "\n";

    //std::cout << "-----------------" << node.getName() << "\n";
    traverse(node);
  }


  void UpdateNPCNodeCallBack::operator()(osg::Node *node, osg::NodeVisitor *nv)
  {
    UpdateNPCVisitor visitor(_root);

    visitor.setTraversalMode( osg::NodeVisitor::TRAVERSE_ALL_CHILDREN );
    visitor.setTraversalMask( ~0x1 );
    node->accept(visitor);

    traverse(node, nv);
  }
};


#include <osgDB/ReadFile>
#include <osgAnimation/BasicAnimationManager>
#include <osg/PositionAttitudeTransform>
#include <osg/AnimationPath>
#include <osgUtil/LineSegmentIntersector>

#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <thread>

#include "NPC.hpp"
#include "CollisionUtil.hpp"

namespace Soleil
{
  const char *NPC::ClassName = "Soleil::NPC";

  
  osg::ref_ptr<osg::AnimationPath> NPC::createAnimationPath(float time)
    {
      osg::ref_ptr<osg::AnimationPath> path = new osg::AnimationPath;
      
      path->setLoopMode(osg::AnimationPath::SWING);
      unsigned int numSamples = 64;
      // float delta_yaw = 2.0f * osg::PI / ((float)numSamples - 1.0f);
      float delta_time = time / (float)numSamples;

      osg::Vec3 pos(this->getPosition());
      osg::Quat rot(this->getAttitude());
      for ( unsigned int i=0; i<32; ++i )
	{
	  // float yaw = delta_yaw * (float)i;
	  // osg::Vec3 pos( sinf(yaw)*radius, cosf(yaw)*radius, 0.0f );
	  // osg::Quat rot( -yaw, osg::Z_AXIS );
	  // path->insert( delta_time * (float)i,
	  // 		osg::AnimationPath::ControlPoint(pos, rot)
	  // 		);
	  pos.x() -= 0.3;
	  path->insert(delta_time * (float)i,
		       osg::AnimationPath::ControlPoint(pos, rot)
		       );

	}
      
      for ( unsigned int i=0; i<numSamples; ++i )
	{
	  // float yaw = delta_yaw * (float)i;
	  // osg::Vec3 pos( sinf(yaw)*radius, cosf(yaw)*radius, 0.0f );
	  // osg::Quat rot( -yaw, osg::Z_AXIS );
	  // path->insert( delta_time * (float)i,
	  // 		osg::AnimationPath::ControlPoint(pos, rot)
	  // 		);
	  rot *= osg::Quat(osg::DegreesToRadians(-90.0 / numSamples), osg::Vec3(0, 0, 1));
	  path->insert(delta_time * ((float)i+31),
		       osg::AnimationPath::ControlPoint(pos, rot)
		       );

	}




      return path;
    }



  
  NPC::NPC(const osg::Vec3d &eye, osg::ref_ptr<osg::Group> root) /// , const osg::Vec3d &center
    : _root(root)
  {
    osg::ref_ptr<osg::Node> model =
      osgDB::readNodeFile("media/spider_flare.osgt");
    //osgDB::readNodeFile("/usr/home/florian/jeux/OpenSceneGraph-Data/axes.osgt");


    /* For animation: */
    // osgAnimation::BasicAnimationManager* manager =
    //   dynamic_cast<osgAnimation::BasicAnimationManager*>
    //   ( model->getUpdateCallback() );
    // if ( !manager ) throw "No Manager";
    // const osgAnimation::AnimationList& animations =
    //   manager->getAnimationList();
    //  std::cout << "**** Animations ****" << std::endl;
    // for ( unsigned int i=0; i<animations.size(); ++i )
    //   {
    // 	const std::string& name = animations[i]->getName();
    // 	if ( name=="Action.spider_flare" )
    // 	  {
    // 	  manager->playAnimation( animations[i].get() );
    // 	  std::cout << "Playing..."  << "\n";
    // 	  }
    // 	std::cout << name << std::endl;
    //   }
    
    if (model == nullptr)
      throw "Model cannot be read";

    // this->setTransformation(eye, center, osg::Vec3( 0, 0, 1));
    this->_position = eye;

    this->setAttitude(osg::Quat(osg::DegreesToRadians(-90.0), osg::Vec3(1, 0, 0)));

    this->_attitude *= osg::Quat(osg::DegreesToRadians(-90.0), osg::Vec3(0, 0, 1));
    // this->_attitude.makeRotate(osg::DegreesToRadians(-90.0), osg::Vec3(0, 0, 1));

    this->addChild(model);


    // _path = createAnimationPath(6.0f);
    // osg::ref_ptr<osg::AnimationPathCallback> apcb = new osg::AnimationPathCallback;
    // apcb->setAnimationPath(_path);
    //this->setUpdateCallback(apcb);

    //moveForward();
    //turnRight(Cardinal::S, Direction::right);
  }


  NPC::~NPC(void)
  {
  }

  

  void 	NPC::setTransformation(const osg::Vec3d &eye, const osg::Quat &rotation)
  {

     // std::cout << "FirstPersonManipulator::setTransformation: Old Position [" << _position << " X " << _attitude
     // 	      << "] New Position: [" <<  eye << " X " << rotation << "]\n";
    this->_position = eye;
    this->_attitude = rotation;
  }

  void 	NPC::setTransformation(const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up)
  {
    osg::Matrixd view;
    
    view.makeLookAt(eye, center, up);
    
    this->setTransformation(eye, view.getRotate()); // .inverse()
  }

  void NPC::moveForward(void)
  {
    const std::clock_t beginTime = std::clock();
    auto t_start = std::chrono::high_resolution_clock::now();

    float time = 1.0;
    osg::ref_ptr<osg::AnimationPath> path = new osg::AnimationPath;

    path->setLoopMode(osg::AnimationPath::NO_LOOPING);
    unsigned int numSamples = 10;
    // float delta_yaw = 2.0f * osg::PI / ((float)numSamples - 1.0f);
    float delta_time = time / (float)numSamples;

    osg::Vec3 pos(this->getPosition());
    osg::Quat rot(this->getAttitude());
    float speed = 1.0 / (float)numSamples;
    for (unsigned int i=0; i<numSamples; ++i)
      {
	//float yaw = delta_yaw * (float)i;
	// osg::Vec3 pos( sinf(yaw)*radius, cosf(yaw)*radius, 0.0f );
	//osg::Quat rot( -yaw, osg::Y_AXIS );
	// path->insert( delta_time * (float)i,
	// 		osg::AnimationPath::ControlPoint(pos, rot)
	// 		);
	pos.x() -= speed;
	///rot *= osg::Quat(osg::DegreesToRadians((direction * 45.0 * cardinal) / numSamples), osg::Z_AXIS);
	
	path->insert(delta_time * (float)i,
		     osg::AnimationPath::ControlPoint(pos, rot)
		     );

      }

    //osg::ref_ptr<osg::AnimationPathCallback> apcb = new osg::AnimationPathCallback;
    //    apcb->setAnimationPath(path);
    _apcb = new MovementCallBack(path);
    this->setUpdateCallback(_apcb);


    
    const std::clock_t endTime = std::clock();
    auto t_end = std::chrono::high_resolution_clock::now();
    
    // std::cout << std::fixed << std::setprecision(2) << "CPU time used: "
    //           << 1000.0 * (endTime-beginTime) / CLOCKS_PER_SEC << " ms\n"
    //           << "Wall clock time passed: "
    //           << std::chrono::duration<double, std::milli>(t_end-t_start).count()
    //           << " ms\n";
  }

  void NPC::turnRight(int cardinal, Direction direction)
  {
    //float time = 1 + (cardinal / 4); /** TODO Slow a little bit when the NPC do revert but not too much */
    float time = 1;
    osg::ref_ptr<osg::AnimationPath> path = new osg::AnimationPath;

    path->setLoopMode(osg::AnimationPath::NO_LOOPING);
    unsigned int numSamples = 10;
    float delta_yaw = 2.0f * osg::PI / ((float)numSamples - 1.0f);
    float delta_time = time / (float)numSamples;

    //osg::Vec3 pos(this->getPosition());
    osg::Quat rot(this->getAttitude());
    for ( unsigned int i=0; i<numSamples; ++i )
      {
	float yaw = delta_yaw * (float)i;
	// osg::Vec3 pos( sinf(yaw)*radius, cosf(yaw)*radius, 0.0f );
	//osg::Quat rot( -yaw, osg::Y_AXIS );
	// path->insert( delta_time * (float)i,
	// 		osg::AnimationPath::ControlPoint(pos, rot)
	// 		);
	//pos.x() -= 0.3;
	rot *= osg::Quat(osg::DegreesToRadians((direction * 45.0 * cardinal) / numSamples), osg::Z_AXIS);
	
	path->insert(delta_time * (float)i,
		     osg::AnimationPath::ControlPoint(this->getPosition(), rot)
		     );

      }

    // osg::ref_ptr<osg::AnimationPathCallback> apcb = new osg::AnimationPathCallback;
    // apcb->setAnimationPath(path);
    // this->setUpdateCallback(apcb);
    _apcb = new MovementCallBack(path);
    this->setUpdateCallback(_apcb);

  }

  void NPC::think()
  {
    //osg::Vec3 front(this->getPosition());
    // // osg::Vec3 front =  getAttitude() * this->getPosition();
    //front.x() -= 1.0;
    // osg::Vec3d move(0, 0, 10);
    // osg::Vec3d front = (getAttitude() * move) + getPosition();
    
    

    
    // osg::ref_ptr<osgUtil::LineSegmentIntersector> line = new osgUtil::LineSegmentIntersector(getPosition(), front);
    // //line->setIntersectionLimit(osgUtil::Intersector::IntersectionLimit::LIMIT_NEAREST);
    
    // osgUtil::IntersectionVisitor  v(line);
    // //v.setTraversalMask(~0x1);
    
    // _root->accept(v);

    // if (line->containsIntersections())
    //   {
    // 	turnRight(Cardinal::W, Direction::right);
    // 	return;
    //   }

    // if (CollisionUtil(osg::Vec3(0, 0, -0.5), osg::Vec3(0, 0, -1), osg::Vec4(1,1,1,1), this, false).compute(*_root))
    //   {
    // 	turnRight(Cardinal::W, Direction::right);
    // 	return;
    //   }
    moveForward();
  }

  const char *NPC::className() const
  {
    return ClassName;
  }

  bool NPC::isAnimated(void) const
  {
    if (!_apcb) return false;
    
    return _apcb->getPause();
  }



  NPC::MovementCallBack::MovementCallBack(osg::AnimationPath *ap)
    : AnimationPathCallback(ap)
  {
  }
  
  NPC::MovementCallBack::~MovementCallBack(void)
  {
  }
  
  void NPC::MovementCallBack::operator() (osg::Node *node, osg::NodeVisitor *nv)
  {
    //std::cout << "Time:" <<  nv->getFrameStamp()->getSimulationTime()  << "\n";
    
    if (getPause())
      {
	// must call any nested node callbacks and continue subgraph traversal.
	NodeCallback::traverse(node,nv);
	// But not continue
	return ;
      }

    if (this->getAnimationTime() >= .99)
      {
    	setPause(true);
    	// //static_cast<NPC*>(node)->moveForward();
	// static_cast<NPC*>(node)->think();
    	// // return ;//  TODO does the return help?
      }
    
    osg::AnimationPathCallback::operator()(node, nv);
  }
  
}  // Soleil

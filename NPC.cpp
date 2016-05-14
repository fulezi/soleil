
#include <osgDB/ReadFile>
#include <osgAnimation/BasicAnimationManager>
#include <osg/PositionAttitudeTransform>
#include <osg/AnimationPath>

#include "NPC.hpp"

namespace Soleil
{

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



  
  NPC::NPC(const osg::Vec3d &eye) /// , const osg::Vec3d &center
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

    moveForward();
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
    float time = 1.0;
    osg::ref_ptr<osg::AnimationPath> path = new osg::AnimationPath;

    path->setLoopMode(osg::AnimationPath::NO_LOOPING);
    unsigned int numSamples = 10;
    // float delta_yaw = 2.0f * osg::PI / ((float)numSamples - 1.0f);
    float delta_time = time / (float)numSamples;

    osg::Vec3 pos(this->getPosition());
    osg::Quat rot(this->getAttitude());
    float speed = 1.0 / (float)numSamples;
    for ( unsigned int i=0; i<numSamples; ++i )
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
    osg::ref_ptr<osg::AnimationPathCallback> apcb = new MovementCallBack(path);
    this->setUpdateCallback(apcb);
  }

  void NPC::turnRight(int cardinal, Direction direction)
  {
    float time = 1 + (cardinal / 4); /** TODO Slow a little bit when the NPC do revert but not too much */
    osg::ref_ptr<osg::AnimationPath> path = new osg::AnimationPath;

    path->setLoopMode(osg::AnimationPath::NO_LOOPING);
    unsigned int numSamples = 64;
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

    osg::ref_ptr<osg::AnimationPathCallback> apcb = new osg::AnimationPathCallback;
    apcb->setAnimationPath(path);
    this->setUpdateCallback(apcb);

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
    if (this->getAnimationTime() >= 1.0)
      {
	setPause(true);
	static_cast<NPC*>(node)->moveForward();
	return ;//  TODO does the return help?
      }

    if (getPause())
      {
	// must call any nested node callbacks and continue subgraph traversal.
	NodeCallback::traverse(node,nv);
	// But not continue
	return ;
      }
    osg::AnimationPathCallback::operator()(node, nv);
  }
  
}  // Soleil

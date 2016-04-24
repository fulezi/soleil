
#include <iostream>
#include <osg/io_utils>

#include "FirstPersonManipulator.hpp"

/* TODO Il y a un 'saut' de la rotation au demarrage car la souris n'est pas au centre */
/* TODO Ralentir la vitesse de deplacement et de rotation */
/* TODO regler les problemes de collision sur le cote */
namespace Soleil
{
  FirstPersonManipulator::FirstPersonManipulator(const osg::Vec3d &eye, const osg::Vec3d &center)
    : _move(0, 0, 0)
  {
    osg::Vec3 up(0.0, 0.0, 1.0);
    
    initTransformation(eye, center, up);
    //rformMouseDeltaMovement(20, 20);

    // _homeEye = eye;
    // _homeCenter = center;
    // _homeUp = up;
    getTransformation(_homeEye, _homeCenter, _homeUp);
    _autoComputeHomePosition = false;

  }
  
  FirstPersonManipulator::~FirstPersonManipulator(void)
  {
  }

  void 	FirstPersonManipulator::setTransformation(const osg::Vec3d &eye, const osg::Quat &rotation)
  {

     // std::cout << "FirstPersonManipulator::setTransformation: Old Position [" << _position << " X " << _attitude
     // 	      << "] New Position: [" <<  eye << " X " << rotation << "]\n";
    this->_position = eye;
    this->_attitude = rotation;
  }

  void 	FirstPersonManipulator::setTransformation(const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up)
  {
    // std::cout << "FirstPersonManipulator::setTransformation: "
    // 	      << "eye: " << eye
    // 	      << ", center: " << center
    // 	      << ", up: " << up
    // 	      << "\n";
    osg::Matrixd view;
    
    view.makeLookAt(eye, center, up);


    ////////////////////////////////////////////////////
    // Necessary otherwise the camera seem head downs //
    ////////////////////////////////////////////////////
    // view.makeRotate(osg::DegreesToRadians(90.0), osg::Vec3(1, 0, 0));

    
    // std::cout << "FirstPersonManipulator::setTransformation: " << view  << "\n";
    
    this->setTransformation(eye, view.getRotate().inverse());
  }


  void 	FirstPersonManipulator::initTransformation(const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up)
  {
        std::cout << "FirstPersonManipulator::initTransformation: "
	      << "eye: " << eye
	      << ", center: " << center
	      << ", up: " << up
	      << "\n";
    osg::Matrixd view;
    
    view.makeLookAt(eye, center, up);


    ////////////////////////////////////////////////////
    // Necessary otherwise the camera seem head downs //
    ////////////////////////////////////////////////////
    //view.makeRotate(osg::DegreesToRadians(90.0), osg::Vec3(1, 0, 0));

    
    // std::cout << "FirstPersonManipulator::setTransformation: " << view  << "\n";
    
    this->setTransformation(eye, view.getRotate().inverse());
  }

  void 	FirstPersonManipulator::getTransformation(osg::Vec3d &eye, osg::Quat &rotation) const
  {
    eye = this->_position;
    rotation = this->_attitude;
  }

  void 	FirstPersonManipulator::getTransformation(osg::Vec3d &eye, osg::Vec3d &center, osg::Vec3d &up) const
  {
    // osg::Matrixd view = osg::Matrix::translate(this->_position) * osg::Matrix::rotate(this->_attitude);

    // // view.setTrans(this->_position);
    // // view.setRotate(this->_attitude);
    // view.getLookAt(eye, center, up);
    center = _position + _attitude * osg::Vec3d( 0.,0., -1. );
    eye = _position;
    up = _attitude * osg::Vec3d( 0.,1.,0. );
  }

  void 	FirstPersonManipulator::setByMatrix(const osg::Matrixd &matrix)
  {
    // std::cout << "FirstPersonManipulator::setByMatrix: " << matrix << "\n";
    this->setTransformation(matrix.getTrans(), matrix.getRotate());
  }
  
  void 	FirstPersonManipulator::setByInverseMatrix(const osg::Matrixd &matrix)
  {
    // std::cout << "FirstPersonManipulator::setByInverseMatrix: " << matrix << "\n";
    setByMatrix(osg::Matrixd::inverse(matrix));
  }
    
  osg::Matrixd 	FirstPersonManipulator::getMatrix() const
  {
    osg::Matrixd view;

    view.setTrans(this->_position);
    view.setRotate(this->_attitude);
    // std::cout << "FirstPersonManipulator::getMatrix: " << view << "\n";
    return view;
    //return osg::Matrixd::rotate( _attitude ) * osg::Matrixd::translate( _position );
  }
  
  osg::Matrixd 	FirstPersonManipulator::getInverseMatrix() const
  {
    // osg::Matrixd view;

    // view.setTrans(-this->_position);
    // view.setRotate(this->_attitude);
    // std::cout << "FirstPersonManipulator::getInverseMatrix: " << view << "\n";
    // return view;

    // std::cout << "FirstPersonManipulator::getInverseMatrix: " << osg::Matrixd::inverse(getMatrix()) << "\n";
    return osg::Matrixd::inverse(getMatrix());
    // osg::Matrixd v;
    // v = getMatrix();
    // return v.inverse(v);
    ///return osg::Matrixd::translate( -_position ) * osg::Matrixd::rotate( _attitude.inverse() );
  }

  bool 	FirstPersonManipulator::handleFrame(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us)
  {
    bool r = StandardManipulator::handleFrame(ea, us);

    if (_move.x() == 0 && _
	move.z() == 0)
      return r;
    
    osg::Vec3d translation = _attitude * _move;
    translation.z() = 0;
    translation = _position + translation;

    if (translation.isNaN())
      return r;
    
    // TEST intersection
    //std::cout << "Translation: " << _position << " going to: " << translation   << "\n";
    osg::ref_ptr<osgUtil::LineSegmentIntersector>
      intersector =
      new osgUtil::LineSegmentIntersector(
					  _position, translation
					  );
    //osgUtil::Intersector::WINDOW, ea.getX(), ea.getY()
    intersector->setIntersectionLimit(osgUtil::Intersector::IntersectionLimit::LIMIT_NEAREST);
    osgUtil::IntersectionVisitor iv( intersector.get() );
    iv.setTraversalMask( ~0x1 );
    us.asView()->getCamera()->accept( iv );
    if ( intersector->containsIntersections() )
      {
	return r;
      }

    
    //_position += translation;
    _position = translation;
    
    return r; // TODO what means exactly this 'r'?
  }
  
  bool 	FirstPersonManipulator::handleKeyDown(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us)
  {
    switch (ea.getKey())
      {
      case 'w': case 'W':
	_move.z() = -_translationSpeed;
	break;
      case 's': case 'S':
    	_move.z() = _translationSpeed;
    	break;
      case 'a': case 'A':
    	_move.x() = -_translationSpeed / 2;
    	break;
      case 'd': case 'D':
    	_move.x() = _translationSpeed / 2;
    	break;
      case 'q': case 'Q':
    	_attitude *= osg::Quat(osg::DegreesToRadians(1.0), osg::Vec3(0, 0, 1));
    	break;
      case 'e': case 'E':
    	_attitude *= osg::Quat(osg::DegreesToRadians(-1.0), osg::Vec3(0, 0, 1));
    	break;
      }
    
    // switch (ea.getKey())
    //   {
    //   case 'w': case 'W':
    // 	_position += _attitude * osg::Vec3(0, 0, -_translationSpeed);
    // 	break;
    //   case 's': case 'S':
    // 	_position += _attitude * osg::Vec3(0, 0, _translationSpeed);
    // 	break;
    //   case 'a': case 'A':
    // 	_position += _attitude * osg::Vec3(-_translationSpeed, 0, 0);
    // 	break;
    //   case 'd': case 'D':
    // 	_position += _attitude * osg::Vec3(_translationSpeed, 0, 0);
    // 	break;
    //   case 'q': case 'Q':
    // 	_attitude *= osg::Quat(osg::DegreesToRadians(1.0), osg::Vec3(0, 0, 1));
    // 	break;
    //   case 'e': case 'E':
    // 	_attitude *= osg::Quat(osg::DegreesToRadians(-1.0), osg::Vec3(0, 0, 1));
    // 	break;

    //   }
    return true;
  }

  bool 	FirstPersonManipulator::handleKeyUp(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us)
  {
    switch (ea.getKey())
      {
      case 'w': case 'W':
	_move.z() = 0;
	break;
      case 's': case 'S':
    	_move.z() = 0;
    	break;
      case 'a': case 'A':
    	_move.x() = 0;
    	break;
      case 'd': case 'D':
    	_move.x() = 0;
    	break;
      }
    return true;
  }

  bool 	FirstPersonManipulator::performMouseDeltaMovement(const float dx, const float dy)
  {
    // std::cout << "Mouse: " << dx << "\n";
    
    // _attitude *= osg::Quat(osg::DegreesToRadians(_rotationSpeed), osg::Vec3(0, 0, -dx)); // osg::Vec3(0, 0, -ea.getXnormalized()
    // _attitude *= osg::Quat(osg::DegreesToRadians(_rotationSpeed), osg::Vec3(-dy, 0, 0)); // osg::Vec3(0, 0, -ea.getXnormalized()

    // osg::Quat q(osg::DegreesToRadians(_rotationSpeed), osg::Vec3(0, 0, -dx));
    // q *= osg::Quat(osg::DegreesToRadians(_rotationSpeed), osg::Vec3(-dy, 0, 0));
    // _attitude *= q;

    // _attitude.makeRotate(osg::DegreesToRadians(_rotationSpeed), osg::Vec3(0, 0, -dx));
    
    osg::CoordinateFrame coordinateFrame = getCoordinateFrame( _position );
    osg::Vec3d localUp = getUpVector( coordinateFrame );
    rotateYawPitch( _attitude, dx / 1000, dy / 1000, localUp );    

   // rotateYawPitch(_attitude, dx / 1000, dy / 1000);


    return true;
  }
  
  bool 	FirstPersonManipulator::handleMouseMove(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us)
  {
    float dx = ea.getX() - _mouseCenterX;
    float dy = ea.getY() - _mouseCenterY;
    //std::cout << "Mouse: " << dx << ", " << dy << "\n";

    if( dx == 0.f && dy == 0.f )
      {
	// std::cout << "Mouse: No mouvement..." << "\n";
        return false;
      }

    //addMouseEvent( ea );
    centerMousePointer( ea, us );

    return performMouseDeltaMovement( dx, dy );
    //return true;
  }

  void 	FirstPersonManipulator::init(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us)
  {
    StandardManipulator::init(ea, us);
    addMouseEvent( ea );
    centerMousePointer(ea, us);
    std::cout << "Yaaaaaaaaaaaaaaaaaaaaaaa"  << "\n";
  }

  void 	FirstPersonManipulator::home(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us)
  {
    StandardManipulator::home(ea, us);
    addMouseEvent( ea );
    centerMousePointer(ea, us);
    std::cout << "Yaaaaaaaaaaaaaaaaaaaaaaa"  << "\n";
  }

  // void 	home(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us)
  // {
    
  // }
  
  // void 	home(double d) override
  // {
    
  // }


  // bool FirstPersonManipulator::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
  // {    
  //   switch (ea.getEventType())
  //     {
  //     case osgGA::GUIEventAdapter::KEYDOWN:
  // 	switch (ea.getKey())
  // 	  {
  // 	  case 'w': case 'W':
  // 	    {
  // 	      osg::Vec3d eye;
  // 	      osg::Vec3d center;
  // 	      osg::Vec3d normale;
  // 	      _view.getLookAt(eye, center, normale);

  // 	      // compute direction:
  // 	      osg::Vec3d direction = center - eye;
  // 	      direction.normalize();
  // 	      direction.x() += 0.1;
  // 	      eye = eye + direction;
  // 	      center = center + direction;
  // 	      _view.makeLookAt(eye, center, normale);
	      
  // 	      // osg::Vec3d trans = _view.getTrans();
  // 	      // std::cout << "Before: " << _view << "Trans: " << trans  << "\n";
  // 	      // trans.x() += 0.01;
  // 	      // //_view.makeTranslate(trans);
  // 	      // _view.setTrans(trans);
  // 	      // std::cout << "Translate: " << _view  << "\n";
  // 	    }
  // 	    break;
  // 	  case 'a': case 'A':
  // 	    {
  // 	      osg::Vec3d eye;
  // 	      osg::Vec3d center;
  // 	      osg::Vec3d normale;
  // 	      _view.getLookAt(eye, center, normale);

  // 	      /* Test with qauternion */
  // 	      // osg::Quat orientation = _view.getRotate();
  // 	      // osg::Vec3d direction(0.01, 0.0, 0.00);
  // 	      // direction = orientation * direction;
  // 	      // eye = eye + direction;
  // 	      // center = center + direction;
  // 	      // _view.makeLookAt(eye, center, normale);


	      
  // 	      osg::Vec3d direction = center - eye;
  // 	      ///direction.normalize();
  // 	      direction.y() += 0.1;
  // 	      eye = eye + direction;
  // 	      center = center + direction;
  // 	      _view.makeLookAt(eye, center, normale);


	      
	      
  // 	      // osg::Vec3d trans = _view.getTrans();
  // 	      // std::cout << "Before: " << _view << "Trans: " << trans  << "\n";
  // 	      // trans.x() += 0.01;
  // 	      // //_view.makeTranslate(trans);
  // 	      // _view.setTrans(trans);
  // 	      // std::cout << "Translate: " << _view  << "\n";
  // 	    }
  // 	    break;
  // 	  default:
  // 	    break;
  // 	  }
  // 	break;
  //     }
  //   return false;
  // }
  
  // const osg::Matrixd FirstPersonManipulator::getView(void) const noexcept
  // {
  //   return _view;
  // }
  
}  // Soleil

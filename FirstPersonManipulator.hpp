
#ifndef SOLEIL_FIRSTPERSONMANIPULATOR__HPP
#define SOLEIL_FIRSTPERSONMANIPULATOR__HPP

# include <osgGA/StandardManipulator>
# include <osg/PositionAttitudeTransform>

#include <osg/Geode>

#include "GameInstance.hpp"

namespace Soleil
{
  
  class FirstPersonManipulator : public osgGA::StandardManipulator, public osg::PositionAttitudeTransform
  {
  public:
    FirstPersonManipulator(osg::ref_ptr<GameInstance> gameInstance,  const osg::Vec3d &eye, const osg::Vec3d &center);
    virtual ~FirstPersonManipulator(void);

    // public:
    //   virtual const osg::Matrixd getView(void) const noexcept;
    //   virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);


    /* CameraManipulator: */
  public:
    // set the position of the matrix manipulator using a 4x4 Matrix.
    virtual void 	setByMatrix(const osg::Matrixd &matrix) override;
    // set the position of the matrix manipulator using a 4x4 Matrix.
    virtual void 	setByInverseMatrix(const osg::Matrixd &matrix) override;
    // get the position of the manipulator as 4x4 Matrix.
    virtual osg::Matrixd 	getMatrix() const override;
    // get the position of the manipulator as a inverse matrix of the manipulator, typically used as a model view matrix. 
    virtual osg::Matrixd 	getInverseMatrix() const override;
 	

    /* Standardmanipulator: */
  public:
    // Sets manipulator by eye position and eye orientation.
    virtual void 	setTransformation(const osg::Vec3d &eye, const osg::Quat &rotation) override;
    // Sets manipulator by eye position, center of rotation, and up vector.
    virtual void 	setTransformation(const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up) override;
    // Gets manipulator's eye position and eye orientation.
    virtual void 	getTransformation (osg::Vec3d &eye, osg::Quat &rotation) const override;
    // Gets manipulator's focal center, eye position, and up vector. 
    virtual void 	getTransformation(osg::Vec3d &eye, osg::Vec3d &center, osg::Vec3d &up) const override;

    virtual bool 	handleFrame(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us) override;

    // Start/restart the manipulator.
    virtual void 	init (const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us) override;

    // Move the camera to the default position. 
    virtual void 	home (const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us) override;

    // // Move the camera to the default position.
    // virtual void 	home (const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us) override;
    // // Move the camera to the default position.
    // virtual void 	home (double d) override;

    /* Events: */
  public:
    virtual bool 	handleKeyDown(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us) override;
    virtual bool 	handleKeyUp(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us) override;
    virtual bool 	handleMouseMove(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us) override;
    virtual bool 	performMouseDeltaMovement(const float dx, const float dy) override;


  protected:
    virtual void 	initTransformation(const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up);

    
  protected:
    osg::Vec3		_move;
    double		_rotationSpeed = 0.8;
    double		_translationSpeed = 2.0;
    double		_lastFrameTime = 0;
    //osg::Matrixd		_view;
    osg::ref_ptr<GameInstance> _gameInstance;

  public:
    bool DrawedCollision(osgGA::GUIActionAdapter &us, const osg::Vec3 &start, const osg::Vec3 &end, osg::Vec4 c);
    osg::ref_ptr<osg::Geode> _tmp = new osg::Geode();
    
  };

  
}  // Soleil


#endif

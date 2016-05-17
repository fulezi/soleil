

#ifndef SOLEIL_NPC__HPP
#define SOLEIL_NPC__HPP

# include <osg/PositionAttitudeTransform>
# include <osg/Callback>

namespace Soleil
{
  
  class NPC : public osg::PositionAttitudeTransform
  {
  public:
    enum Direction { left = 1 , right = -1 };
    enum Cardinal { N, NE, E, SE, S, SW, W, NW };
    const static char *ClassName;

  protected:
    class MovementCallBack : public osg::AnimationPathCallback
    {
    public:
      MovementCallBack(osg::AnimationPath *ap);
      virtual void operator()(Node *node, osg::NodeVisitor *nv) override;
    protected:
      virtual ~MovementCallBack(void);

    };
    
  public:
    NPC(const osg::Vec3d &eye, osg::ref_ptr<osg::Group> root); // , const osg::Vec3d &center
    virtual ~NPC(void);

    virtual const char *className() const override;
    
    virtual void setTransformation(const osg::Vec3d &eye, const osg::Quat &rotation);
    virtual void setTransformation(const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up);

    virtual void think();
    virtual void moveForward(void);
    virtual void turnRight(int cardinal, Direction direction);
    virtual bool isAnimated(void) const;
    
  protected:
    virtual osg::ref_ptr<osg::AnimationPath> createAnimationPath(float time);
    
  protected:
    osg::ref_ptr<osg::Group>		_root;
    osg::ref_ptr<osg::AnimationPath>	_path;
    osg::ref_ptr<osg::AnimationPathCallback> _apcb = nullptr;

  protected:
    double		_speed = 0.1;
    osg::Vec3		_destination;
    osg::Quat		_orientation;
  };
  
  
}  // Soleil


#endif

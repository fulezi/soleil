

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
    NPC(const osg::Vec3d &eye); // , const osg::Vec3d &center
    virtual ~NPC(void);

    virtual void setTransformation(const osg::Vec3d &eye, const osg::Quat &rotation);
    virtual void setTransformation(const osg::Vec3d &eye, const osg::Vec3d &center, const osg::Vec3d &up);

    virtual void moveForward(void);
    virtual void turnRight(int cardinal, Direction direction);
    
  protected:
    virtual osg::ref_ptr<osg::AnimationPath> createAnimationPath(float time);
    
  protected:
    osg::ref_ptr<osg::AnimationPath>	_path;

  };
  
  
}  // Soleil


#endif

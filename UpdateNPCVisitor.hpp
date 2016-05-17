
#ifndef SOLEIL_UPDATENPCVISITOR__HPP
#define SOLEIL_UPDATENPCVISITOR__HPP

# include <osg/NodeCallback>
# include <osg/NodeVisitor>

namespace Soleil
{

  class UpdateNPCNodeCallBack : public osg::NodeCallback
  {
  public:
    UpdateNPCNodeCallBack(osg::ref_ptr<osg::Group> root) : _root(root) {}
    virtual ~UpdateNPCNodeCallBack(void) {}

    virtual void operator()(osg::Node *node, osg::NodeVisitor *nv) override;

  protected:
    osg::ref_ptr<osg::Group>	_root;
  };

  class UpdateNPCVisitor : public osg::NodeVisitor
  {
  public:
    UpdateNPCVisitor(osg::ref_ptr<osg::Group> node);
    virtual ~UpdateNPCVisitor(void);
  
    virtual void apply(osg::Node &node) override;

  protected:
    virtual inline bool isPossible(const osg::Vec3d &position, const osg::Vec3d &translation);

  protected:
    osg::ref_ptr<osg::Group>	_root;
  };
};

#endif


#ifndef SOLEIL_MESH__HPP
#define SOLEIL_MESH__HPP

# include <osg/MatrixTransform>
# include <osg/Geode>

namespace Soleil
{
  class Mesh : public osg::MatrixTransform
  {
  public:
    Mesh(void);
    virtual ~Mesh(void);

  public:
    void	setPosition(const osg::Vec3 &position);

  public:
    static const char *_className;
    
    virtual const char* className(void) const override;
    
  protected:
    osg::ref_ptr<osg::Geode>		_node;
  };
};
#endif


#include "Mesh.hpp"

namespace Soleil
{
  const char *Mesh::_className = "Soleil::Mesh";

  Mesh::Mesh()
    : _node(new osg::Geode)
  {
    this->addChild(_node);
  }

  Mesh::~Mesh(void)
  {
  }

  void	Mesh::setPosition(const osg::Vec3 &position)
  {
    this->setMatrix(osg::Matrix::translate(position));
  }

  const char *Mesh::className(void) const
  {
    return _className;
  }
};

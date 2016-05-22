
#ifndef SOLEIL_NEXTLEVELZONE__HPP
#define SOLEIL_NEXTLEVELZONE__HPP

# include <osgViewer/Viewer>

# include <osg/NodeCallback>
# include <osg/NodeVisitor>
# include <osg/PositionAttitudeTransform>

# include "Level.hpp"

namespace Soleil
{
  class NextLevelZoneCallBack : public osg::NodeCallback
  {
  public:
    NextLevelZoneCallBack(const Level &level, const osg::PositionAttitudeTransform &playerPosition, osgViewer::Viewer &viewer);
    virtual ~NextLevelZoneCallBack(void);

    virtual void operator()(osg::Node *node, osg::NodeVisitor *nv) override;

  protected:
    const Level					&_level;
    const osg::PositionAttitudeTransform	&_playerPosition;

    osgViewer::Viewer				&_viewer;
  };

};

#endif

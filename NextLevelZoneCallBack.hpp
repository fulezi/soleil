
#ifndef SOLEIL_NEXTLEVELZONE__HPP
#define SOLEIL_NEXTLEVELZONE__HPP

# include <osgViewer/Viewer>

# include <osg/NodeCallback>
# include <osg/NodeVisitor>
# include <osg/PositionAttitudeTransform>

# include "GameInstance.hpp"
# include "Level.hpp"

namespace Soleil
{
  class NextLevelZoneCallBack : public osg::NodeCallback
  {
  public:
    NextLevelZoneCallBack(const Level &level, const osg::PositionAttitudeTransform &playerPosition, GameInstance &viewer);
    virtual ~NextLevelZoneCallBack(void);

    virtual void operator()(osg::Node *node, osg::NodeVisitor *nv) override;

    virtual std::string	nextZone() const { return _nextZone; }

  protected:
    const Level					&_level;
    const osg::PositionAttitudeTransform	&_playerPosition;
    GameInstance				&_viewer;
    std::string					_nextZone = "";
  };

};

#endif

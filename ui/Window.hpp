
#ifndef SOLEIL_WINDOW__HPP
#define SOLEIL_WINDOW__HPP

# include <osg/MatrixTransform>
# include <osg/PositionAttitudeTransform>
# include <osg/Projection>
# include <osg/Texture2D>
# include <osg/Billboard>
# include <osgText/Text>

# include "Container.hpp"

namespace Soleil
{
  namespace ui
  {

    class Window : public osg::Projection , public Container
    {
    public:
      Window(const double width, const double height);

      virtual void set(Widget *widget, size_t row, size_t col = 1) override;

      virtual void setBackground(const std::string &background);
    
      virtual ~Window(void) {}

    protected:
      //osg::ref_ptr<osg::Geode> createPlane(const char *background);
      
      // virtual void add(Widget *widget)
      // {}

    
    protected:
      osg::ref_ptr<osg::Geode> _hudGeode;
      double	_width;
      double	_height;
      osg::ref_ptr<osg::MatrixTransform> _matrix;

    };

    
  };
};

#endif

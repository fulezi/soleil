
#ifndef SOLEIL_UI__HPP
#define SOLEIL_UI__HPP

# include <osg/MatrixTransform>
# include <osg/PositionAttitudeTransform>
# include <osg/Projection>
# include <osg/Texture2D>
# include <osg/Billboard>
# include <osgText/Text>


namespace Soleil
{
  namespace ui
  {
    class Widget
    {
    public:
      Widget() {}
      virtual ~Widget(void) {}

      virtual void setSize(double width, double height) = 0;

      virtual osg::Drawable *getDrawable() = 0;
    };

    class Text : public osgText::Text, public Widget
    {
    public:
      Text();
	

      Text(const char label[]);
	
    
      virtual ~Text(void)
      {
      }

      virtual void setSize(double width, double height);

      virtual osg::Drawable *getDrawable();
    

    protected:
      double computeSize() const;
      

    };

    class IContainer
    {
    public:
      virtual ~IContainer(void) {}

      // virtual void columns(size_t number) = 0;
      // virtual size_t columns(void) = 0 const;
      // virtual void rows(size_t number) = 0;
      // virtual size_t rows(const) = 0 const;
    
      virtual void set(Widget *widget, size_t row, size_t col = 1) = 0;
    };


    class Container : public IContainer
    {
    public:
      Container(size_t rows, size_t columns, double width, double height);
    
      virtual ~Container(void)
      {
      }

      virtual void set(Widget *widget, size_t row, size_t col = 1);

    // protected:
    //   virtual void add(Widget *widget) = 0;
    
    protected:
      size_t		_rows;
      size_t		_columns;
      double		_width;
      double		_height;
    };

    class Window : public osg::Projection, public Container
    {
    public:
      Window(const double width, const double height, const char *background = nullptr);

      virtual void set(Widget *widget, size_t row, size_t col = 1) override;
    
      virtual ~Window(void) {}

    protected:
      osg::ref_ptr<osg::Geode> createPlane(const char *background);
      
      // virtual void add(Widget *widget)
      // {}

    
    protected:
      osg::ref_ptr<osg::Geode> HUDGeode = new osg::Geode;
      double	_width;
      double	_height;
    };

    
  }; 
};

#endif

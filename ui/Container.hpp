


#ifndef SOLEIL_CONTAINER__HPP
#define SOLEIL_CONTAINER__HPP

# include <osg/Drawable>

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
      double	_width;
      double	_height;
    };
  };
};


#endif

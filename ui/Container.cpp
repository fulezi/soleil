
#include "Container.hpp"

namespace Soleil
{
  namespace ui
  {
    Container::Container(size_t rows, size_t columns, double width, double height)
      : _rows(rows)
      , _columns(columns)
      , _width(width)
      , _height(height)
    {
    }
    
    void Container::set(Widget *widget, size_t row, size_t col)
    {
      widget->setSize(_width / _rows, _height / _columns);
    }
  };
};

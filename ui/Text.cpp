

#include "Text.hpp"

namespace Soleil
{
  namespace ui
  {
    Text::Text()
      : osgText::Text()
    {
      this->setFont("media/dirtydoz.ttf");
    }

    Text::Text(const char label[])
      : Text()
    {
      this->setText(label);
    }

    void Text::setSize(double width, double height)
    {
      /* TODO: If it beome required to limit the size on the height, we
	 need to also compute the Vertical advance for the tallest char and
	 take the smallest value */
      //double size = std::min(width, height);
      double size = width;
      
      
      size = size / computeSize();
      
      this->setCharacterSize(size);
      //this->setMaximumWidth(size);
    }

    osg::Drawable *Text::getDrawable()
    {
      return this;
    }

    double Text::computeSize() const
    {
      osgText::Font *a = (osgText::Font*)this->getFont(); //
      
      double size = 0;
      for (auto c : this->getText())
	{
	  auto s =  a->getGlyph(this->_fontSize, 'a')->getHorizontalAdvance();
	  size += s;
	}      
      return size;
    }

  };
};

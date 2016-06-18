


#ifndef SOLEIL_TEXT__HPP
#define SOLEIL_TEXT__HPP

# include <osgText/Text>

# include "Container.hpp"

namespace Soleil
{
  namespace ui
  {
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

  };
};

#endif

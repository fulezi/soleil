
#include <iostream>

#include <osgDB/ReadFile>

#include "UI.hpp"

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
	  std::cout << "Char [" << c << "] = " << s << "\n";
	  size += s;
	}

      std::cout << "Computed size: " << size  << "\n";
      
      return size;
    }

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


    Window::Window(const double width, const double height, const char *background)
	: Container(1, 1, width, height)
	, _width(width)
	, _height(height)
      {
	// TODO get screen size --->
	// double screenw = 1920;
	// double screenh = 1080;

	// double posw = (screenw - width) / 2;
	// double posh = (screenh - height) / 2;

	// std::cout << "posw:" << posw << ", posh: " << posh << "\n";
      
	osg::Projection* HUDProjectionMatrix = new osg::Projection;
	//this->setMatrix(osg::Matrix::ortho2D(0 ,_width, 0, _height));
	this->setMatrix(osg::Matrix::ortho2D(0, width, 0, height));

	osg::MatrixTransform* HUDModelViewMatrix = new osg::MatrixTransform;
	//HUDModelViewMatrix->setMatrix(osg::Matrix::identity());
	// osg::Matrix m;

	//m.setTrans(posw, posh, .0);
	// HUDModelViewMatrix->setMatrix(m);
	HUDModelViewMatrix->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	this->addChild(HUDModelViewMatrix);

	HUDModelViewMatrix->addChild(createPlane(background));

      }

    void Window::set(Widget *widget, size_t row, size_t col)
      {
	Container::set(widget, row, col);

	HUDGeode->addDrawable(widget->getDrawable());
      }

    osg::ref_ptr<osg::Geode> Window::createPlane(const char *background)
    {

      osg::Geometry* HUDBackgroundGeometry = new osg::Geometry();

      osg::Vec3Array* HUDBackgroundVertices = new osg::Vec3Array;
      HUDBackgroundVertices->push_back(osg::Vec3(     0,   0		,-1));
      HUDBackgroundVertices->push_back(osg::Vec3(_width,   0		,-1));
      HUDBackgroundVertices->push_back(osg::Vec3(_width, _height	,-1));
      HUDBackgroundVertices->push_back(osg::Vec3(     0, _height	,-1));

      osg::DrawElementsUInt* HUDBackgroundIndices =
	new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON, 0);
      HUDBackgroundIndices->push_back(0);
      HUDBackgroundIndices->push_back(1);
      HUDBackgroundIndices->push_back(2);
      HUDBackgroundIndices->push_back(3);

      osg::Vec4Array* HUDcolors = new osg::Vec4Array;
      /* Transparent Dark grey background */
      HUDcolors->push_back(osg::Vec4(0.f,0.f,0.f,.35f));

      // Create and set up a state set using the texture from above:
      //osg::StateSet* HUDStateSet = HUDGeode->getOrCreateStateSet();
      osg::StateSet* HUDStateSet = new osg::StateSet();
      HUDGeode->setStateSet(HUDStateSet);
      
      // if (background != nullptr)
      // 	{
	  osg::Vec2Array* texcoords = new osg::Vec2Array(4);
	  (*texcoords)[0].set(0.0f,0.0f);
	  (*texcoords)[1].set(1.0f,0.0f);
	  (*texcoords)[2].set(1.0f,1.0f);
	  (*texcoords)[3].set(0.0f,1.0f);
	  HUDBackgroundGeometry->setTexCoordArray(0,texcoords);
	  osg::Texture2D* HUDTexture = new osg::Texture2D;
	  HUDTexture->setDataVariance(osg::Object::DYNAMIC);
	  osg::Image* hudImage;
	  hudImage = osgDB::readImageFile(background);
	  HUDTexture->setImage(hudImage);

	  HUDStateSet->
	    setTextureAttributeAndModes(0,HUDTexture,osg::StateAttribute::ON);
	// }
	
      osg::Vec3Array* HUDnormals = new osg::Vec3Array;
      HUDnormals->push_back(osg::Vec3(0.0f,0.0f,1.0f));
      HUDBackgroundGeometry->setNormalArray(HUDnormals);
      HUDBackgroundGeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
      HUDBackgroundGeometry->addPrimitiveSet(HUDBackgroundIndices);
      HUDBackgroundGeometry->setVertexArray(HUDBackgroundVertices);
      HUDBackgroundGeometry->setColorArray(HUDcolors);
      HUDBackgroundGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);


      // For this state set, turn blending on (so alpha texture looks right)
      HUDStateSet->setMode(GL_BLEND,osg::StateAttribute::ON);

      // Disable depth testing so geometry is draw regardless of depth values
      // of geometry already draw.
      HUDStateSet->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
      HUDStateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

      // Need to make sure this geometry is draw last. RenderBins are handled
      // in numerical order so set bin number to 11
      HUDStateSet->setRenderBinDetails( 12, "RenderBin");

      HUDGeode->addDrawable(HUDBackgroundGeometry);
	    
      
      return HUDGeode;
    }

    
  };
};

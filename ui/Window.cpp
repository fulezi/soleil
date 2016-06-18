

#include <osgDB/ReadFile>

#include "Window.hpp"

namespace Soleil
{
  namespace ui
  {
    Window::Window(const double width, const double height)
      : Container(1, 1, width, height)
      , _width(width)
      , _height(height)
    {
      /*
       * Create a projection with a unity matrix to be displayed front of the screen:
       */
      this->setMatrix(osg::Matrix::ortho2D(0, width, 0, height));
      _matrix = new osg::MatrixTransform;
      _matrix->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

      _hudGeode = new osg::Geode();
      _matrix->addChild(_hudGeode);

      
      this->addChild(_matrix);
     
    }

    void Window::setBackground(const std::string &background)
    {
      osg::Geometry* hudBackground = new osg::Geometry();

      /*
       * Fill th projection:
       */
      osg::Vec3Array* vertices = new osg::Vec3Array;
      vertices->push_back(osg::Vec3(        0,          0, -1));
      vertices->push_back(osg::Vec3(_width,          0, -1));
      vertices->push_back(osg::Vec3(_width, _height, -1));
      vertices->push_back(osg::Vec3(        0 ,_height, -1));

      osg::DrawElementsUInt* indices =
	new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON, 0);
      indices->push_back(0);
      indices->push_back(1);
      indices->push_back(2);
      indices->push_back(3);

      /*
       * Color does impact how the texture will be drawn:
       * - Set the alpa to 0 and the texture will be invisilbe, set to 0.5 you will see the behind
       * - Set the color to 0.8 to make the texture a bit more darken... or to 1 to have no modification
       */
      osg::Vec4Array* colors = new osg::Vec4Array;
      colors->push_back(osg::Vec4(0.8f,0.8f,0.8f,1.f)); 

      osg::Vec2Array* texcoords = new osg::Vec2Array(4);
      (*texcoords)[0].set(0.0f,0.0f);
      (*texcoords)[1].set(1.0f,0.0f);
      (*texcoords)[2].set(1.0f,1.0f);
      (*texcoords)[3].set(0.0f,1.0f);

      hudBackground->setTexCoordArray(0,texcoords);
      osg::Texture2D* texture = new osg::Texture2D;
      texture->setDataVariance(osg::Object::DYNAMIC);
      osg::Image* hudImage;
      hudImage = osgDB::readImageFile(background);
      texture->setImage(hudImage);
      osg::Vec3Array *normals = new osg::Vec3Array;
      normals->push_back(osg::Vec3(0.0f,0.0f,1.0f));
      hudBackground->setNormalArray(normals);
      hudBackground->setNormalBinding(osg::Geometry::BIND_OVERALL);
      hudBackground->addPrimitiveSet(indices);
      hudBackground->setVertexArray(vertices);
      hudBackground->setColorArray(colors);
      hudBackground->setColorBinding(osg::Geometry::BIND_OVERALL);


      osg::StateSet* hudStateSet = new osg::StateSet();
      _hudGeode->setStateSet(hudStateSet);
      hudStateSet->
	setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);

      // For this state set, turn blending on (so alpha texture looks right)
      hudStateSet->setMode(GL_BLEND,osg::StateAttribute::ON);

      // Disable depth testing so geometry is draw regardless of depth values
      // of geometry already draw.
      hudStateSet->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
      hudStateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

      // Need to make sure this geometry is draw last. RenderBins are handled
      // in numerical order so set bin number to 11
      // TODO Each window should have a RenderBin so one window can draw on top of another one.
      hudStateSet->setRenderBinDetails( 11, "RenderBin");

      /*
       * Make sure the light of our scene will not interfere with our Window:
       */
      hudStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED); 
    
      _hudGeode->addDrawable(hudBackground);


    }

    void Window::set(Widget *widget, size_t row, size_t col)
    {
      Container::set(widget, row, col);

      // // TODO: It may be required to create a geod per widget otherwise, to allow modify the stateset of the widget.
      // _hudGeode->addDrawable(widget->getDrawable());

      /*
       * Center the widget
       */
      const osg::BoundingBox b = widget->getDrawable()->getBoundingBox();
      double posw = (_width - b.xMax()) / 2;
      double posh = (_height - b.yMax()) / 2;

      /*
       * We need to add a Geode to center the widget, otherwise it will also center the backgound.
       * TODO is it betterto separate the backgound?
       */
      //osg::ref_ptr<osg::Geode>
      osg::Geode *g = new osg::Geode;
      g->addDrawable(widget->getDrawable());
      //osg::ref_ptr<osg::MatrixTransform>
      osg::MatrixTransform *matrix = new osg::MatrixTransform(osg::Matrixd::translate(posw, posh, 0.0));
      matrix->getOrCreateStateSet()->setRenderBinDetails(12, "RenderBin");
      matrix->addChild(g);

      //_hudGeode->addChild(matrix);
      _matrix->addChild(matrix);
    }
    
  };
};

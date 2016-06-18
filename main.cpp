#include <osg/MatrixTransform>	
#include <osgDB/ReadFile>	
#include <osgViewer/Viewer>
#include <osg/LightSource>
#include <osgGA/FirstPersonManipulator>
#include <osg/AnimationPath>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/TrackballManipulator>

#include <osg/Texture2D>
#include <osg/Billboard>
#include <osgText/Text>

#include <osg/io_utils>

#include "Cube.hpp"

#include "GameInstance.hpp"
#include "LevelReader.hpp"
#include "FirstPersonManipulator.hpp"
#include "UpdateNPCVisitor.hpp"
#include "NextLevelZoneCallBack.hpp"

#include "ui/Window.hpp"
#include "ui/Text.hpp"

#include <iostream>
#include <osg/io_utils>


namespace Soleil
{
  class Object : public osg::PositionAttitudeTransform
  {
  public:
    Object() {
      osg::ref_ptr<osg::Node> model =
	osgDB::readNodeFile("media/axes.osgt");
      if (!model)
	  throw "Cannot read model";
      osg::ref_ptr<osg::Group> group = new osg::Group;
      group->addChild(model);
      this->addChild(group);
    }

    void translate(const osg::Vec3 &translation) {
        // osg::Quat	q(osg::DegreesToRadians(-90.0), osg::Vec3(0, 0, 1));
  // osg::Vec3	v(2, 1, 1);
  // std::cout << "Before: " << v << "\n";
  // v = q * v;
  // obj->setPosition(osg::Vec3(0, 0, 1) + v);
  // std::cout << "After: " << v << "\n";

      _position += _attitude * translation;
    }

    virtual ~Object(void) {} 
  };
};

osg::ref_ptr<osg::Node> createLightSource( unsigned int num,
			      const osg::Vec3& trans,
			      const osg::Vec4& color )
{
  osg::ref_ptr<osg::Light> light = new osg::Light;
  light->setLightNum( num );
  light->setDiffuse( color );

  /* full scene lightened: */
  light->setAmbient(osg::Vec4(0.1,0.1,0.1,0.1));
  light->setSpecular(osg::Vec4(0.1,0.1,0.1,0.1));  // some examples don't have this one

  
  light->setPosition( osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f) );
  osg::ref_ptr<osg::LightSource> lightSource = new
    osg::LightSource;
  lightSource->setLight( light );
  osg::ref_ptr<osg::MatrixTransform> sourceTrans = new osg::MatrixTransform;
  sourceTrans->setMatrix( osg::Matrix::translate(trans) );
  sourceTrans->addChild( lightSource.get() );
  return sourceTrans;
}


osg::ref_ptr<osg::Geometry> createQuad()
{
  osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
  osg::ref_ptr<osg::Image> image =
    osgDB::readImageFile("media/pumpkin/Pumpkin.png");
  texture->setImage(image.get());
  osg::ref_ptr<osg::Geometry> quad = osg::createTexturedQuadGeometry(
								     osg::Vec3(-0.5f, 0.0f,-0.5f),
								     osg::Vec3(1.0f,0.0f,0.0f),
								     osg::Vec3(0.0f,0.0f,1.0f)
								     );
  osg::StateSet* ss = quad->getOrCreateStateSet();
  ss->setTextureAttributeAndModes(0, texture.get());
  return quad;
}

osg::ref_ptr<osg::Camera>  createHUDCamera(double left, double right,
			      double bottom, double top)
{
  osg::ref_ptr<osg::Camera> camera = new osg::Camera;
  camera->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
  camera->setClearMask( GL_DEPTH_BUFFER_BIT );
  camera->setRenderOrder( osg::Camera::POST_RENDER );
  camera->setAllowEventFocus( false );
  // camera->setProjectionMatrix(
  // 			      osg::Matrix::ortho2D(left, right, bottom, top) );
  return camera;
}


osgText::Text* createText( const osg::Vec3& pos,
			   const std::string& content,
			   float size )
{
  osg::ref_ptr<osgText::Font> g_font =
    osgText::readFontFile("fonts/arial.ttf");
  osg::ref_ptr<osgText::Text> text = new osgText::Text;
  text->setFont( g_font.get() );
  text->setCharacterSize( size );
  text->setAxisAlignment( osgText::TextBase::XY_PLANE );
  text->setPosition( pos );
  text->setText( content );
  return text.release();
}

int	main(int argc, char **argv)
{
  osg::ArgumentParser arguments(&argc,argv);
 
  
  //////////
  // TEST //
  //////////
  // osg::ref_ptr<Soleil::Object> obj = new Soleil::Object;
  // obj->setPosition(osg::Vec3(1, 2, 4.2));
  // obj->setAttitude(osg::Quat(osg::DegreesToRadians(-90.0), osg::Vec3(0, 0, 1)));
  // //obj->setAttitude(osg::Quat(-0.5, -0.5, -0.5, 0.5));
  // root->addChild(obj);
  // // osg::Quat	q(osg::DegreesToRadians(-90.0), osg::Vec3(0, 0, 1));
  // // osg::Vec3	v(2, 1, 1);
  // // std::cout << "Before: " << v << "\n";
  // // v = q * v;
  // // obj->setPosition(osg::Vec3(0, 0, 1) + v);
  // // std::cout << "After: " << v << "\n";
  // obj->translate(osg::Vec3(2, -1, -3));
  

  /// TEST ON CAMERA
  // viewer.getCamera()->setClearColor( osg::Vec4( 0., 0., 0., 0. ) );  
        
  /* View Developper: Just view an quit */
  // if (arguments.read("-d"))
  //   {
  //     osg::ref_ptr<osgViewer::Viewer>  viewer = new osgViewer::Viewer;
  //     viewer->setSceneData(root);
  //     return viewer->run();
  //   }

  bool devView = arguments.read("-d");
  int r = 0;
  //osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
  osg::ref_ptr<Soleil::GameInstance> viewer = new Soleil::GameInstance;

  viewer->realize();
  std::cout << "CONTEXTS: ------------------------------------------------------------------------" << "\n";

  int width;
  int height;
  osgViewer::ViewerBase::Contexts contexts;
  viewer->getContexts(contexts);
  for (osg::GraphicsContext *c : contexts)
    {
      std::cout << "Contexts found";
      osgViewer::GraphicsWindow *w = dynamic_cast<osgViewer::GraphicsWindow*>(c);
      if (w)
	{
	  std::cout << "\tGraphic windows is valid";
	  w->useCursor(false);
	  const auto traits = c->getTraits();
	  width = traits->width;
	  height = traits->height;
	}
      std::cout << "\n";
    }

  viewer->getCamera()->setClearColor( osg::Vec4( 0., 0., 0., 0. ) );  

  
  std::string nextZone = "media/entrance.level";//cb->nextZone();
  while (r == 0 && nextZone.length() > 0)
    {
      viewer->setDone(false);
      //osg::ref_ptr<osg::Group> root = new osg::Group;
      osg::ref_ptr<osg::Switch> root = new osg::Switch;
      
      root->setName("rootNode");

      

      Soleil::LevelReader l;
      osg::ref_ptr<Soleil::Level>  level;
      std::string file = nextZone;
      try
	{
	  if (arguments.read("-l", osg::ArgumentParser::Parameter(file)))
	    level = l.readYAML(file, root);
	  else
	    {
	      level = l.readYAML(nextZone, root);
	    }
	  if (!level)
	    throw "Cannot read model";
	  root->addChild(level, viewer->playing());
	}
      catch (const std::exception &e)
	{
	  std::cout << "Read model '"<< file << "' failed: "<< e.what() << "=>" << ::strerror(errno)  << "\n";
	  return 1;
	}
      catch (const char *msg)
	{
	  std::cout << "Read model failed: "<< msg  << "\n";
	  return 1;
	}



      ///////////////
      // menu Test //
      ///////////////
      // osg::ref_ptr<osg::Billboard> menu = new osg::Billboard;
      // menu->setMode(osg::Billboard::POINT_ROT_EYE);
      // osg::ref_ptr<osg::Geometry> quad = createQuad();
      // //menu->addDrawable(quad, osg::Vec3( 2.5f-0.2f, 1, 0.0f));
      // menu->addDrawable(quad);
      // osg::StateSet* ss = menu->getOrCreateStateSet();
      // ss->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );


      // osg::ref_ptr<osgText::Font> g_font =
      // 	osgText::readFontFile("fonts/arial.ttf");
      // menu->addDrawable(createText(osg::Vec3(150.0f, 500.0f, 0.0f),
      // 				   "The Cessna monoplane",
      // 				   20.0f));

      
      // osg::ref_ptr<osg::Camera>  camera = createHUDCamera(0,1280,0,1024);

      // camera->setViewMatrixAsLookAt(
      // 				osg::Vec3(0.0f,-5.0f,5.0f), osg::Vec3(),
      // 				osg::Vec3(0.0f,1.0f,1.0f)
      // 				);

      
      // camera->addChild(menu);
      // camera->addChild(new Soleil::Object);

      // root->addChild(camera);
      // root->addChild(new Soleil::Object);

     
      // std::cout << "CONTEXTS END: --------------------------------------------------------------------" << "\n";


      std::cout << "MENU: ------------------------------------------------------------------------" << "\n";


      Soleil::ui::Window *w = new Soleil::ui::Window(800, 600); //(1024, 65);
      w->setBackground( "media/background.png"); 
      osg::ref_ptr<Soleil::ui::Text> text = new Soleil::ui::Text("Enter the Maze!");
      w->set(text, 1);
      root->addChild(w, viewer->inMenu());


      std::cout << "GAME OVER: -------------------------------------------------------------------" << "\n";

      Soleil::ui::Window *windowGameOver = new Soleil::ui::Window(800, 600); //(1024, 65);
      //windowGameOver->setBackground( "media/background.png"); 
      osg::ref_ptr<Soleil::ui::Text> textGameOver = new Soleil::ui::Text("Game Over!");
      textGameOver->setColor(osg::Vec4(1.0, 0.5, 0.5, 0.8));
      windowGameOver->set(textGameOver, 1);
      root->addChild(windowGameOver, false);


      
      
      /////////////////////
      // Fin TEST	 //
      /////////////////////


      
      root->addChild(createLightSource(0, osg::Vec3(0, -2.0, 0), osg::Vec4(0.1, 0.1, 0.1, 0.1)));
      root->addChild(createLightSource(1, osg::Vec3(0, -16.0, 10.0), osg::Vec4(0.1, 0.1, 0.1, 0.1)));

      root->getOrCreateStateSet()->setMode( GL_LIGHT0, osg::StateAttribute::ON );
      root->getOrCreateStateSet()->setMode( GL_LIGHT1, osg::StateAttribute::ON );

      viewer->setSceneData(root);
      root->addUpdateCallback(new Soleil::UpdateNPCNodeCallBack(root));
      if (devView == false)
	{
	  Soleil::FirstPersonManipulator *f = new Soleil::FirstPersonManipulator(viewer, level->startingPosition(), level->startingOrientation());
	  //root->addChild(f->_tmp);
	  viewer->setCameraManipulator(f);
	  osg::ref_ptr<Soleil::NextLevelZoneCallBack> cb = new Soleil::NextLevelZoneCallBack(*level, *f, *viewer);
	  root->addUpdateCallback(cb);
	  
	  r = viewer->run();	
	  nextZone = cb->nextZone();
	}
      else
	return viewer->run();
    }

  return r;
}

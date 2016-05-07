#include <osg/MatrixTransform>	
#include <osgDB/ReadFile>	
#include <osgViewer/Viewer>
#include <osg/LightSource>
#include <osgGA/FirstPersonManipulator>
#include <osg/AnimationPath>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/TrackballManipulator>

#include <osg/io_utils>

#include "Cube.hpp"

#include "LevelReader.hpp"
#include "FirstPersonManipulator.hpp"

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
  light->setAmbient(osg::Vec4(1.0,1.0,1.0,1.0));
  light->setSpecular(osg::Vec4(1,1,1,1));  // some examples don't have this one

  
  light->setPosition( osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f) );
  osg::ref_ptr<osg::LightSource> lightSource = new
    osg::LightSource;
  lightSource->setLight( light );
  osg::ref_ptr<osg::MatrixTransform> sourceTrans = new osg::MatrixTransform;
  sourceTrans->setMatrix( osg::Matrix::translate(trans) );
  sourceTrans->addChild( lightSource.get() );
  return sourceTrans;
}

int	main(int argc, char **argv)
{
  osg::ArgumentParser arguments(&argc,argv);
  
  osg::ref_ptr<osg::Group> root = new osg::Group;

  root->addChild(createLightSource(0, osg::Vec3(0, -2.0, 0), osg::Vec4(1.0, 1.0, 1.0, 1.0)));
  root->addChild(createLightSource(1, osg::Vec3(0, -16.0, 10.0), osg::Vec4(1.0, 1.0, 1.0, 1.0)));

  root->getOrCreateStateSet()->setMode( GL_LIGHT0, osg::StateAttribute::ON );
  root->getOrCreateStateSet()->setMode( GL_LIGHT1, osg::StateAttribute::ON );


  Soleil::LevelReader l;
  osg::ref_ptr<Soleil::Level>  level;
  try
    {
      std::string file;

      if (!arguments.read("-l", osg::ArgumentParser::Parameter(file)))
	level = l.readFile("media/bastion.level");
      else
	level = l.readFile(file);

      // level = l.readFile("/usr/home/florian/Documents/Jeux/futur/axes.level");
      //level = l.readFile("/usr/home/florian/Documents/Jeux/futur/large.level");
      //level = l.readFile("/usr/home/florian/Documents/Jeux/futur/ouvert.level");
      //level = l.readFile("/usr/home/florian/Documents/Jeux/futur/bastion.level");
      //level = l.readFile("/usr/home/florian/Documents/Jeux/futur/huge.level");
      if (!level)
  	throw "Cannot read model";
      root->addChild(level);
    }
  catch (const std::exception &e)
    {
      std::cout << "Read model failed: "<< e.what() << "=>" << ::strerror(errno)  << "\n";
      return 1;
    }
  catch (const char *msg)
    {
      std::cout << "Read model failed: "<< msg  << "\n";
      return 1;
    }
  

  
  //////////
  // TEST //
  //////////
  osg::ref_ptr<Soleil::Object> obj = new Soleil::Object;
  obj->setPosition(osg::Vec3(1, 2, 4.2));
  obj->setAttitude(osg::Quat(osg::DegreesToRadians(-90.0), osg::Vec3(0, 0, 1)));
  //obj->setAttitude(osg::Quat(-0.5, -0.5, -0.5, 0.5));
  root->addChild(obj);
  // osg::Quat	q(osg::DegreesToRadians(-90.0), osg::Vec3(0, 0, 1));
  // osg::Vec3	v(2, 1, 1);
  // std::cout << "Before: " << v << "\n";
  // v = q * v;
  // obj->setPosition(osg::Vec3(0, 0, 1) + v);
  // std::cout << "After: " << v << "\n";
  obj->translate(osg::Vec3(2, -1, -3));
  

  /// TEST ON CAMERA
  // viewer.getCamera()->setClearColor( osg::Vec4( 0., 0., 0., 0. ) );  

  osg::ref_ptr<osgViewer::Viewer>  viewer = new osgViewer::Viewer;
  viewer->setSceneData(root);

  if (!arguments.read("-d"))
    {
      Soleil::FirstPersonManipulator *f = new Soleil::FirstPersonManipulator(level->startingPosition(), level->startingOrientation());
      root->addChild(f->_tmp);
    viewer->setCameraManipulator(f);
    }
    
  /* else auto-add trackball */
  
    
  // while (!viewer.done())
  //   {
  //     // eye.x()		+= .01;
  //     // center.x()	+= .01;
      
  //     //viewer.getCamera()->setViewMatrixAsLookAt(eye, center, normale);
  //     viewer.getCamera()->setViewMatrix(caman->getView());
      
  //     // Draw the next frame.
  //     viewer.frame();
  //   }
  // return 0;


  viewer->realize();
	
	    for(unsigned int contextID = 0;
	        contextID<osg::DisplaySettings::instance()->getMaxNumberOfGraphicsContexts();
	        ++contextID)
	    {
	        osg::GLExtensions* textExt = osg::GLExtensions::Get(contextID,false);
	        if (textExt)
	        {
	            if (!textExt->isMultiTexturingSupported)
	            {
	                std::cout<<"Warning: multi-texturing not supported by OpenGL drivers, unable to run application."<<std::endl;
	                return 1;
	            }
		    else
		      std::cout << "Ouffff"  << "\n";
	        }
	    }

  
  return viewer->run();
}

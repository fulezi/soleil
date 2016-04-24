#include <osg/MatrixTransform>	
#include <osgDB/ReadFile>	
#include <osgViewer/Viewer>
#include <osg/LightSource>
#include <osgGA/FirstPersonManipulator>

#include <osg/PositionAttitudeTransform>

#include <osg/io_utils>

#include "Cube.hpp"

#include "LevelReader.hpp"
#include "FirstPersonManipulator.hpp"

#include <iostream>
#include <osg/io_utils>


// #include <osg/Texture2D>
// namespace Soleil
// {
//   class BlenderCube : public Mesh
//   {
//   public:
//     BlenderCube() {
//       osg::ref_ptr<osg::Node> model =
// 	osgDB::readNodeFile("/usr/home/florian/Documents/Jeux/futur/cube.obj");
//       if (!model)
// 	  throw "Cannot read model";
//       osg::ref_ptr<osg::Group> group = new osg::Group;
//       group->addChild(model);
//       this->addChild(group);
//       osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
//       osg::ref_ptr<osg::Image> image = osgDB::readImageFile("/usr/home/florian/Documents/Jeux/futur/stone_3_2048x2048.jpg");
//       texture->setImage(image);
//       texture->setUnRefImageDataAfterApply(true);
//       texture->setDataVariance(osg::Object::DYNAMIC);
//       this->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture);
//     }
//     virtual ~BlenderCube(void) {}    
//   };
// };

namespace Soleil
{
  class Object : public osg::PositionAttitudeTransform
  {
  public:
    Object() {
      osg::ref_ptr<osg::Node> model =
	osgDB::readNodeFile("/usr/home/florian/jeux/OpenSceneGraph-Data/axes.osgt");
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
  osgViewer::Viewer viewer;
  
  osg::ref_ptr<osg::Group> root = new osg::Group;

  //root->addChild(new Soleil::Cube());
  root->addChild(createLightSource(0, osg::Vec3(0, -2.0, 0), osg::Vec4(1.0, 1.0, 1.0, 1.0)));
  root->addChild(createLightSource(1, osg::Vec3(0, -16.0, 10.0), osg::Vec4(1.0, 1.0, 1.0, 1.0)));

  root->getOrCreateStateSet()->setMode( GL_LIGHT0, osg::StateAttribute::ON );
  root->getOrCreateStateSet()->setMode( GL_LIGHT1, osg::StateAttribute::ON );


  // osg::ref_ptr<Soleil::Mesh> cube = new Soleil::Cube();
  // cube->setPosition(osg::Vec3(1.0, 0.0, 0.0));
  // root->addChild(cube);


  // osg::ref_ptr<Soleil::Mesh> cube = new Soleil::BlenderCube();
  // cube->setPosition(osg::Vec3(3.0, 0.0, 0.0));
  // root->addChild(cube);

  Soleil::LevelReader l;
  osg::ref_ptr<Soleil::Level>  level;
  try
    {
      // level = l.readFile("/usr/home/florian/Documents/Jeux/futur/axes.level");
      level = l.readFile("/usr/home/florian/Documents/Jeux/futur/large.level");
      if (!level)
  	throw "Cannot read model";
      root->addChild(level);
    }
  catch (const std::exception &e)
    {
      std::cout << "Read model failed: "<< e.what() << "=>" << ::strerror(errno)  << "\n";
      return 1;
    }
  
  
  viewer.setSceneData(root);

  // std::cout << "Starting Position: " << level->startingPosition() << ". Orientation: " << level->startingOrientation() <<  "\n";
  // viewer.getCamera()->setProjectionMatrixAsPerspective(40., 1., .1, 1000.);




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
  // osg::Vec3d normale(0.0, 0.0, 1.0); 
  
  // viewer.getCamera()->setClearColor( osg::Vec4( 0., 0., 0., 0. ) );  

  // osg::ref_ptr<Soleil::FirstPersonManipulator> caman = new Soleil::FirstPersonManipulator(level->startingPosition(), level->startingOrientation());
  // viewer.addEventHandler(caman);
  // osg::Vec3d eye = level->startingPosition();
  // osg::Vec3d center = level->startingOrientation();

  // osgGA::FirstPersonManipulator doomView;  
  viewer.setCameraManipulator(new Soleil::FirstPersonManipulator(level->startingPosition(), level->startingOrientation()));
    
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


  
  return viewer.run();
}

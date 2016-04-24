

#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osg/Geometry>
#include <osg/Geode>
#include <osgUtil/SmoothingVisitor>
#include <osgViewer/Viewer>

#include "Cube.hpp"



namespace Soleil
{

  Cube::Cube()
    : Mesh()
  {
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array();
    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;

    // ------ top
    vertices->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
    vertices->push_back(osg::Vec3(1.0f, 0.0f, 0.0f));
    vertices->push_back(osg::Vec3(1.0f, 0.0f, 1.0f));
    vertices->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));
	      
    normals->push_back(osg::Vec3(0.0f,-1.0f, 0.0f));
    normals->push_back(osg::Vec3(0.0f,-1.0f, 0.0f));
    normals->push_back(osg::Vec3(0.0f,-1.0f, 0.0f));
    normals->push_back(osg::Vec3(0.0f,-1.0f, 0.0f));

    texcoords->push_back( osg::Vec2(0.0f, 0.0f) ); 
    texcoords->push_back( osg::Vec2(0.0f, 1.0f) ); 
    texcoords->push_back( osg::Vec2(1.0f, 1.0f) ); 
    texcoords->push_back( osg::Vec2(1.0f, 0.0f) ); 


    // ------ top
    vertices->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));
    vertices->push_back(osg::Vec3(1.0f, 0.0f, 1.0f));
    vertices->push_back(osg::Vec3(1.0f, 1.0f, 1.0f));
    vertices->push_back(osg::Vec3(0.0f, 1.0f, 1.0f));
	      
    normals->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));
    normals->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));
    normals->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));
    normals->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));

    texcoords->push_back( osg::Vec2(0.0f, 0.0f) ); 
    texcoords->push_back( osg::Vec2(0.0f, 1.0f) ); 
    texcoords->push_back( osg::Vec2(1.0f, 1.0f) ); 
    texcoords->push_back( osg::Vec2(1.0f, 0.0f) ); 

 
    // ------ back
    vertices->push_back(osg::Vec3(0.0f, 1.0f, 0.0f));
    vertices->push_back(osg::Vec3(1.0f, 1.0f, 0.0f));
    vertices->push_back(osg::Vec3(1.0f, 1.0f, 1.0f));
    vertices->push_back(osg::Vec3(0.0f, 1.0f, 1.0f));
	      
    normals->push_back(osg::Vec3(0.0f, 1.0f, 0.0f));
    normals->push_back(osg::Vec3(0.0f, 1.0f, 0.0f));
    normals->push_back(osg::Vec3(0.0f, 1.0f, 0.0f));
    normals->push_back(osg::Vec3(0.0f, 1.0f, 0.0f));

    texcoords->push_back( osg::Vec2(0.0f, 0.0f) ); 
    texcoords->push_back( osg::Vec2(0.0f, 1.0f) ); 
    texcoords->push_back( osg::Vec2(1.0f, 1.0f) ); 
    texcoords->push_back( osg::Vec2(1.0f, 0.0f) ); 


    // ------ Bottom
    vertices->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
    vertices->push_back(osg::Vec3(1.0f, 0.0f, 0.0f));
    vertices->push_back(osg::Vec3(1.0f, 1.0f, 0.0f));
    vertices->push_back(osg::Vec3(0.0f, 1.0f, 0.0f));
	      
    normals->push_back(osg::Vec3(0.0f, 0.0f, -1.0f));
    normals->push_back(osg::Vec3(0.0f, 0.0f, -1.0f));
    normals->push_back(osg::Vec3(0.0f, 0.0f, -1.0f));
    normals->push_back(osg::Vec3(0.0f, 0.0f, -1.0f));

    texcoords->push_back( osg::Vec2(0.0f, 0.0f) ); 
    texcoords->push_back( osg::Vec2(0.0f, 1.0f) ); 
    texcoords->push_back( osg::Vec2(1.0f, 1.0f) ); 
    texcoords->push_back( osg::Vec2(1.0f, 0.0f) ); 


    // ------ Left
    vertices->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
    vertices->push_back(osg::Vec3(0.0f, 1.0f, 0.0f));
    vertices->push_back(osg::Vec3(0.0f, 1.0f, 1.0f));
    vertices->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));
	      
    normals->push_back(osg::Vec3(-1.0f, 0.0f, 0.0f));
    normals->push_back(osg::Vec3(-1.0f, 0.0f, 0.0f));
    normals->push_back(osg::Vec3(-1.0f, 0.0f, 0.0f));
    normals->push_back(osg::Vec3(-1.0f, 0.0f, 0.0f));

    texcoords->push_back( osg::Vec2(0.0f, 0.0f) ); 
    texcoords->push_back( osg::Vec2(0.0f, 1.0f) ); 
    texcoords->push_back( osg::Vec2(1.0f, 1.0f) ); 
    texcoords->push_back( osg::Vec2(1.0f, 0.0f) ); 


    // ------ Right
    vertices->push_back(osg::Vec3(1.0f, 0.0f, 0.0f));
    vertices->push_back(osg::Vec3(1.0f, 1.0f, 0.0f));
    vertices->push_back(osg::Vec3(1.0f, 1.0f, 1.0f));
    vertices->push_back(osg::Vec3(1.0f, 0.0f, 1.0f));
	      
    normals->push_back(osg::Vec3(1.0f, 0.0f, 0.0f));
    normals->push_back(osg::Vec3(1.0f, 0.0f, 0.0f));
    normals->push_back(osg::Vec3(1.0f, 0.0f, 0.0f));
    normals->push_back(osg::Vec3(1.0f, 0.0f, 0.0f));

    texcoords->push_back( osg::Vec2(0.0f, 0.0f) ); 
    texcoords->push_back( osg::Vec2(0.0f, 1.0f) ); 
    texcoords->push_back( osg::Vec2(1.0f, 1.0f) ); 
    texcoords->push_back( osg::Vec2(1.0f, 0.0f) ); 


    
    
    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
    geom->setVertexArray(vertices);
    geom->setNormalArray(normals, osg::Array::Binding::BIND_PER_VERTEX);
    geom->addPrimitiveSet( new osg::DrawArrays(GL_QUADS, 0, 24));
    geom->setTexCoordArray(0, texcoords.get());
    osgUtil::SmoothingVisitor::smooth(*geom);

    
    geom->setTexCoordArray(0, texcoords.get(), osg::Array::Binding::BIND_PER_VERTEX);


    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    osg::ref_ptr<osg::Image> image = osgDB::readImageFile("/usr/home/florian/Documents/Jeux/futur/stone_3_2048x2048.jpg");
    texture->setImage(image);
    texture->setUnRefImageDataAfterApply(true); 
    
    _node->addDrawable(geom);
    _node->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture);
  }

  Cube::~Cube()
  {
  }

}

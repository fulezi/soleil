

#include "Floor.hpp"

namespace Soleil
{

  static osg::ref_ptr<osg::Node> CreateFloor()
  {
    osg::ref_ptr<osg::Vec3Array> vertices  = new osg::Vec3Array;
    vertices->push_back( osg::Vec3(-0.5f, 0.0f,-0.5f) );
    vertices->push_back( osg::Vec3( 0.5f, 0.0f,-0.5f) );
    vertices->push_back( osg::Vec3( 0.5f, 0.0f, 0.5f) );
    vertices->push_back( osg::Vec3(-0.5f, 0.0f, 0.5f) );

    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
    normals->push_back( osg::Vec3(0.0f,-1.0f, 0.0f) );
    osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array;
    texcoords->push_back( osg::Vec2(0.0f, 0.0f) );
    texcoords->push_back( osg::Vec2(0.0f, 1.0f) );
    texcoords->push_back( osg::Vec2(1.0f, 1.0f) );
    texcoords->push_back( osg::Vec2(1.0f, 0.0f) );


    osg::ref_ptr<osg::Geometry> quad = new osg::Geometry;
    quad->setVertexArray( vertices.get() );
    quad->setNormalArray( normals.get() );
    quad->setNormalBinding( osg::Geometry::BIND_OVERALL );
    quad->setTexCoordArray( 0, texcoords.get() );
    quad->addPrimitiveSet( new osg::DrawArrays(GL_QUADS, 0, 4) );


    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    osg::ref_ptr<osg::Image> image =
      osgDB::readImageFile( "/usr/home/florian/jeux/OpenSceneGraph-Data/Images/lz.rgb" );
    texture->setImage( image.get() );


    osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform();

    mt->addChild(quad);
    mt->setName("MatrixTransformOctogone");


    // osg::ref_ptr<osg::Geode> root = new osg::Geode;
    // root->addDrawable( quad.get() );
    mt->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture.get() );


    return mt;
    
  }

  
  Floor::Floor(bool modified)
    : Mesh()
  {
    osg::ref_ptr<osg::Vec3Array> vertices  = new osg::Vec3Array;
    vertices->push_back( osg::Vec3(-0.5f, 0.0f,-0.5f) );
    vertices->push_back( osg::Vec3( 0.5f, 0.0f,-0.5f) );
    vertices->push_back( osg::Vec3( 0.5f, 0.0f, 0.5f) );
    vertices->push_back( osg::Vec3(-0.5f, 0.0f, 0.5f) );

    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
    normals->push_back( osg::Vec3(0.0f,-1.0f, 0.0f) );
    osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array;
    texcoords->push_back( osg::Vec2(0.0f, 0.0f) );
    texcoords->push_back( osg::Vec2(0.0f, 1.0f) );
    texcoords->push_back( osg::Vec2(1.0f, 1.0f) );
    texcoords->push_back( osg::Vec2(1.0f, 0.0f) );


    osg::ref_ptr<osg::Geometry> quad = new osg::Geometry;
    quad->setVertexArray( vertices.get() );
    quad->setNormalArray( normals.get() );
    quad->setNormalBinding( osg::Geometry::BIND_OVERALL );
    quad->setTexCoordArray( 0, texcoords.get() );
    quad->addPrimitiveSet( new osg::DrawArrays(GL_QUADS, 0, 4) );

    this->_node->addDrawable(quad);

    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    osg::ref_ptr<osg::Image> image =
      osgDB::readImageFile( "/usr/home/florian/jeux/OpenSceneGraph-Data/Images/lz.rgb" );
    texture->setImage( image.get() );    
    this->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture.get() );

    
  }

  
  Floor::~Floor(void)
  {
  }

  
};

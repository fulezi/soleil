

#include <algorithm>

#include <osg/Texture2D>
#include <osg/Geometry>
#include <osg/Geode>
#include <osgUtil/SmoothingVisitor>
#include <osgDB/ReadFile>
#include <osg/PositionAttitudeTransform>
#include <osg/Material>

#include "LevelReader.hpp"
#include "Cube.hpp"
#include "NPC.hpp"


#include <iostream>
#include <osg/io_utils>

namespace Soleil
{

  LevelReader::LevelReader()
  {
    //supportsExtension("level", "Text map format for Donjon");
  }

  LevelReader::~LevelReader()
  {
  }


  osg::ref_ptr<Level>  LevelReader::readFile(const std::string &file) const
  {    
    std::string fileName = osgDB::findDataFile(file);
    if (fileName.empty()) return nullptr; // TODO throw e?

    errno = 0;
    std::cout << "Stream: " << fileName.c_str() << "\n";
    std::ifstream stream;
    stream.exceptions(std::ifstream::failbit); //  | std::ifstream::badbit
    stream.open(fileName.c_str(), std::ios::in);
    if (!stream) return nullptr; // TODO throw e

    return readStream(stream);
  }
  
  osg::ref_ptr<Level> LevelReader::readStream(std::istream &stream) const
  {
    char	block;			// current reading block
    float	x = 0;			// Current position on x
    float	y = 0;			// Current position on y
    int		wallCount = 0; 		// Number of cubes
    float	maxX = 0;		// Max size of the map in X


    osg::ref_ptr<Soleil::Level> level = new Soleil::Level();
    std::string	line;
    LevelChunk chunkx;
    LevelChunk chunky;
    //chunky.textureSlot = 1;
    chunky.texturePath = "media/textures/Metal_seamless2_ch16.jpg";
    while(std::getline(stream, line))
      {
	std::stringstream linestream(line);

	while (linestream >> block)
	  {
	    /* TODO Configurable size for the blocks */
	    float posx = 1.0 * x; 
	    float posy = 1.0 * y;	      
	    float endx = posx + 1.0;
	    float endy = posy + 1.0;

	    bool blockFound = false;
	    switch (block)
	      {
	      case '.':
		break;
	      case 'D':
		level->_startingPosition = osg::Vec3(posx + .5, posy + .5, .3);
		blockFound = true;
		break;
	      case 'd':
		level->_startingOrientation = osg::Vec3(posx + .5, posy + .5, .3);
		blockFound = true;
		break;
	      case 'x':
		createCube(&chunkx, posx, posy, endx, endy);
		blockFound = true;
		wallCount++;
		break;
	      case 'y':
		createCube(&chunky, posx, posy, endx, endy);
		blockFound = true;
		wallCount++;
		break;
		// default:
		// 	blockFound = levelConfiguration(block);
		// 	break;
	      }
	    if (blockFound == false)
	      std::cout << "Unexpected character: " <<  block << std::endl;

	    x += 1.0;
	    maxX = std::max(x, maxX);
	  }
	x  = 0;
	y -= 1.0; // TODO Cube size
      }
    // TODO read failed?

    // Floor -------------
    level->vertices->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
    level->vertices->push_back(osg::Vec3(maxX, 0.0f, 0.0f));
    level->vertices->push_back(osg::Vec3(maxX, y, 0.0f));
    level->vertices->push_back(osg::Vec3(0, y, 0.0f));

    level->normals->push_back(osg::Vec3(0.0f, 0.0f, -1.0f));
    level->normals->push_back(osg::Vec3(0.0f, 0.0f, -1.0f));
    level->normals->push_back(osg::Vec3(0.0f, 0.0f, -1.0f));
    level->normals->push_back(osg::Vec3(0.0f, 0.0f, -1.0f));

    level->texcoords->push_back( osg::Vec2(0.0f, 0.0f) ); // 0
    level->texcoords->push_back( osg::Vec2(0.0f, 1.0f) ); // 1
    level->texcoords->push_back( osg::Vec2(1.0f, 1.0f) ); // 2
    level->texcoords->push_back( osg::Vec2(1.0f, 0.0f) ); // 3

  
    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
    geom->setVertexArray(level->vertices);
    geom->setNormalArray(level->normals, osg::Array::Binding::BIND_PER_VERTEX);
    geom->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4)); // +4 for the floor texture coordinates
    osgUtil::SmoothingVisitor::smooth(*geom);
    //

    // 
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable(geom);


    /* I added this in the hope to have the second texture appearing */
    // osg::Material* material = new osg::Material;
    // material->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);
    // material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
    // material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
    // material->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
    // material->setShininess(osg::Material::FRONT_AND_BACK, 64.0f);
    // level->getOrCreateStateSet()->setAttribute(material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);


    
    level->addChild(geode);
    level->addChild(chunkx.toGeometry());
    level->addChild(chunky.toGeometry());

    
    // Texture 1
    geom->setTexCoordArray(0, level->texcoords.get());
    {
      osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
      osg::ref_ptr<osg::Image> image = osgDB::readImageFile("media/textures/stone_3_2048x2048.jpg");
      texture->setImage(image);
      texture->setUnRefImageDataAfterApply(true);
      geom->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture);
    }
    //Texture 2
    // geom->setTexCoordArray(1, level->texcoords2.get());
    // {
    //   osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    //   osg::ref_ptr<osg::Image> image = osgDB::readImageFile("media/textures/Metal_seamless2_ch16.jpg");
    //   texture->setImage(image);
    //   texture->setUnRefImageDataAfterApply(true);
    //   level->getOrCreateStateSet()->setTextureAttributeAndModes(1, texture);
    // }
    
    std::cout << "done.\n";
    return level;
  }


  void LevelReader::createCube(LevelChunk *chunk,
			       float posx, float posy, float endx, float endy) const
  {
    chunk->vertices->push_back(osg::Vec3(posx, posy, 0.0f));
    chunk->vertices->push_back(osg::Vec3(endx, posy, 0.0f));
    chunk->vertices->push_back(osg::Vec3(endx, posy, 1.0f));
    chunk->vertices->push_back(osg::Vec3(posx, posy, 1.0f));

    chunk->normals->push_back(osg::Vec3(0.0f,-1.0f, 0.0f));
    chunk->normals->push_back(osg::Vec3(0.0f,-1.0f, 0.0f));
    chunk->normals->push_back(osg::Vec3(0.0f,-1.0f, 0.0f));
    chunk->normals->push_back(osg::Vec3(0.0f,-1.0f, 0.0f));

    chunk->texcoords->push_back( osg::Vec2(0.0f, 0.0f)); // 0
    chunk->texcoords->push_back( osg::Vec2(0.0f, 1.0f) ); // 1
    chunk->texcoords->push_back( osg::Vec2(1.0f, 1.0f) ); // 2
    chunk->texcoords->push_back( osg::Vec2(1.0f, 0.0f) ); // 3
  
  
    // ------ top
    chunk->vertices->push_back(osg::Vec3(posx, posy, 1.0f));
    chunk->vertices->push_back(osg::Vec3(endx, posy, 1.0f));
    chunk->vertices->push_back(osg::Vec3(endx, endy, 1.0f));
    chunk->vertices->push_back(osg::Vec3(posx, endy, 1.0f));

    chunk->normals->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));
    chunk->normals->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));
    chunk->normals->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));
    chunk->normals->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));

    chunk->texcoords->push_back( osg::Vec2(0.0f, 0.0f) ); // 0
    chunk->texcoords->push_back( osg::Vec2(0.0f, 1.0f) ); // 1
    chunk->texcoords->push_back( osg::Vec2(1.0f, 1.0f) ); // 2
    chunk->texcoords->push_back( osg::Vec2(1.0f, 0.0f) ); // 3
  
  
    // ------ back
    chunk->vertices->push_back(osg::Vec3(posx, endy, 0.0f));
    chunk->vertices->push_back(osg::Vec3(endx, endy, 0.0f));
    chunk->vertices->push_back(osg::Vec3(endx, endy, 1.0f));
    chunk->vertices->push_back(osg::Vec3(posx, endy, 1.0f));

    chunk->normals->push_back(osg::Vec3(0.0f, 1.0f, 0.0f));
    chunk->normals->push_back(osg::Vec3(0.0f, 1.0f, 0.0f));
    chunk->normals->push_back(osg::Vec3(0.0f, 1.0f, 0.0f));
    chunk->normals->push_back(osg::Vec3(0.0f, 1.0f, 0.0f));

    chunk->texcoords->push_back( osg::Vec2(0.0f, 0.0f) ); // 0
    chunk->texcoords->push_back( osg::Vec2(0.0f, 1.0f) ); // 1
    chunk->texcoords->push_back( osg::Vec2(1.0f, 1.0f) ); // 2
    chunk->texcoords->push_back( osg::Vec2(1.0f, 0.0f) ); // 3
  
  
    // ------ Bottom
    chunk->vertices->push_back(osg::Vec3(posx, posy, 0.0f));
    chunk->vertices->push_back(osg::Vec3(endx, posy, 0.0f));
    chunk->vertices->push_back(osg::Vec3(endx, endy, 0.0f));
    chunk->vertices->push_back(osg::Vec3(posx, endy, 0.0f));

    chunk->normals->push_back(osg::Vec3(0.0f, 0.0f, -1.0f));
    chunk->normals->push_back(osg::Vec3(0.0f, 0.0f, -1.0f));
    chunk->normals->push_back(osg::Vec3(0.0f, 0.0f, -1.0f));
    chunk->normals->push_back(osg::Vec3(0.0f, 0.0f, -1.0f));

    chunk->texcoords->push_back( osg::Vec2(0.0f, 0.0f) ); // 0
    chunk->texcoords->push_back( osg::Vec2(0.0f, 1.0f) ); // 1
    chunk->texcoords->push_back( osg::Vec2(1.0f, 1.0f) ); // 2
    chunk->texcoords->push_back( osg::Vec2(1.0f, 0.0f) ); // 3


    // ------ Left
    chunk->vertices->push_back(osg::Vec3(posx, posy, 0.0f));
    chunk->vertices->push_back(osg::Vec3(posx, endy, 0.0f));
    chunk->vertices->push_back(osg::Vec3(posx, endy, 1.0f));
    chunk->vertices->push_back(osg::Vec3(posx, posy, 1.0f));

    chunk->normals->push_back(osg::Vec3(-1.0f, 0.0f, 0.0f));
    chunk->normals->push_back(osg::Vec3(-1.0f, 0.0f, 0.0f));
    chunk->normals->push_back(osg::Vec3(-1.0f, 0.0f, 0.0f));
    chunk->normals->push_back(osg::Vec3(-1.0f, 0.0f, 0.0f));

    chunk->texcoords->push_back( osg::Vec2(0.0f, 0.0f) ); // 0
    chunk->texcoords->push_back( osg::Vec2(0.0f, 1.0f) ); // 1
    chunk->texcoords->push_back( osg::Vec2(1.0f, 1.0f) ); // 2
    chunk->texcoords->push_back( osg::Vec2(1.0f, 0.0f) ); // 3
  
  
    // ------ Right
    chunk->vertices->push_back(osg::Vec3(endx, posy, 0.0f));
    chunk->vertices->push_back(osg::Vec3(endx, endy, 0.0f));
    chunk->vertices->push_back(osg::Vec3(endx, endy, 1.0f));
    chunk->vertices->push_back(osg::Vec3(endx, posy, 1.0f));

    chunk->normals->push_back(osg::Vec3(1.0f, 0.0f, 0.0f));
    chunk->normals->push_back(osg::Vec3(1.0f, 0.0f, 0.0f));
    chunk->normals->push_back(osg::Vec3(1.0f, 0.0f, 0.0f));
    chunk->normals->push_back(osg::Vec3(1.0f, 0.0f, 0.0f));

    chunk->texcoords->push_back( osg::Vec2(0.0f, 0.0f) ); // 0
    chunk->texcoords->push_back( osg::Vec2(0.0f, 1.0f) ); // 1
    chunk->texcoords->push_back( osg::Vec2(1.0f, 1.0f) ); // 2
    chunk->texcoords->push_back( osg::Vec2(1.0f, 0.0f) ); // 3

  }


  osg::ref_ptr<osg::Geometry>  LevelChunk::toGeometry(void) const
  {
    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
    geom->setVertexArray(vertices);
    geom->setNormalArray(normals, osg::Array::Binding::BIND_PER_VERTEX);
    geom->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, normals->size()));

    geom->setTexCoordArray(textureSlot, texcoords.get());



    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    osg::ref_ptr<osg::Image> image = osgDB::readImageFile(texturePath);
    texture->setImage(image);
    texture->setUnRefImageDataAfterApply(true);
    geom->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture);
    return geom;
  }
} 

//REGISTER_OSGPLUGIN(level, LevelReader)

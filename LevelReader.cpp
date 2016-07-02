

#include <algorithm>

#include <osg/Texture2D>
#include <osg/Geometry>
#include <osg/Geode>
#include <osgUtil/SmoothingVisitor>
#include <osgDB/ReadFile>
#include <osg/PositionAttitudeTransform>
#include <osg/Material>
#include <osg/TexEnv>

#include "yaml-cpp/yaml.h"

#include "LevelReader.hpp"
#include "Cube.hpp"
#include "NPC.hpp"

#include "Sound.hpp"

#include <iostream>
#include <osg/io_utils>

namespace Soleil
{

  LevelReader::LevelReader()
  {
  }

  LevelReader::~LevelReader()
  {
  }


  osg::ref_ptr<Level> LevelReader::readYAML(const std::string &file, osg::ref_ptr<osg::Group> root) const
  {
    YAML::Node config = YAML::LoadFile(file);

    YAML::Node blocks = config["blocks"];
    if (!blocks.IsMap())
      throw "The Block section must be a list"; // TODO Clear exception with line number
    YAML::Node zones = config["zones"];
    if (!zones.IsMap())
      throw "The Block section must be a list"; // TODO Clear exception with line number
    

    std::map<char, LevelChunk> chunks;
    
    for(YAML::const_iterator it = blocks.begin(); it != blocks.end(); ++it) {
      std::string key = it->first.as<std::string>();
      std::string value = it->second.as<std::string>();

      if (key.size() != 1)
	throw "Bloks must be one characters"; // TODO Clear exception with line number

      char c = key[0];
      chunks[c].texturePath = value;
      std::cout << "Key: " << key << ", value: " << value << std::endl;

      /* TODO NLEV: if key appears in zone: */
      if (zones[c])
	{
	  chunks[c].nextZone = zones[c].as<std::string>();
	  std::cout << "Chunck " << c << " is a next zone: " << chunks[c].nextZone << "\n";
	}
    }

    std::string map = config["map"].as<std::string>();
    std::cout << map << "\n";

    char	block;			// current reading block
    float	x = 0;			// Current position on x
    float	y = 0;			// Current position on y
    int		wallCount = 0; 		// Number of cubes
    float	maxX = 0;		// Max size of the map in X
    int		npcId = 0;

    osg::ref_ptr<Soleil::Level> level = new Soleil::Level(root);
    for (int i = 0; i < map.size(); i++) {
      block = map[i];
      /* TODO Configurable size for the blocks */
      float posx = 1.0 * x; 
      float posy = 1.0 * y;	      
      float endx = posx + 1.0;
      float endy = posy + 1.0;
	    
      x += 1.0;
      /* keep the widest size for the floor */
      maxX = std::max(x, maxX);

      switch (block)
	{
	case '\n': // TODO FIXME Try with Windows carriage returns
	  x  = 0;
	  y -= 1.0; // TODO Cube size
	  break;
	case '.':
	  break;
	case 'D':
	  level->_startingPosition = osg::Vec3(posx + .5, posy + .5, .3);
	  break;
	case 'd':
	  level->_startingOrientation = osg::Vec3(posx + .5, posy + .5, .3);
	  break;
	case 'n':
	  {
	    npcId++;
	    osg::ref_ptr<NPC> ghoul = new NPC(osg::Vec3(posx + .5 , posy + .5 , 0), root);
	    ghoul->setScale(osg::Vec3(.6, .6, .6));
	    // //ghoul->setAttitude(osg::Quat(osg::DegreesToRadians(-90.0), osg::Vec3(0, 1, 0)));
	    // ghoul->setAttitude(osg::Quat(osg::DegreesToRadians(-90.0), osg::Vec3(1, 0, 0)));
	    ghoul->setName("GhoulNode_"+std::to_string(npcId));
	    level->addChild(ghoul);



	    osg::ref_ptr<UpdateSoundCallBack> updateSound = new UpdateSoundCallBack(*ghoul);
	    ghoul->addUpdateCallback(updateSound);

	    level->_sounds.push_back(updateSound);
	    
	  }
	  break;
	default:
	  std::map<char, LevelChunk>::iterator foundChunk = chunks.find(block);
	  if (foundChunk == chunks.end())
	    std::cout << "Unexpected character: " <<  block << std::endl; // TODO Line and col
	  else
	    {
	      createCube(&foundChunk->second, posx, posy, endx, endy);
	      wallCount++;
	    }
	  break;
	}
    }

      std::cout << NPC::ClassName  << "\n";

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
    level->texcoords->push_back( osg::Vec2(0.0f, maxX) ); // 1
    level->texcoords->push_back( osg::Vec2(y, maxX) ); // 2
    level->texcoords->push_back( osg::Vec2(y, 0.0f) ); // 3

    // roof -------------
    level->vertices->push_back(osg::Vec3(0.0f,	0.0f,	1.0f));
    level->vertices->push_back(osg::Vec3(maxX,	0.0f,	1.0f));
    level->vertices->push_back(osg::Vec3(maxX,	y,	1.0f));
    level->vertices->push_back(osg::Vec3(0,	y,	1.0f));

    level->normals->push_back(osg::Vec3(0.0f, 0.0f, -1.0f));
    level->normals->push_back(osg::Vec3(0.0f, 0.0f, -1.0f));
    level->normals->push_back(osg::Vec3(0.0f, 0.0f, -1.0f));
    level->normals->push_back(osg::Vec3(0.0f, 0.0f, -1.0f));

    level->texcoords->push_back( osg::Vec2(0.0f, 0.0f) ); // 0
    level->texcoords->push_back( osg::Vec2(0.0f, maxX) ); // 1
    level->texcoords->push_back( osg::Vec2(y, maxX) ); // 2
    level->texcoords->push_back( osg::Vec2(y, 0.0f) ); // 3


    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
    geom->setName("FloorGometry");
    geom->setVertexArray(level->vertices);
    geom->setNormalArray(level->normals, osg::Array::Binding::BIND_PER_VERTEX);
    geom->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 8)); // +4 for the floor texture coordinates and 4 for the roof
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
    for(std::map<char, LevelChunk>::iterator it = chunks.begin(); it != chunks.end(); ++it) {
      osg::ref_ptr<osg::Geometry> cube = it->second.toGeometry();
      
      level->addChild(cube);
      
      if (it->second.nextZone.length() > 0)
	level->_nextLevelZones.push_back(cube);
    }

    
    // Texture 1
    geom->setTexCoordArray(0, level->texcoords.get());
    {
      osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
      osg::ref_ptr<osg::Image> image = osgDB::readImageFile("media/textures/summoningwar/stone 1.png");
      texture->setImage(image);
      texture->setUnRefImageDataAfterApply(true);

      texture->setWrap(osg::Texture::WrapParameter::WRAP_S, osg::Texture::WrapMode::REPEAT);
      texture->setWrap(osg::Texture::WrapParameter::WRAP_T, osg::Texture::WrapMode::REPEAT);
      texture->setWrap(osg::Texture::WrapParameter::WRAP_R, osg::Texture::WrapMode::REPEAT);

      
      geom->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture);
    }

    return level;
  }

  void LevelReader::createCube(LevelChunk *chunk,
			       float posx, float posy, float endx, float endy) const
  {
    // ------ Front
    chunk->vertices->push_back(osg::Vec3(posx, posy, 0.0f));
    chunk->vertices->push_back(osg::Vec3(endx, posy, 0.0f));
    chunk->vertices->push_back(osg::Vec3(endx, posy, 1.0f));
    chunk->vertices->push_back(osg::Vec3(posx, posy, 1.0f));

    chunk->normals->push_back(osg::Vec3(0.0f,-1.0f, 0.0f));
    chunk->normals->push_back(osg::Vec3(0.0f,-1.0f, 0.0f));
    chunk->normals->push_back(osg::Vec3(0.0f,-1.0f, 0.0f));
    chunk->normals->push_back(osg::Vec3(0.0f,-1.0f, 0.0f));

    chunk->texcoords->push_back( osg::Vec2(0.0f, 0.0f)); // 0
    chunk->texcoords->push_back( osg::Vec2(1.0f, 0.0f) ); // 3
    chunk->texcoords->push_back( osg::Vec2(1.0f, 1.0f) ); // 2
    chunk->texcoords->push_back( osg::Vec2(0.0f, 1.0f) ); // 1
  
  
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
    chunk->texcoords->push_back( osg::Vec2(1.0f, 0.0f) ); // 3
    chunk->texcoords->push_back( osg::Vec2(1.0f, 1.0f) ); // 2
    chunk->texcoords->push_back( osg::Vec2(0.0f, 1.0f) ); // 1
  
  
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
    chunk->texcoords->push_back( osg::Vec2(1.0f, 0.0f) ); // 3
    chunk->texcoords->push_back( osg::Vec2(1.0f, 1.0f) ); // 2
    chunk->texcoords->push_back( osg::Vec2(0.0f, 1.0f) ); // 1
  
  
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
    chunk->texcoords->push_back( osg::Vec2(1.0f, 0.0f) ); // 3
    chunk->texcoords->push_back( osg::Vec2(1.0f, 1.0f) ); // 2
    chunk->texcoords->push_back( osg::Vec2(0.0f, 1.0f) ); // 1


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
    chunk->texcoords->push_back( osg::Vec2(1.0f, 0.0f) ); // 3
    chunk->texcoords->push_back( osg::Vec2(1.0f, 1.0f) ); // 2
    chunk->texcoords->push_back( osg::Vec2(0.0f, 1.0f) ); // 1

  
  
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
    chunk->texcoords->push_back( osg::Vec2(1.0f, 0.0f) ); // 3
    chunk->texcoords->push_back( osg::Vec2(1.0f, 1.0f) ); // 2
    chunk->texcoords->push_back( osg::Vec2(0.0f, 1.0f) ); // 1


  }


  osg::ref_ptr<osg::Geometry>  LevelChunk::toGeometry(void)// const
  {
    static int cubeId = 0;
    osg::ref_ptr<osg::Geometry> geom;
    //if (cubeId == 6)
    if (this->nextZone.length() > 0)
      {
	geom = new NextLevelZone(this->nextZone);
	std::cout << "Creating next zone: " << this->nextZone << "\n";
      }
    else
      geom = new osg::Geometry;
    geom->setVertexArray(vertices);
    geom->setNormalArray(normals, osg::Array::Binding::BIND_PER_VERTEX);
    geom->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, normals->size()));

    geom->setTexCoordArray(textureSlot, texcoords.get());

    

    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    osg::ref_ptr<osg::Image> image = osgDB::readImageFile(texturePath);
    texture->setImage(image);
    texture->setUnRefImageDataAfterApply(true);
    /* Deactivate the line on the textures */
    texture->setBorderWidth(1);
    geom->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture);

    geom->getOrCreateStateSet()->setMode(osg::TexEnv::BLEND, osg::StateAttribute::ON);

    geom->setName("CubeId_"+std::to_string(cubeId));
    cubeId++;

    // if (cubeId == 6)
    //   nextZone = "!! Sortie !!";

    // TODO NLEV: Use UserDataContainer to store the next level? Or use a callback that when tigeered, trigger the next level?
    
    
    return geom;
  }
} 

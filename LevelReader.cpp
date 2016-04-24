

#include <algorithm>

#include <osg/Texture2D>
#include <osg/Geometry>
#include <osg/Geode>
#include <osgUtil/SmoothingVisitor>
#include <osgDB/ReadFile>
#include <osg/PositionAttitudeTransform>

#include "LevelReader.hpp"
#include "Cube.hpp"


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
    float	x = 0;
    float	y = 0;
    int		i = 0;			// Number of cubes
    float	maxX = 0;		// Max size of the map in X


    osg::ref_ptr<Soleil::Level> level = new Soleil::Level();
    std::string	line;
    while(std::getline(stream, line))
      {
	std::stringstream linestream(line);

	while (linestream >> block)
	  {
	    float posx = 1.0 * x; 
	    float posy = 1.0 * y;	      
	    float endx = posx + 1.0;
	    float endy = posy + 1.0;

	    if (block == 'x')
	      {
	      
		createCube(level.get(), posx, posy, endx, endy);
	      
	      
		std::cout << i <<") New cube: "<< x << ", " << y << "\t[" << posx << ", " << endx << "]-[" << posy << ", " << endy << "]\n";
		i++;
	      }
	    else if (block == 'D')
	      {
		level->_startingPosition = osg::Vec3(posx + .5, posy + .5, .3);
	      }
	    else if (block == 'd')
	      {
		level->_startingOrientation = osg::Vec3(posx + .5, posy + .5, .3);
	      }
	    else if (block == '*')
	      {
		osg::ref_ptr<osg::Node> model =
		  osgDB::readNodeFile("/usr/home/florian/jeux/OpenSceneGraph-Data/axes.osgt");
		if (!model)
		  throw "Cannot read model";
		osg::ref_ptr<osg::PositionAttitudeTransform> g = new osg::PositionAttitudeTransform;
		g->setPosition(osg::Vec3(posx, posy, 0));
		g->addChild(model);
		level->addChild(g);
		
	      }
	    else
	      std::cout << "Unexpected character: " <<  block << std::endl;

	    x += 1.0;
	    maxX = std::max(x, maxX);
	  }
	x  = 0;
	y -= 1.0; // TODO Cube size
      }
    // TODO read failed?

    // Ground -------------
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
    geom->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 24*i+4)); // +4 for the ground texture
    geom->setTexCoordArray(0, level->texcoords.get());
    osgUtil::SmoothingVisitor::smooth(*geom);
    //
    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    osg::ref_ptr<osg::Image> image = osgDB::readImageFile("/usr/home/florian/Documents/Jeux/futur/stone_3_2048x2048.jpg");
    texture->setImage(image);
    texture->setUnRefImageDataAfterApply(true);
    // 
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable(geom);

    level->addChild(geode);
    level->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture);
 
    std::cout << "done.\n";
    return level;
  }


  void LevelReader::createCube(Soleil::Level *level,
			       float posx, float posy, float endx, float endy) const
  {
    // ------ Front
    level->vertices->push_back(osg::Vec3(posx, posy, 0.0f));
    level->vertices->push_back(osg::Vec3(endx, posy, 0.0f));
    level->vertices->push_back(osg::Vec3(endx, posy, 1.0f));
    level->vertices->push_back(osg::Vec3(posx, posy, 1.0f));

    level->normals->push_back(osg::Vec3(0.0f,-1.0f, 0.0f));
    level->normals->push_back(osg::Vec3(0.0f,-1.0f, 0.0f));
    level->normals->push_back(osg::Vec3(0.0f,-1.0f, 0.0f));
    level->normals->push_back(osg::Vec3(0.0f,-1.0f, 0.0f));

    level->texcoords->push_back( osg::Vec2(0.0f, 0.0f) ); // 0
    level->texcoords->push_back( osg::Vec2(0.0f, 1.0f) ); // 1
    level->texcoords->push_back( osg::Vec2(1.0f, 1.0f) ); // 2
    level->texcoords->push_back( osg::Vec2(1.0f, 0.0f) ); // 3
  
  
    // ------ top
    level->vertices->push_back(osg::Vec3(posx, posy, 1.0f));
    level->vertices->push_back(osg::Vec3(endx, posy, 1.0f));
    level->vertices->push_back(osg::Vec3(endx, endy, 1.0f));
    level->vertices->push_back(osg::Vec3(posx, endy, 1.0f));

    level->normals->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));
    level->normals->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));
    level->normals->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));
    level->normals->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));

    level->texcoords->push_back( osg::Vec2(0.0f, 0.0f) ); // 0
    level->texcoords->push_back( osg::Vec2(0.0f, 1.0f) ); // 1
    level->texcoords->push_back( osg::Vec2(1.0f, 1.0f) ); // 2
    level->texcoords->push_back( osg::Vec2(1.0f, 0.0f) ); // 3
  
  
    // ------ back
    level->vertices->push_back(osg::Vec3(posx, endy, 0.0f));
    level->vertices->push_back(osg::Vec3(endx, endy, 0.0f));
    level->vertices->push_back(osg::Vec3(endx, endy, 1.0f));
    level->vertices->push_back(osg::Vec3(posx, endy, 1.0f));

    level->normals->push_back(osg::Vec3(0.0f, 1.0f, 0.0f));
    level->normals->push_back(osg::Vec3(0.0f, 1.0f, 0.0f));
    level->normals->push_back(osg::Vec3(0.0f, 1.0f, 0.0f));
    level->normals->push_back(osg::Vec3(0.0f, 1.0f, 0.0f));

    level->texcoords->push_back( osg::Vec2(0.0f, 0.0f) ); // 0
    level->texcoords->push_back( osg::Vec2(0.0f, 1.0f) ); // 1
    level->texcoords->push_back( osg::Vec2(1.0f, 1.0f) ); // 2
    level->texcoords->push_back( osg::Vec2(1.0f, 0.0f) ); // 3
  
  
    // ------ Bottom
    level->vertices->push_back(osg::Vec3(posx, posy, 0.0f));
    level->vertices->push_back(osg::Vec3(endx, posy, 0.0f));
    level->vertices->push_back(osg::Vec3(endx, endy, 0.0f));
    level->vertices->push_back(osg::Vec3(posx, endy, 0.0f));

    level->normals->push_back(osg::Vec3(0.0f, 0.0f, -1.0f));
    level->normals->push_back(osg::Vec3(0.0f, 0.0f, -1.0f));
    level->normals->push_back(osg::Vec3(0.0f, 0.0f, -1.0f));
    level->normals->push_back(osg::Vec3(0.0f, 0.0f, -1.0f));

    level->texcoords->push_back( osg::Vec2(0.0f, 0.0f) ); // 0
    level->texcoords->push_back( osg::Vec2(0.0f, 1.0f) ); // 1
    level->texcoords->push_back( osg::Vec2(1.0f, 1.0f) ); // 2
    level->texcoords->push_back( osg::Vec2(1.0f, 0.0f) ); // 3


    // ------ Left
    level->vertices->push_back(osg::Vec3(posx, posy, 0.0f));
    level->vertices->push_back(osg::Vec3(posx, endy, 0.0f));
    level->vertices->push_back(osg::Vec3(posx, endy, 1.0f));
    level->vertices->push_back(osg::Vec3(posx, posy, 1.0f));

    level->normals->push_back(osg::Vec3(-1.0f, 0.0f, 0.0f));
    level->normals->push_back(osg::Vec3(-1.0f, 0.0f, 0.0f));
    level->normals->push_back(osg::Vec3(-1.0f, 0.0f, 0.0f));
    level->normals->push_back(osg::Vec3(-1.0f, 0.0f, 0.0f));

    level->texcoords->push_back( osg::Vec2(0.0f, 0.0f) ); // 0
    level->texcoords->push_back( osg::Vec2(0.0f, 1.0f) ); // 1
    level->texcoords->push_back( osg::Vec2(1.0f, 1.0f) ); // 2
    level->texcoords->push_back( osg::Vec2(1.0f, 0.0f) ); // 3
  
  
    // ------ Right
    level->vertices->push_back(osg::Vec3(endx, posy, 0.0f));
    level->vertices->push_back(osg::Vec3(endx, endy, 0.0f));
    level->vertices->push_back(osg::Vec3(endx, endy, 1.0f));
    level->vertices->push_back(osg::Vec3(endx, posy, 1.0f));

    level->normals->push_back(osg::Vec3(1.0f, 0.0f, 0.0f));
    level->normals->push_back(osg::Vec3(1.0f, 0.0f, 0.0f));
    level->normals->push_back(osg::Vec3(1.0f, 0.0f, 0.0f));
    level->normals->push_back(osg::Vec3(1.0f, 0.0f, 0.0f));

    level->texcoords->push_back( osg::Vec2(0.0f, 0.0f) ); // 0
    level->texcoords->push_back( osg::Vec2(0.0f, 1.0f) ); // 1
    level->texcoords->push_back( osg::Vec2(1.0f, 1.0f) ); // 2
    level->texcoords->push_back( osg::Vec2(1.0f, 0.0f) ); // 3

  }

} 

//REGISTER_OSGPLUGIN(level, LevelReader)

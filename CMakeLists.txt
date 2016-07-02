cmake_minimum_required (VERSION 3.2.3)
project (soleil)

set(CMAKE_CXX_STANDARD 14)

#########################
# Project dependencies  #
#########################
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules/")
find_package(OpenSceneGraph REQUIRED osg osgViewer osgGA osgDB  osgUtil osgAnimation osgManipulator osgText) # 3.5.1  .. PATHS ${OSG_PATH}

FIND_PACKAGE(CPPUNIT REQUIRED)
FIND_PACKAGE(YamlCpp REQUIRED)

find_library(OPENAL_LIBRARY
    NAMES openal
)

find_library(ALUT_LIBRARY
    NAMES alut
)


find_package(Boost
  COMPONENTS format
  )

###################
# Project sources #
###################
include_directories (SYSTEM
  ${OSG_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${YAML_INCLUDE_DIR}
  )

set(HDRS
  Mesh.hpp
  Floor.hpp
  Cube.hpp
  LevelReader.hpp
  Level.hpp
  FirstPersonManipulator.hpp
  NPC.hpp
  UpdateNPCVisitor.hpp
  NextLevelZoneCallBack.hpp
  ui/Container.hpp
  ui/Text.hpp
  ui/Window.hpp
  )

set(SRCS
  Mesh.cpp
  Floor.cpp
  Cube.cpp
  LevelReader.cpp
  Level.cpp
  FirstPersonManipulator.cpp
  NPC.cpp
  UpdateNPCVisitor.cpp
  NextLevelZoneCallBack.cpp
  ui/Container.cpp
  ui/Text.cpp
  ui/Window.cpp
  )

################
# Main Library #
################
add_library(soleil-lib ${HDRS} ${SRCS})

# target_link_libraries(soleil-lib
#   ${OSG_LIBRARY}
#   ${OPENSCENEGRAPH_LIBRARIES}
#   ${Boost_LIBRARIES}
#   )

##############
# Executable #
##############
add_executable(soleil
  main.cpp
  )
target_link_libraries(soleil
  soleil-lib
  ${OSG_LIBRARY}
  ${OPENSCENEGRAPH_LIBRARIES}
  ${Boost_LIBRARIES}
  ${YAMLCPP_LIBRARY}
  ${ALUT_LIBRARY}
  ${OPENAL_LIBRARY}
  )

########
# Test #
########
# enable_testing()
# add_subdirectory(tests)
# add_test(Test tests/UnitTests --log_level=message)

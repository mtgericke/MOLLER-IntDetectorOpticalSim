#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.8...3.18)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()
project(MOLLEROPT)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED vis_all ui_all COMPONENTS gdml qt)
  execute_process(COMMAND geant4-config --version
    OUTPUT_VARIABLE GEANT4_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  message(STATUS "Found Geant4 ${GEANT4_VERSION}")
else()
  find_package(Geant4 REQUIRED)
endif()
set(CMAKE_CXX_FLAGS "-g -Wall -pg -Wno-shadow -Wno-pedantic -Wno-unused-parameter -Wno-overloaded-virtual -Wno-sign-compare -Wno-unused-variable -Wno-unused-but-set-variable -Wno-switch")

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
include(${Geant4_USE_FILE})

#----------------------------------------------------------------------------
# Find ROOT (required package)
#
find_package(ROOT REQUIRED)

# ROOT version 6 required
if(ROOT_FOUND)
  STRING(REGEX MATCH "6.*" VERSION6MATCH ${ROOT_VERSION})
  if(NOT VERSION6MATCH)
    message(FATAL_ERROR "MOLLEROpt requires ROOT 6")
  endif()
endif()

# Include ROOT's CMake functions for dictionary generation
#  since root6.20, the file is renamed and included by default, so include
#  only when we find the *old* name
if(EXISTS "${ROOT_DIR}/modules/RootNewMacros.cmake")
  include("${ROOT_DIR}/modules/RootNewMacros.cmake")
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)



#----------------------------------------------------------------------------
# P01 requires shared libraries
#
if(NOT Geant4_shared_FOUND)
  message(FATAL_ERROR "MOLLEROPT must use shared libraries")
endif()

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${PROJECT_SOURCE_DIR}/include
                    ${Geant4_INCLUDE_DIR}
                    ${ROOT_INCLUDE_DIRS}
		    ${CMAKE_CURRENT_BINARY_DIR}/include)
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)
file(GLOB libsources ${PROJECT_SOURCE_DIR}/src/MOLLEROptPrimaryEvent.cc ${PROJECT_SOURCE_DIR}/src/MOLLEROptDetectorEvent.cc ${PROJECT_SOURCE_DIR}/src/MOLLEROptMainEvent.cc)
file(GLOB libheaders ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Generate dictionaries, add ROOT libraries properties
#
ROOT_GENERATE_DICTIONARY(MOLLEROptDict ${PROJECT_SOURCE_DIR}/include/MOLLEROptDictClasses.hh LINKDEF ${PROJECT_SOURCE_DIR}/include/MOLLEROptLinkDef.hh)


add_executable(MOLLEROpt MOLLEROpt.cc ${sources} ${headers} MOLLEROptDict.cxx)

#add_library(MOLLEROptDictionaries SHARED ${libsources} ${libheaders} MOLLEROptMainEventDict.cxx MOLLEROptDetectorEventDict.cxx MOLLEROptPrimaryEventDict.cxx)


add_library(MOLLEROptDictionaries SHARED ${sources} MOLLEROptDict.cxx)

target_link_libraries(MOLLEROptDictionaries PUBLIC ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
#add_executable(MOLLEROpt MOLLEROpt.cc ${sources} ${headers} )
target_link_libraries(MOLLEROpt PRIVATE MOLLEROptDictionaries PUBLIC ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

#target_link_libraries(libMOLLEROpt ${ROOT_LIBRARIES} ${Boost_LIBRARIES} )

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build P01. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(MOLLER_SCRIPTS
  macros/gui.mac
  macros/Qt.mac
  macros/myRun.mac
  macros/vis.mac
  macros/MakeScanMacros3.py
  data/OpticalPropertiesInputData.txt
)

foreach(_script ${MOLLER_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# Add program to the project targets
# (this avoids the need of typing the program name after make)
#
#add_custom_target(MOLLEROPT DEPENDS MOLLEROpt)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
#install(TARGETS MOLLEROpt DESTINATION bin)
#install(TARGETS MOLLEROptDictionaries DESTINATION lib)
#install(FILES ${PROJECT_BINARY_DIR}/MOLLEROptDictClasses_rdict.pcm DESTINATION lib)

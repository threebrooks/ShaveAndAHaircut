cmake_minimum_required(VERSION 3.7)

if (NOT DEFINED GODEC_ROOT)
  message(FATAL_ERROR "Define GODEC_ROOT")
endif()

if (NOT DEFINED TSF_ROOT)
  message(FATAL_ERROR "Define TSF_ROOT")
endif()

project(godec_SAAH)

# Default optimization
if (NOT CMAKE_BUILD_TYPE) 
 set(CMAKE_BUILD_TYPE Release) 
endif() 

set(GCC_MINIMAL_VERSION 4.9.0)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${GCC_MINIMAL_VERSION})
    message(FATAL_ERROR "Insufficient gcc version: ${CMAKE_CXX_COMPILER_VERSION}. Version ${GCC_MINIMAL_VERSION} or above is required.")
  endif()
endif()

add_compile_options(-Wall -Wno-sign-compare -Wno-unused-local-typedefs -Winit-self -fno-omit-frame-pointer -DEIGEN_NO_DEBUG) 
add_compile_options(-pthread)
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG") 

find_program(CCACHE_PROGRAM ccache)
if(DEFINED ENV{CCACHE_DIR} AND CCACHE_PROGRAM)
  message(STATUS "CCACHE_DIR environment variable set to '$ENV{CCACHE_DIR}', will use ${CCACHE_PROGRAM}")
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON)
set(Boost_USE_MULTITHREADED ON)

find_package(Boost
  1.55.0      # Minimum or EXACT version e.g. 1.36.0
  REQUIRED             # Fail with error if Boost is not found
  COMPONENTS date_time atomic regex system serialization filesystem iostreams timer chrono program_options thread 
  )

find_package(PkgConfig)
if(DEFINED Eigen3_INCLUDE_DIRS)
else()
  find_package(Eigen3 3.2.92 EXACT REQUIRED) # Make sure this is the same version as the precompiled Godec version
endif()

find_package(Java REQUIRED)
find_package(Java COMPONENTS REQUIRED Runtime)
find_package(Java COMPONENTS REQUIRED Development)
find_package(JNI REQUIRED)
find_package(PythonLibs 3.3 REQUIRED)
find_package(ZLIB REQUIRED)

if(PLATFORM STREQUAL "RaspberryPi")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -Wno-psabi ${RASPBERRY_PI_OPT_FLAGS} -rdynamic -mhard-float")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Ofast -Wno-psabi")
endif()
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-undefined")

add_definitions(
  -DLINUX_MACHINE=1
  -DEIGEN_NO_DEBUG 
  -D${PLATFORM}
  )

file(GLOB GODEC_LIBS ${GODEC_ROOT}/lib/*.a)

include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${Boost_INCLUDE_DIR}
  ${Eigen3_INCLUDE_DIRS}
  ${JNI_INCLUDE_DIRS}
  ${JNI_INCLUDE_DIRS}/linux
  ${ZLIB_INCLUDE_DIRS}
  ${GODEC_ROOT}/include
  ${GODEC_ROOT}/include/core_components
  ${PYTHON_INCLUDE_DIRS}
  ${TSF_ROOT}
  )

include_directories(${GZSTREAM}/include)
set(SOURCE_FILES
  src/SAAHDetect.h
  src/SAAHDetect.cc
  src/godec_SAAH.cc
  )

add_library(godec_SAAH SHARED ${SOURCE_FILES})

if(PLATFORM STREQUAL "RaspberryPi")
  set(PILIBS -lwiringPi)
endif()

set(LINKER_LIBS -Wl,--no-undefined -ldl -lrt -lasound ${PILIBS} ${GODEC_LIBS} ${PYTHON_LIBRARIES} ${ZLIB_LIBRARY})

target_link_libraries(godec_SAAH
	-Wl,--start-group
	${LINKER_LIBS}
        Boost::program_options Boost::regex Boost::system Boost::serialization Boost::filesystem Boost::iostreams Boost::timer Boost::chrono Boost::thread
	-Wl,--end-group
        )
set_target_properties(godec_SAAH PROPERTIES OUTPUT_NAME "godec_SAAH")

install (TARGETS godec_SAAH DESTINATION .)
install (FILES ${PROJECT_SOURCE_DIR}/env.sh DESTINATION .)
install (DIRECTORY ${GODEC_ROOT}/ DESTINATION Godec)
install (DIRECTORY ${BOOST_LIBRARYDIR} DESTINATION Godec/bin PATTERN *.so )


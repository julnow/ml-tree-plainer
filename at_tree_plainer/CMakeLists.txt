cmake_minimum_required(VERSION 3.0)
project(ATP CXX)
if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 11)
endif()
set(PROJECT_VERSION 1.0)

macro(add_target_property _target _prop _value)
   get_target_property(_oldprop "${_target}" ${_prop})
   if(NOT _oldprop)
      set_target_properties("${_target}" PROPERTIES ${_prop} "${_value}")
   else(NOT _oldprop)
      set_target_properties("${_target}" PROPERTIES ${_prop} "${_oldprop} ${_value}")
   endif(NOT _oldprop)
endmacro(add_target_property)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
message(STATUS "Using C++${CMAKE_CXX_STANDARD}")

# by default build optimized code with debug symbols
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE RELWITHDEBINFO)
endif ()

# in DEBUG mode make verbose Makefile
if (CMAKE_BUILD_TYPE MATCHES DEBUG)
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif ()

# set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb -g -DDEBUG -D__DEBUG -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb -DDEBUG -D__DEBUG -Wall")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELEASE} -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -ftree-vectorize -ffast-math -DNODEBUG")
message(STATUS "Using CXX flags for ${CMAKE_BUILD_TYPE}: ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}}")
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

find_package(AnalysisTree REQUIRED)
find_package(ROOT REQUIRED RIO)

if (ROOT_FOUND)
    message(STATUS "Using ROOT: ${ROOT_VERSION} <${ROOT_CONFIG}>")
    include_directories(${CMAKE_SOURCE_DIR} ${ROOT_INCLUDE_DIR} ${ROOT_INCLUDE_DIRS})
    include(${ROOT_USE_FILE})
endif (ROOT_FOUND)

include_directories(${CMAKE_SOURCE_DIR} ${AnalysisTree_INCLUDE_DIR})

#************
set(SOURCES
  ATreePlainer.cpp
  )

string(REPLACE ".cpp" ".hpp" HEADERS "${SOURCES}")
add_library(ATP SHARED ${SOURCES} G__ATP.cxx)
        
if(ROOT_FOUND)
  ROOT_GENERATE_DICTIONARY(G__ATP ${HEADERS} LINKDEF ATPLinkDef.h OPTIONS "-DDO_TPCCATRACKER_EFF_PERFORMANCE")
  include_directories(${CMAKE_SOURCE_DIR})
  target_link_libraries(ATP ${ROOT_LIBRARIES} AnalysisTreeBase AnalysisTreeInfra)
endif(ROOT_FOUND)  
add_target_property(ATP COMPILE_FLAGS "-DDO_TPCCATRACKER_EFF_PERFORMANCE")

add_executable(main main.cpp)
add_dependencies(main ATP)
add_target_property(main COMPILE_FLAGS "-DDO_TPCCATRACKER_EFF_PERFORMANCE")
target_link_libraries(main ATP)

install(TARGETS ATP EXPORT ATPTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
        )

install(
        FILES
        ${HEADERS}
        DESTINATION
        include
        COMPONENT
        Devel
)

set(PCM_FILE_NAME libATP)

install(
        FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PCM_FILE_NAME}_rdict.pcm"
        DESTINATION
        lib
        OPTIONAL
)

install (TARGETS main RUNTIME DESTINATION bin)
#************

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/ATP/ATPConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)

export(EXPORT ATPTargets
        FILE "${CMAKE_CURRENT_BINARY_DIR}/ATP/ATPTargets.cmake")

message(STATUS "ATPTargets: ${ATPTargets}")
        
set(ConfigPackageLocation lib/cmake/ATP)

set(ATP_INCLUDE_DIR "include")
set(ATP_LIBRARY_DIR "lib")

configure_package_config_file(
        ATPConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/ATP/ATPConfig.cmake
        INSTALL_DESTINATION ${ConfigPackageLocation}
        PATH_VARS ATP_INCLUDE_DIR ATP_LIBRARY_DIR
)

install(EXPORT ATPTargets
        FILE
        ATPTargets.cmake
        DESTINATION
        ${ConfigPackageLocation}
        )

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/ATP/ATPConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/ATP/ATPConfigVersion.cmake"
        DESTINATION
        ${ConfigPackageLocation}
        COMPONENT
        Devel
        )

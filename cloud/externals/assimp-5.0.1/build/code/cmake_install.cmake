# Install script for directory: B:/dev/Cloud/cloud/externals/assimp-5.0.1/code

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Assimp")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "B:/dev/Cloud/cloud/externals/assimp-5.0.1/build/code/Debug/assimp-vc142-mtd.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "B:/dev/Cloud/cloud/externals/assimp-5.0.1/build/code/Release/assimp-vc142-mt.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "B:/dev/Cloud/cloud/externals/assimp-5.0.1/build/code/MinSizeRel/assimp-vc142-mt.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "B:/dev/Cloud/cloud/externals/assimp-5.0.1/build/code/RelWithDebInfo/assimp-vc142-mt.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "B:/dev/Cloud/cloud/externals/assimp-5.0.1/build/code/Debug/assimp-vc142-mtd.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "B:/dev/Cloud/cloud/externals/assimp-5.0.1/build/code/Release/assimp-vc142-mt.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "B:/dev/Cloud/cloud/externals/assimp-5.0.1/build/code/MinSizeRel/assimp-vc142-mt.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "B:/dev/Cloud/cloud/externals/assimp-5.0.1/build/code/RelWithDebInfo/assimp-vc142-mt.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xassimp-devx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp" TYPE FILE FILES
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/anim.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/aabb.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/ai_assert.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/camera.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/color4.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/color4.inl"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/build/code/../include/assimp/config.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/defs.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/Defines.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/cfileio.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/light.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/material.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/material.inl"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/matrix3x3.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/matrix3x3.inl"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/matrix4x4.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/matrix4x4.inl"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/mesh.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/pbrmaterial.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/postprocess.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/quaternion.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/quaternion.inl"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/scene.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/metadata.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/texture.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/types.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/vector2.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/vector2.inl"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/vector3.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/vector3.inl"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/version.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/cimport.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/importerdesc.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/Importer.hpp"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/DefaultLogger.hpp"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/ProgressHandler.hpp"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/IOStream.hpp"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/IOSystem.hpp"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/Logger.hpp"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/LogStream.hpp"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/NullLogger.hpp"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/cexport.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/Exporter.hpp"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/DefaultIOStream.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/DefaultIOSystem.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/ZipArchiveIOSystem.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/SceneCombiner.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/fast_atof.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/qnan.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/BaseImporter.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/Hash.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/MemoryIOWrapper.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/ParsingUtils.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/StreamReader.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/StreamWriter.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/StringComparison.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/StringUtils.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/SGSpatialSort.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/GenericProperty.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/SpatialSort.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/SkeletonMeshBuilder.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/SmoothingGroups.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/SmoothingGroups.inl"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/StandardShapes.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/RemoveComments.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/Subdivision.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/Vertex.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/LineSplitter.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/TinyFormatter.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/Profiler.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/LogAux.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/Bitmap.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/XMLTools.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/IOStreamBuffer.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/CreateAnimMesh.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/irrXMLWrapper.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/BlobIOSystem.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/MathFunctions.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/Macros.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/Exceptional.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/ByteSwapper.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xassimp-devx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp/Compiler" TYPE FILE FILES
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/Compiler/pushpack1.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/Compiler/poppack1.h"
    "B:/dev/Cloud/cloud/externals/assimp-5.0.1/code/../include/assimp/Compiler/pstdint.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "B:/dev/Cloud/cloud/externals/assimp-5.0.1/build/code/Debug/assimp-vc142-mtd.pdb")
  endif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "B:/dev/Cloud/cloud/externals/assimp-5.0.1/build/code/RelWithDebInfo/assimp-vc142-mt.pdb")
  endif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
endif()


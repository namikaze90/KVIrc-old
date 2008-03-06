#CMakeLists for /

# Name of the project
PROJECT(kvirc)

# Minimum cmake version
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.0)

# Some vars
SET(KVI_PACKAGE "kvirc")
SET(KVI_VERSION "4.0.0")
SET(KVI_VERSION_BRANCH "4.0")

# Check for Qt4
#FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${CMAKE_ROOT}/Modules/FindQt4.cmake)
#INCLUDE(${QT_USE_FILE})
#INCLUDE(${QT_USE_QT3SUPPORT})

# Check for Perl
#FIND_PACKAGE(Perl REQUIRED)
INCLUDE(${CMAKE_ROOT}/Modules/FindPerl.cmake)

# Check for OpenSSL
#FIND_PACKAGE(OpenSSL REQUIRED)
INCLUDE(${CMAKE_ROOT}/Modules/FindOpenSSL.cmake)

# Check for ZLib
INCLUDE(${CMAKE_ROOT}/Modules/FindZLIB.cmake)

# Check for X11
INCLUDE(${CMAKE_ROOT}/Modules/FindX11.cmake)

# Check for libpng
INCLUDE(${CMAKE_ROOT}/Modules/FindPNG.cmake)

# Check for Doxygen
INCLUDE(${CMAKE_ROOT}/Modules/FindDoxygen.cmake)

# Check for Cygwin
INCLUDE(${CMAKE_ROOT}/Modules/FindCygwin.cmake)


# Search for subdirectories
SUBDIRS(src data)

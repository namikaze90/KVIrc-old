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
FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

# Check for Perl
FIND_PACKAGE(Perl REQUIRED)

# Search for subdirectories
SUBDIRS(src data)

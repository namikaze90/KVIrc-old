# Name of the project
PROJECT(kvirc)

# Minimum cmake version
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.0)

# Check for Qt4
FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

# Search for subdirectories
SUBDIRS(src data)

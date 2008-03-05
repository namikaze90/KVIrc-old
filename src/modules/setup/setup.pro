TARGET = kvisetup
 
HEADERS += setupwizard.h
 						
SOURCES += libkvisetup.cpp setupwizard.cpp
 						
win32 {
LIBS = -lole32 -luuid
}

include(../module.pri)
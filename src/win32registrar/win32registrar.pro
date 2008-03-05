include(../../config.pri)

CONFIG   += precompile_header thread
LANGUAGE  = C++
TEMPLATE = app
TARGET = win32registrar

DEFINES += _UNICODE

CONFIG(debug, debug|release) {
     DESTDIR = ../../bin/debug/
 } else {
     DESTDIR = ../../bin/release/
 }
 
HEADERS += win32registrar.h
    
    
SOURCES += win32registrar.cpp

LIBS += -lshlwapi

RC_FILE = win32registrar.rc

target.path = ../../bin/image/
INSTALLS += target 

CONFIG(debug, debug|release) {
    MOC_DIR      = ../../build/$$TARGET/debug/moc
		OBJECTS_DIR  = ../../build/$$TARGET/debug/obj
 } else {
    MOC_DIR      = ../../build/$$TARGET/release/moc
		OBJECTS_DIR  = ../../build/$$TARGET/release/obj
 }

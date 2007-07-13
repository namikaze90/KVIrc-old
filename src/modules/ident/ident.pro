TARGET = kviident
 
HEADERS += libkviident.h
 						
SOURCES += libkviident.cpp
 						
win32 {
	LIBS += -lws2_32
}

include(../module.pri)
TARGET = kvisnd
 
HEADERS += libkvisnd.h
 						
SOURCES += libkvisnd.cpp
 				
win32{		
	LIBS += -lwinmm
}

include(../module.pri)
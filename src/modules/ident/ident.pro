TARGET = kviident
 
HEADERS += libkviident.h
 						
SOURCES += libkviident.cpp
 						
mac {
	LIBS += -lws2_32
}

include(../module.pri)
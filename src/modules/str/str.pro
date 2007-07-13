TARGET = kvistr
 
SOURCES += libkvistr.cpp

win32{
	LIBS += -leay32 \
	    -lssleay32 
}

mac {
 LIBS += -lssl -lcrypto
}			

include(../module.pri)
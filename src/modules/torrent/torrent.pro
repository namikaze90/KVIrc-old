TARGET = kviwindow

HEADERS += tc_statusbarapplet.h \
	tc_ktorrentdcopinterface.h \
	tc_interface.h

SOURCES += libkvitorrent.cpp \
	tc_statusbarapplet.cpp \
	tc_ktorrentdcopinterface.cpp \
	tc_interface.cpp

include(../module.pri)
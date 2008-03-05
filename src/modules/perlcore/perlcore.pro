TARGET = kviperlcore

win32{
	INCLUDEPATH += $(PERLDIR)/lib/CORE/
	LIBS += -lperl58
}

mac {
        INCLUDEPATH += $(PERLDIR)/CORE/
        LIBS += -lperl -L$(PERLDIR)/CORE/
}

 
HEADERS += perlcoreinterface.h
 						
SOURCES += libkviperlcore.cpp
 						
include(../module.pri)
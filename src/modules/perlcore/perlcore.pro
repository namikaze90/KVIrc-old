TARGET = kviperlcore

win32{
	INCLUDEPATH += $(PERLDIR)/lib/CORE/
	LIBS += -lperl58
}

mac {
        INCLUDEPATH += /System/Library/Perl/5.8.6/darwin-thread-multi-2level/CORE/
        LIBS += -lperl -L/System/Library/Perl/5.8.6/darwin-thread-multi-2level/CORE/
}

 
HEADERS += perlcoreinterface.h
 						
SOURCES += libkviperlcore.cpp
 						
include(../module.pri)
TARGET = kvitrayicon

HEADERS += libkvitrayicon.h trayicon.h

SOURCES += libkvitrayicon.cpp trayicon.cpp trayicon_x11.cpp trayicon_mac.cpp trayicon_win.cpp

include(../module.pri)

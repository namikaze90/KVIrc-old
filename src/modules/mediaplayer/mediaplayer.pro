TARGET = kvimediaplayer
 
HEADERS += mp_amarokinterface.h \
		mp_dcopinterface.h \
		mp_interface.h \
		mp_jukinterface.h \
		mp_mp3.h \
		mp_winampinterface.h \
		mp_xmmsinterface.h  \
		mp_amipinterface.h \
		mp_audaciousinterface.h
 						
SOURCES += libkvimediaplayer.cpp \
		mp_amarokinterface.cpp \
		mp_dcopinterface.cpp \
		mp_interface.cpp \
		mp_jukinterface.cpp \
		mp_mp3.cpp \
		mp_winampinterface.cpp \
		mp_xmmsinterface.cpp \
		mp_amipinterface.cpp \
		mp_audaciousinterface.cpp
 						
include(../module.pri)

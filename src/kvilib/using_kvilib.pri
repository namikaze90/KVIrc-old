INCLUDEPATH += ../kvilib/tal/ \
    ../kvilib/config/ \
    ../kvilib/core/ \
    ../kvilib/ext/ \
    ../kvilib/file/ \
    ../kvilib/irc/ \
    ../kvilib/net/ \
    ../kvilib/system/
    
!contains( DEFINES, __KVILIB__ ) {
      unix: LIBS += -lkvilib
	  mac: LIBS += -lkvilib
	  win32: LIBS += -lkvilib4
  } 

unix {
	CONFIG += qdbus
}


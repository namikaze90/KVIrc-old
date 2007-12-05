include(../config.pri)

TEMPLATE      = subdirs

DESTDIR = ../bin/image

unix {

	config.path = $${KVI_INSTALL_PREFIX}/share/$${KVI_PACKAGE}/$${KVI_VERSION_BRANCH}/config
	config.files = config/*.kvc config/preinstalled.kvc.win32-example
	
	defscript.path = $${KVI_INSTALL_PREFIX}/share/$${KVI_PACKAGE}/$${KVI_VERSION_BRANCH}/defscript
	defscript.files = defscript/*.kvs
	
	deftheme.path = $${KVI_INSTALL_PREFIX}/share/$${KVI_PACKAGE}/$${KVI_VERSION_BRANCH}/deftheme/silverirc
	deftheme.files = themes/silverirc/*.png deftheme/silverirc/*.kvc
	
	pics.path = $${KVI_INSTALL_PREFIX}/share/$${KVI_PACKAGE}/$${KVI_VERSION_BRANCH}/pics
	pics.files = pics/*.png pics/coresmall/*.png
	
	coresmall.path = $${KVI_INSTALL_PREFIX}/share/$${KVI_PACKAGE}/$${KVI_VERSION_BRANCH}/pics/coresmall
	coresmall.files = pics/coresmall/*.png
	
	msgcolors.path = $${KVI_INSTALL_PREFIX}/share/$${KVI_PACKAGE}/$${KVI_VERSION_BRANCH}/msgcolors
	msgcolors.files = msgcolors/*.msgclr
}

win32 {
	config.path = $$DESTDIR/config
	config.files = config/*.kvc config/preinstalled.kvc.win32-example
	
	defscript.path = $$DESTDIR/defscript
	defscript.files = defscript/*.kvs
	
	deftheme.path = $$DESTDIR/deftheme/silverirc
	deftheme.files = themes/silverirc/*.png deftheme/silverirc/*.kvc
	
	pics.path = $$DESTDIR/pics
	pics.files = pics/*.png pics/coresmall/*.png
	
	coresmall.path = $$DESTDIR/pics/coresmall
	coresmall.files = pics/coresmall/*.png
	
	msgcolors.path = $$DESTDIR/msgcolors
	msgcolors.files = msgcolors/*.msgclr
}

mac {
	config.path = $$DESTDIR/config
	config.files = config/*.kvc config/preinstalled.kvc.win32-example
	
	defscript.path = $$DESTDIR/defscript
	defscript.files = defscript/*.kvs
	
	deftheme.path = $$DESTDIR/deftheme/silverirc
	deftheme.files = themes/silverirc/*.png deftheme/silverirc/*.kvc
	
	pics.path = $$DESTDIR/pics
	pics.files = pics/*.png pics/coresmall/*.png
	
	coresmall.path = $$DESTDIR/pics/coresmall
	coresmall.files = pics/coresmall/*.png
	
	msgcolors.path = $$DESTDIR/msgcolors
	msgcolors.files = msgcolors/*.msgclr
}
INSTALLS += config defscript deftheme pics coresmall msgcolors

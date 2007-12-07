include(config.pri)

TEMPLATE = subdirs
SUBDIRS = src data


win32 {

	CONFIG(debug, debug|release) {
		D = d
	} else {
		D = 
	}

	dlls.path = bin/image
	dlls.files = $$[QT_INSTALL_LIBS]/Qt3Support$${D}4.dll \
				$$[QT_INSTALL_LIBS]/QtNetwork$${D}4.dll \
				$$[QT_INSTALL_LIBS]/QtSql$${D}4.dll \
				$$[QT_INSTALL_LIBS]/QtGui$${D}4.dll \
				$$[QT_INSTALL_LIBS]/QtXml$${D}4.dll \
				$$[QT_INSTALL_LIBS]/QtCore$${D}4.dll
	
	imageformats.path = bin/image/qt-plugins/imageformats
	imageformats.files = $$[QT_INSTALL_PLUGINS]/imageformats/qtiff$${D}4.dll \
				$$[QT_INSTALL_PLUGINS]/imageformats/qsvg$${D}4.dll \
				$$[QT_INSTALL_PLUGINS]/imageformats/qmng$${D}4.dll \
				$$[QT_INSTALL_PLUGINS]/imageformats/qgif$${D}4.dll \
				$$[QT_INSTALL_PLUGINS]/imageformats/qjpeg$${D}4.dll 
	
	iconengines.path = bin/image/qt-plugins/iconengines
	iconengines.files = $$[QT_INSTALL_PLUGINS]/iconengines/qsvg$${D}4.dll

	INSTALLS += dlls imageformats
}

unix {

	doc.path = $${KVI_INSTALL_PREFIX}/doc/$${KVI_PACKAGE}-$${KVI_VERSION_BRANCH}/
	doc.files = doc/*.txt

	license.path = $${KVI_INSTALL_PREFIX}/share/$${KVI_PACKAGE}/$${KVI_VERSION_BRANCH}/license
	license.files = doc/COPYING

	stuff.path = $${KVI_INSTALL_PREFIX}/doc/$${KVI_PACKAGE}-$${KVI_VERSION_BRANCH}/
	stuff.files = ChangeLog FAQ
}

win32 {
	doc.path = bin/image/doc
	doc.files = doc/*.txt

	license.path = bin/image/license
	license.files = doc/COPYING

	stuff.path = bin/image/
	stuff.files = ChangeLog FAQ
}

mac {
	doc.path = bin/image/doc
	doc.files = doc/*.txt

	license.path = bin/image/license
	license.files = doc/COPYING

	stuff.path = bin/image/
	stuff.files = ChangeLog FAQ
}

INSTALLS += doc license stuff
CONFIG += qdbus


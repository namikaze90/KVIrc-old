TEMPLATE      = subdirs
CONFIG += release
SUBDIRS = src data

win32 {
	dlls.path = bin/image
	dlls.files = $$[QT_INSTALL_LIBS]/Qt3Support4.dll \
				$$[QT_INSTALL_LIBS]/QtNetwork4.dll \
				$$[QT_INSTALL_LIBS]/QtSql4.dll \
				$$[QT_INSTALL_LIBS]/QtGui4.dll \
				$$[QT_INSTALL_LIBS]/QtXml4.dll \
				$$[QT_INSTALL_LIBS]/QtCore4.dll
	
	imageformats.path = bin/image/qt-plugins/imageformats
	imageformats.files = $$[QT_INSTALL_PLUGINS]/imageformats/qtiff4.dll \
				$$[QT_INSTALL_PLUGINS]/imageformats/qsvg4.dll \
				$$[QT_INSTALL_PLUGINS]/imageformats/qmng4.dll \
				$$[QT_INSTALL_PLUGINS]/imageformats/qgif4.dll \
				$$[QT_INSTALL_PLUGINS]/imageformats/qjpeg4.dll 
	
	iconengines.path = bin/image/qt-plugins/iconengines
	iconengines.files = $$[QT_INSTALL_PLUGINS]/iconengines/qsvg4.dll
	
	INSTALLS += dlls imageformats
}

doc.path = bin/image/doc
doc.files = doc/*.txt

license.path = bin/image/license
license.files = doc/COPYING

stuff.path = bin/image/
stuff.files = ChangeLog FAQ

INSTALLS += doc license stuff
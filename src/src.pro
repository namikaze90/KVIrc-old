TEMPLATE = subdirs
CONFIG += ordered

unix {
	CONFIG += qdbus
}

SUBDIRS = kvilib \
    kvirc \
    modules

win32{
    SUBDIRS += win32registrar
}

TEMPLATE = subdirs
CONFIG += ordered qdbus
SUBDIRS = kvilib \
    kvirc \
    modules

win32{
    SUBDIRS += win32registrar
}

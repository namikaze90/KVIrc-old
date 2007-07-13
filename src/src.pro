TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = kvilib \
    kvirc \
    modules

win32{
    SUBDIRS += win32registrar
}
include(../config.pri)

TEMPLATE      = subdirs

DESTDIR = ../bin/image

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

INSTALLS += config defscript deftheme pics coresmall msgcolors
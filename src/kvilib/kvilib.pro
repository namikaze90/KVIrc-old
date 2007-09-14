include(../../config.pri)
CONFIG += precompile_header \
    thread \
    dll
LANGUAGE = C++
TEMPLATE = lib
TARGET = kvilib
QT += core \
    gui \
    qt3support

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = pch.h
CONFIG(debug, debug|release):DESTDIR = ../../bin/debug/
else:DESTDIR = ../../bin/release/
HEADERS += tal/kvi_tal_application_kde.h \
    tal/kvi_tal_application_qt.h \
    tal/kvi_tal_filedialog_qt4.h \
    tal/kvi_tal_grid_qt4.h \
    tal/kvi_tal_groupbox_qt4.h \
    tal/kvi_tal_hbox_qt4.h \
    tal/kvi_tal_iconview_qt4.h \
    tal/kvi_tal_listbox_qt4.h \
    tal/kvi_tal_listview_qt4.h \
    tal/kvi_tal_mainwindow_qt4.h \
    tal/kvi_tal_menubar_qt.h \
    tal/kvi_tal_popupmenu.h \
    tal/kvi_tal_scrollview_qt4.h \
    tal/kvi_tal_tabdialog_qt4.h \
    tal/kvi_tal_textedit_qt4.h \
    tal/kvi_tal_toolbar_qt4.h \
    tal/kvi_tal_vbox_qt4.h \
    tal/kvi_tal_widgetstack_qt4.h \
    tal/kvi_tal_wizard_qt4.h \
    tal/kvi_tal_application.h \
    tal/kvi_tal_filedialog.h \
    tal/kvi_tal_grid.h \
    tal/kvi_tal_groupbox.h \
    tal/kvi_tal_hbox.h \
    tal/kvi_tal_iconview.h \
    tal/kvi_tal_listbox.h \
    tal/kvi_tal_listview.h \
    tal/kvi_tal_mainwindow.h \
    tal/kvi_tal_menubar.h \
    tal/kvi_tal_popupmenu.h \
    tal/kvi_tal_scrollview.h \
    tal/kvi_tal_tabdialog.h \
    tal/kvi_tal_textedit.h \
    tal/kvi_tal_toolbar.h \
    tal/kvi_tal_toolbardocktype.h \
    tal/kvi_tal_tooltip.h \
    tal/kvi_tal_vbox.h \
    tal/kvi_tal_widgetstack.h \
    tal/kvi_tal_windowstate.h \
    tal/kvi_tal_wizard.h \
    system/kvi_byteorder.h \
    system/kvi_env.h \
    system/kvi_library.h \
    system/kvi_locale.h \
    system/kvi_process.h \
    system/kvi_stdarg.h \
    system/kvi_thread.h \
    system/kvi_time.h \
    net/kvi_dns.h \
    net/kvi_http.h \
    net/kvi_netutils.h \
    net/kvi_socket.h \
    net/kvi_sockettype.h \
    net/kvi_ssl.h \
    irc/kvi_avatar.h \
    irc/kvi_avatarcache.h \
    irc/kvi_ircmask.h \
    irc/kvi_ircserver.h \
    irc/kvi_ircserverdb.h \
    irc/kvi_ircuserdb.h \
    irc/kvi_mirccntrl.h \
    irc/kvi_nickserv.h \
    irc/kvi_useridentity.h \
    file/kvi_file.h \
    file/kvi_fileutils.h \
    file/kvi_packagefile.h \
    ext/kvi_accel.h \
    ext/kvi_cmdformatter.h \
    ext/kvi_config.h \
    ext/kvi_crypt.h \
    ext/kvi_databuffer.h \
    ext/kvi_dcophelper.h \
    ext/kvi_doublebuffer.h \
    ext/kvi_draganddrop.h \
    ext/kvi_garbage.h \
    ext/kvi_imagelib.h \
    ext/kvi_md5.h \
    ext/kvi_mediatype.h \
    ext/kvi_miscutils.h \
    ext/kvi_msgtype.h \
    ext/kvi_osinfo.h \
    ext/kvi_pixmap.h \
    ext/kvi_proxydb.h \
    ext/kvi_regchan.h \
    ext/kvi_regusersdb.h \
    ext/kvi_sharedfiles.h \
    ext/kvi_stringconversion.h \
    ext/kvi_xlib.h \
    core/kvi_bswap.h \
    core/kvi_error.h \
    core/kvi_heapobject.h \
    core/kvi_inttypes.h \
    core/kvi_malloc.h \
    core/kvi_memmove.h \
    core/kvi_qcstring.h \
    core/kvi_qstring.h \
    core/kvi_strasm.h \
    core/kvi_string.h \
    core/kvi_stringarray.h \
    config/kvi_confignames.h \
    config/kvi_debug.h \
    config/kvi_defaults.h \
    config/kvi_fileextensions.h \
    config/kvi_settings.h \
    config/kvi_sourcesdate.h \
    config/kvi_version.h \
    config/kvi_wincfg.h
SOURCES += tal/kvi_tal_application.cpp \
    tal/kvi_tal_filedialog.cpp \
    tal/kvi_tal_grid.cpp \
    tal/kvi_tal_groupbox.cpp \
    tal/kvi_tal_hbox.cpp \
    tal/kvi_tal_iconview.cpp \
    tal/kvi_tal_listbox.cpp \
    tal/kvi_tal_listview.cpp \
    tal/kvi_tal_mainwindow.cpp \
    tal/kvi_tal_menubar.cpp \
    tal/kvi_tal_popupmenu.cpp \
    tal/kvi_tal_scrollview.cpp \
    tal/kvi_tal_tabdialog.cpp \
    tal/kvi_tal_textedit.cpp \
    tal/kvi_tal_toolbar.cpp \
    tal/kvi_tal_tooltip.cpp \
    tal/kvi_tal_vbox.cpp \
    tal/kvi_tal_widgetstack.cpp \
    tal/kvi_tal_wizard.cpp \
    system/kvi_env.cpp \
    system/kvi_locale.cpp \
    system/kvi_thread.cpp \
    system/kvi_time.cpp \
    net/kvi_dns.cpp \
    net/kvi_http.cpp \
    net/kvi_netutils.cpp \
    net/kvi_socket.cpp \
    net/kvi_ssl.cpp \
    irc/kvi_avatar.cpp \
    irc/kvi_avatarcache.cpp \
    irc/kvi_ircmask.cpp \
    irc/kvi_ircserver.cpp \
    irc/kvi_ircserverdb.cpp \
    irc/kvi_ircuserdb.cpp \
    irc/kvi_mirccntrl.cpp \
    irc/kvi_nickserv.cpp \
    irc/kvi_useridentity.cpp \
    file/kvi_file.cpp \
    file/kvi_fileutils.cpp \
    file/kvi_packagefile.cpp \
    ext/kvi_cmdformatter.cpp \
    ext/kvi_config.cpp \
    ext/kvi_crypt.cpp \
    ext/kvi_databuffer.cpp \
    ext/kvi_dcophelper.cpp \
    ext/kvi_doublebuffer.cpp \
    ext/kvi_garbage.cpp \
    ext/kvi_imagelib.cpp \
    ext/kvi_md5.cpp \
    ext/kvi_mediatype.cpp \
    ext/kvi_miscutils.cpp \
    ext/kvi_msgtype.cpp \
    ext/kvi_osinfo.cpp \
    ext/kvi_pixmap.cpp \
    ext/kvi_proxydb.cpp \
    ext/kvi_regchan.cpp \
    ext/kvi_regusersdb.cpp \
    ext/kvi_sharedfiles.cpp \
    ext/kvi_stringconversion.cpp \
    core/kvi_error.cpp \
    core/kvi_heapobject.cpp \
    core/kvi_malloc.cpp \
    core/kvi_memmove.cpp \
    core/kvi_qstring.cpp \
    core/kvi_string.cpp \
    core/kvi_stringarray.cpp
FORMS += 
RESOURCES += 
DEFINES += __KVILIB__ \
    __QMAKE__
DEFINES -= UNICODE
win32 { 
    LIBS += -leay32 \
        -lssleay32 \
        -lws2_32
    RC_FILE = ../../data/resources/auto-generated/kvilib.rc
}
mac:LIBS += -lpthread \
    -lssl \
    -lcrypto \
    -lz
unix:LIBS += -lpthread \
    -lssl \
    -lcrypto \
    -lz
unix:DEFINES += COMPILE_THREADS_USE_POSIX \
    COMPILE_USE_QT4
include( using_kvilib.pri )
target.path = ../../bin/image/
INSTALLS += target
CONFIG(debug, debug|release) { 
    MOC_DIR = ../../build/$$TARGET/debug/moc
    OBJECTS_DIR = ../../build/$$TARGET/debug/obj
}
else { 
    MOC_DIR = ../../build/$$TARGET/release/moc
    OBJECTS_DIR = ../../build/$$TARGET/release/obj
}

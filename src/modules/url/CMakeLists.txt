# CMakeLists for src/modules/url

SET(kvieventeditor_MOC_HDRS
	libkviurl.h
	icons.h
)

SET(kviurl_SRCS
	libkviurl.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviurl_MOC_SRCS ${kviurl_MOC_HDRS})

# Create library
ADD_LIBRARY(kviurl MODULE ${kviurl_SRCS} ${kviurl_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kviurl)

INSTALL(TARGETS kviurl LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)
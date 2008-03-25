# CMakeLists for src/modules/config

SET(kviconfig_SRCS
	libkviconfig.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviconfig_MOC_SRCS ${kviconfig_MOC_HDRS})

# Create library
ADD_LIBRARY(kviconfig MODULE ${kviconfig_SRCS} ${kviconfig_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kviconfig)

INSTALL(TARGETS kviconfig LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)
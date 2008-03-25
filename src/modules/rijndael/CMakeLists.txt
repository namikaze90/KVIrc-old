# CMakeLists for src/modules/rijndael

SET(kvieditor_MOC_HDRS
	libkvirijndael.h
	rijndael.h
	ablowfish.h
)

SET(kvirijndael_SRCS
	libkvirijndael.cpp
	rijndael.cpp
	ablowfish.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvirijndael_MOC_SRCS ${kvirijndael_MOC_HDRS})

# Create library
ADD_LIBRARY(kvirijndael MODULE ${kvirijndael_SRCS} ${kvirijndael_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kvirijndael)

INSTALL(TARGETS kvirijndael LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)
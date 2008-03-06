# CMakeLists for src/modules/avatar

SET(kviavatar_MOC_HDRS
	libkviavatar.h
)

SET(kviavatar_SRCS
	libkviavatar.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviavatar_MOC_SRCS ${kviavatar_MOC_HDRS})

# Create library
ADD_LIBRARY(kviavatar ${kviavatar_SRCS} ${kviavatar_MOC_SRCS})
#TARGET_LINK_LIBRARIES(libkvilib kviavatar)
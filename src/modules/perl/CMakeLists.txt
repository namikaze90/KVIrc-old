# CMakeLists for src/modules/perl

SET(kviperl_SRCS
	libkviperl.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviperl_MOC_SRCS ${kviperl_MOC_HDRS})

# Create library
ADD_LIBRARY(kviperl ${kviperl_SRCS} ${kviperl_MOC_SRCS})
#TARGET_LINK_LIBRARIES(libkvilib kviaddon)

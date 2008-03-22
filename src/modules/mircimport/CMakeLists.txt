# CMakeLists for src/modules/mircimport

SET(kvimircimport_MOC_HDRS
	libkvimircimport.h
)

SET(kvimircimport_SRCS
	libkvimircimport.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvimircimport_MOC_SRCS ${kvimircimport_MOC_HDRS})

# Create library
ADD_LIBRARY(kvimircimport ${kvimircimport_SRCS} ${kvimircimport_MOC_SRCS})
#TARGET_LINK_LIBRARIES(libkvilib kviaddon)

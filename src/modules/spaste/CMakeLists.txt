# CMakeLists for src/modules/spaste

SET(kvieventeditor_MOC_HDRS
	controller.h
	libkvispaste.h
)

SET(kvispaste_SRCS
	libkvispaste.cpp
	controller.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvispaste_MOC_SRCS ${kvispaste_MOC_HDRS})

# Create library
ADD_LIBRARY(kvispaste ${kvispaste_SRCS} ${kvispaste_MOC_SRCS})
#TARGET_LINK_LIBRARIES(libkvilib kvispaste)

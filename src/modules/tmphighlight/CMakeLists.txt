# CMakeLists for src/modules/tmphighlight

SET(kvitmphighlight_SRCS
	libkvitmphighlight.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvitmphighlight_MOC_SRCS ${kvitmphighlight_MOC_HDRS})

# Create library
ADD_LIBRARY(kvitmphighlight ${kvitmphighlight_SRCS} ${kvitmphighlight_MOC_SRCS})
#TARGET_LINK_LIBRARIES(libkvilib kvitmphighlight)

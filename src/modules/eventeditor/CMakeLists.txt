# CMakeLists for src/modules/eventeditor

SET(kvieventeditor_MOC_HDRS
	eventeventeditor.h
)

SET(kvieventeditor_SRCS
	eventeventeditor.cpp
	eventeventeditor.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvieventeditor_MOC_SRCS ${kvieventeditor_MOC_HDRS})

# Create library
ADD_LIBRARY(kvieventeditor ${kvieventeditor_SRCS} ${kvieventeditor_MOC_SRCS})
#TARGET_LINK_LIBRARIES(libkvilib kvieventeditor)
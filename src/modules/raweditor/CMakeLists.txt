# CMakeLists for src/modules/raweditor

SET(kviraweditor_MOC_HDRS
	raweditor.h
)

SET(kviraweditor_SRCS
	libkviraweditor.cpp
	raweditor.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviraweditor_MOC_SRCS ${kviraweditor_MOC_HDRS})

# Create library
ADD_LIBRARY(kviraweditor ${kviraweditor_SRCS} ${kviraweditor_MOC_SRCS})
#TARGET_LINK_LIBRARIES(libkvilib kviraweditor)

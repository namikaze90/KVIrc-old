# CMakeLists for src/modules/filetransferwindow

SET(kvifiletransferwindow_MOC_HDRS
	filetransferwindow.h
)

SET(kvifiletransferwindow_SRCS
	libkvifiletransferwindow.cpp
	filetransferwindow.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvifiletransferwindow_MOC_SRCS ${kvifiletransferwindow_MOC_HDRS})

# Create library
ADD_LIBRARY(kvifiletransferwindow ${kvifiletransferwindow_SRCS} ${kvifiletransferwindow_MOC_SRCS})
#TARGET_LINK_LIBRARIES(libkvilib kvifiletransferwindow)
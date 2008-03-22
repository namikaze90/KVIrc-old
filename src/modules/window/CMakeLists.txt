# CMakeLists for src/modules/window

SET(kvieventeditor_MOC_HDRS
	userwindow.h
)

SET(kviwindow_SRCS
	libkviwindow.cpp
	userwindow.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviwindow_MOC_SRCS ${kviwindow_MOC_HDRS})

# Create library
ADD_LIBRARY(kviwindow ${kviwindow_SRCS} ${kviwindow_MOC_SRCS})
#TARGET_LINK_LIBRARIES(libkvilib kviwindow)

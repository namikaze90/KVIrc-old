# CMakeLists for src/modules/socketspy

SET(kvieditor_MOC_HDRS
	socketspywindow.h
)

SET(kvisocketspy_SRCS
	libkvisocketspy.cpp
	socketspywindow.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvisocketspy_MOC_SRCS ${kvisocketspy_MOC_HDRS})

# Create library
ADD_LIBRARY(kvisocketspy ${kvisocketspy_SRCS} ${kvisocketspy_MOC_SRCS})
#TARGET_LINK_LIBRARIES(libkvilib kvisocketspy)

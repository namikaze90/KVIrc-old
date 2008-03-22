# CMakeLists for src/modules/snd

# From qmake
#win32{
#	LIBS += -lwinmm
#}

SET(kvieditor_MOC_HDRS
	libkvisnd.h
)

SET(kvisnd_SRCS
	libkvisnd.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvisnd_MOC_SRCS ${kvisnd_MOC_HDRS})

# Create library
ADD_LIBRARY(kvisnd ${kvisnd_SRCS} ${kvisnd_MOC_SRCS})
#TARGET_LINK_LIBRARIES(libkvilib kvisnd)

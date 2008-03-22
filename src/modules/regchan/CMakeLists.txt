# CMakeLists for src/modules/regchan

SET(kviregchan_SRCS
	libkviregchan.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviregchan_MOC_SRCS ${kviregchan_MOC_HDRS})

# Create library
ADD_LIBRARY(kviregchan ${kviregchan_SRCS} ${kviregchan_MOC_SRCS})
#TARGET_LINK_LIBRARIES(libkvilib kviregchan)

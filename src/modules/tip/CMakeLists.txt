# CMakeLists for src/modules/tip

SET(kvieventeditor_MOC_HDRS
	libkvitip.h
)

SET(kvitip_SRCS
	libkvitip.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvitip_MOC_SRCS ${kvitip_MOC_HDRS})

# Create library
ADD_LIBRARY(kvitip ${kvitip_SRCS} ${kvitip_MOC_SRCS})
#TARGET_LINK_LIBRARIES(libkvilib kvitip)

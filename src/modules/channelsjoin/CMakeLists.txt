# CMakeLists for src/modules/channelsjoin

SET(kvichannelsjoin_MOC_HDRS
	channelsjoinwindow.h
)

SET(kvichannelsjoin_SRCS
	libkvichannelsjoin.cpp
	channelsjoinwindow.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvichannelsjoin_MOC_SRCS ${kvichannelsjoin_MOC_HDRS})

# Create library
ADD_LIBRARY(kvichannelsjoin ${kvichannelsjoin_SRCS} ${kvichannelsjoin_MOC_SRCS})
#TARGET_LINK_LIBRARIES(libkvilib kvichannelsjoin)
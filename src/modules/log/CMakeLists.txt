# CMakeLists for src/modules/log

SET(kvilog_SRCS
	libkvilog.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvilog_MOC_SRCS ${kvilog_MOC_HDRS})

# Create library
ADD_LIBRARY(kvilog ${kvilog_SRCS} ${kvilog_MOC_SRCS})
#TARGET_LINK_LIBRARIES(libkvilib kvilog)
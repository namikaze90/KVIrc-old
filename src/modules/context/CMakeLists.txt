# CMakeLists for src/modules/context

SET(kvicontext_SRCS
	libkvicontext.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvicontext_MOC_SRCS ${kvicontext_MOC_HDRS})

# Create library
ADD_LIBRARY(kvicontext ${kvicontext_SRCS} ${kvicontext_MOC_SRCS})
#TARGET_LINK_LIBRARIES(libkvilib kvicontext)
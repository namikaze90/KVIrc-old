# CMakeLists for src/modules/sharedfile

SET(kvisharedfile_SRCS
	libkvisharedfile.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvisharedfile_MOC_SRCS ${kvisharedfile_MOC_HDRS})

# Create library
ADD_LIBRARY(kvisharedfile ${kvisharedfile_SRCS} ${kvisharedfile_MOC_SRCS})
#TARGET_LINK_LIBRARIES(libkvilib kvisharedfile)

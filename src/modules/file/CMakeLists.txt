# CMakeLists for src/modules/file

SET(kvifile_SRCS
	libkvifile.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvifile_MOC_SRCS ${kvifile_MOC_HDRS})

# Create library
ADD_LIBRARY(kvifile ${kvifile_SRCS} ${kvifile_MOC_SRCS})
#TARGET_LINK_LIBRARIES(libkvilib kvifile)
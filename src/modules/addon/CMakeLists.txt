# CMakeLists for src/modules/addon

SET(kviaddon_MOC_HDRS
	managementdialog.h
)

SET(kviaddon_SRCS
	managementdialog.cpp
	libkviaddon.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviaddon_MOC_SRCS ${kviaddon_MOC_HDRS})

# Create library
ADD_LIBRARY(kviaddon ${kviaddon_SRCS} ${kviaddon_MOC_SRCS})
#TARGET_LINK_LIBRARIES(libkvilib kviaddon)
# CMakeLists for src/modules/theme

SET(kvieventeditor_MOC_HDRS
	managementdialog.h
	packthemedialog.h
	savethemedialog.h
	themefunctions.h
)

SET(kvitheme_SRCS
	libkvitheme.cpp
	managementdialog.cpp
	packthemedialog.cpp
	savethemedialog.cpp
	themefunctions.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvitheme_MOC_SRCS ${kvitheme_MOC_HDRS})

# Create library
ADD_LIBRARY(kvitheme ${kvitheme_SRCS} ${kvitheme_MOC_SRCS})
#TARGET_LINK_LIBRARIES(libkvilib kvitheme)

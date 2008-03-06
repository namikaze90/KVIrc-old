# CMakeLists for src/modules/codetester

SET(kvicodetester_MOC_HDRS
	codetester.h
)

SET(kvicodetester_SRCS
	libkvicodetester.cpp
	codetester.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvicodetester_MOC_SRCS ${kvicodetester_MOC_HDRS})

# Create library
ADD_LIBRARY(kvicodetester ${kvicodetester_SRCS} ${kvicodetester_MOC_SRCS})
#TARGET_LINK_LIBRARIES(libkvilib kvicodetester)
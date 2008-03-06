# CMakeLists for src/modules/action

SET(kviabout_SRCS
	libkviaction.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviaction_MOC_SRCS ${kviaction_MOC_HDRS})

# Create library
ADD_LIBRARY(kviaction ${kviaction_SRCS} ${kviaction_MOC_SRCS})
#TARGET_LINK_LIBRARIES(libkvilib kviaction)
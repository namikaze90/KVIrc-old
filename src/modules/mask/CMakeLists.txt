# CMakeLists for src/modules/mask

SET(kvimask_SRCS
	libkvimask.cpp
)

# Create library
ADD_LIBRARY(kvimask ${kvimask_SRCS} ${kvimask_MOC_SRCS})
#TARGET_LINK_LIBRARIES(libkvilib kvimask)

# CMakeLists for src/modules/mask

SET(kvimask_SRCS
	libkvimask.cpp
)

# Create library
ADD_LIBRARY(kvimask MODULE ${kvimask_SRCS} ${kvimask_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kvimask)

INSTALL(TARGETS kvimask LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)
# CMakeLists for data/deftheme/silverirc

FILE(GLOB files "*.kvc")
FILE(GLOB files2 "*.png")
INSTALL(FILES ${files} DESTINATION ${KVI_INSTALL_PREFIX}/share/${KVI_PACKAGE}/${KVI_VERSION_BRANCH}/deftheme/)
INSTALL(FILES ${files2} DESTINATION ${KVI_INSTALL_PREFIX}/share/${KVI_PACKAGE}/${KVI_VERSION_BRANCH}/deftheme/)
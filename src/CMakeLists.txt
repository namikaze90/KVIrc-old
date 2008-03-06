# CMakeLists for src/

# OS check
IF(UNIX)
	IF(APPLE)
	# MacOS X
		SET(KVI_INSTALL_PREFIX .)
	ELSE(APPLE)
	# Linux
		SET(KVI_INSTALL_PREFIX /usr/local)
	ENDIF(APPLE)
ELSE(UNIX)
	IF(WIN32)
		ADD_SUBDIRECTORY(win32registrar)
		SET(KVI_INSTALL_PREFIX .)
	ENDIF(WIN32)
ENDIF(UNIX)

# Find subdirs
SUBDIRS(kvilib kvirc modules)
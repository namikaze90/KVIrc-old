# CMakeLists for src/

# OS check
IF(UNIX)
	IF(APPLE)
	# MacOS X
		SET(KVI_INSTALL_PREFIX .)
	ELSE()
	# Linux
		SET(KVI_INSTALL_PREFIX /usr/local)
	ENDIF()
ELSEIF(WIN32)
	ADD_SUBDIRECTORY(win32registrar)
	SET(KVI_INSTALL_PREFIX .)
ENDIF()

# Find subdirs
SUBDIRS(kvilib kvirc modules)
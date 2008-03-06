# Find subdirs
SUBDIRS(kvilib kvirc modules)

IF(UNIX)
	IF(APPLE)
	# MacOS X
	ELSE(APPLE)
	# Linux
	ENDIF(APPLE)
ELSE(UNIX)
	IF(WIN32)
		ADD_SUBDIRECTORY(win32registrar)
	ENDIF(WIN32)
ENDIF(UNIX)
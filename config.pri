###############################################################################
# Main build configuration for the KVIrc IRC client
# The settings contained here affect the whole build process.
###############################################################################

# Uncomment the following line if you want a debug version
CONFIG += debug

CONFIG += qdbus

# Do not touch :)
KVI_PACKAGE = "kvirc"
KVI_VERSION = "3.9.9"
KVI_VERSION_BRANCH = "3.9"

# Platform specific sections
unix {
	# Unix stuff installs in /usr/local by default
	KVI_INSTALL_PREFIX = /usr/local
}

win32 {
	# Windows stuff installs straight in the source directory
	KVI_INSTALL_PREFIX = .
	# prevent stupid opensource Q_ASSERT usage
	DEFINES += QT_NO_DEBUG
}

mac {
	# Mac stuff installs straight in the source directory
	KVI_INSTALL_PREFIX = .
}


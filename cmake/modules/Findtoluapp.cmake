#
# Locate TOLUAPP include paths and libraries
# TOLUAPP can be found at http://TOLUAPP.sourceforge.net/
# Written by Manfred Ulz, manfred.ulz_at_tugraz.at

# This module defines
# TOLUAPP_INCLUDE_DIR, where to find tolua++.h, etc.
# TOLUAPP_LIBRARIES, the libraries to link against to use libtolua++5.1.
# TOLUAPP_FOUND, If false, don't try to use tolua++.

FIND_PROGRAM(TOLUAPP_EXEC NAMES tolua++ toluapp tolua++5.1
    PATHS
    "$ENV{TOLUAPP}/include"
    /usr/bin
    /usr/local/bin
    C:/Workspace/Build/tolua++/bin
)

FIND_PATH(TOLUAPP_INCLUDE_DIR tolua++.h
    PATHS
    "$ENV{TOLUAPP}/include"
    /usr/local/include
    /usr/include
    C:/Workspace/Build/tolua++/include
)

FIND_LIBRARY(TOLUAPP_LIBRARIES toluapp tolua++5.1
    PATHS
    "$ENV{TOLUAPP}/lib"
    /usr/local/lib
    /usr/lib
    C:/Workspace/Build/tolua++/lib
    C:/Workspace/Build/tolua++/lib/Debug
    C:/Workspace/Build/tolua++/lib/Release
)

SET(TOLUAPP_FOUND 0)
IF(TOLUAPP_INCLUDE_DIR)
  IF(TOLUAPP_LIBRARIES)
    SET(TOLUAPP_FOUND 1)
    MESSAGE(STATUS "Found TOLUAPP")
  ENDIF(TOLUAPP_LIBRARIES)
ENDIF(TOLUAPP_INCLUDE_DIR)

MARK_AS_ADVANCED(
  TOLUAPP_INCLUDE_DIR
  TOLUAPP_LIBRARIES
) 

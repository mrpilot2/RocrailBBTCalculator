# aIDE's own top-level settings (which export all symbols for Windows DLLs)
# are not applied when only the library subdirectory is consumed.
if(WIN32)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

if(NOT TARGET Aide::AideApplication)
  find_package(AideQt${QT_VERSION_MAJOR} CONFIG QUIET)
endif()

# To use a local aIDE checkout, configure with
#   -DFETCHCONTENT_SOURCE_DIR_AIDEQT6=/path/to/aide
if(NOT TARGET Aide::AideApplication)
  set(aide_GIT_TAG "develop")

  include(FetchContent)
  FetchContent_Declare(
    AideQt${QT_VERSION_MAJOR}
    GIT_REPOSITORY https://github.com/mrpilot2/aide.git
    GIT_TAG ${aide_GIT_TAG}
    SOURCE_SUBDIR aide # Build the library only
    FIND_PACKAGE_ARGS NAMES AideQt${QT_VERSION_MAJOR}
  )

  FetchContent_MakeAvailable(AideQt${QT_VERSION_MAJOR})

  if(${aide_GIT_TAG} MATCHES "^v[0-9]+.[0-9]+.[0-9]+")
    string(SUBSTRING ${aide_GIT_TAG} 1 20 aide_PACKAGE_VERSION)
  else()
    set(aide_PACKAGE_VERSION ${aide_GIT_TAG})
  endif()

  file(
    WRITE
    ${CMAKE_FIND_PACKAGE_REDIRECTS_DIR}/aideqt${QT_VERSION_MAJOR}-config-version.cmake
    "
set(PACKAGE_VERSION ${aide_PACKAGE_VERSION})
set(PACKAGE_VERSION_COMPATIBLE TRUE)
set(PACKAGE_VERSION_EXACT TRUE)
"
  )

  find_package(AideQt${QT_VERSION_MAJOR} CONFIG REQUIRED)
else()
  # assuming aide is previously added as subdirectory
  set(aide_GIT_TAG "develop")
  set(AideQt${QT_VERSION_MAJOR}_VERSION "develop")
endif()


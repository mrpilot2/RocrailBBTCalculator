CMAKE_MINIMUM_REQUIRED(VERSION 3.26)
PROJECT(rocrailBBTCalculator
  LANGUAGES CXX
  VERSION 0.3.1 # x-release-please-version
  )

FIND_PACKAGE(Qt6 COMPONENTS Widgets Xml REQUIRED)
SET(QT_VERSION_MAJOR 6)

# Shared builds on Windows: put all DLLs next to the executables so they
# can be found when running the app and the tests from the build tree.
IF(WIN32)
  SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
ENDIF()

INCLUDE(dependencies/aide.cmake)

ADD_SUBDIRECTORY(src)

ENABLE_TESTING()
ADD_SUBDIRECTORY(tests)
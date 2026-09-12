CMAKE_MINIMUM_REQUIRED(VERSION 3.12)

INCLUDE(GNUInstallDirs)

FIND_PACKAGE(Qt6 COMPONENTS Widgets Xml REQUIRED)

ADD_SUBDIRECTORY(core)
ADD_SUBDIRECTORY(gui)

QT_ADD_EXECUTABLE(rocrailBBTCalculator MACOSX_BUNDLE WIN32
  main.cpp
)

TARGET_LINK_LIBRARIES(rocrailBBTCalculator
  PRIVATE
    Qt6::Widgets

    bbtCalculatorCore
)

if(APPLE)
  # Assume targeting macOS to keep this example simple
  set_target_properties(rocrailBBTCalculator PROPERTIES
  INSTALL_RPATH @executable_path/../Frameworks
)
endif()

install(TARGETS rocrailBBTCalculator
  BUNDLE DESTINATION .
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Additional Qt-specific logic to be executed after the
# above target install is carried out
qt_generate_deploy_app_script(
  TARGET rocrailBBTCalculator
  OUTPUT_SCRIPT deployApp
)

install(SCRIPT "${deployApp}")

SET(CPACK_PACKAGE_NAME "rocrailBBTCalculator")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "rocrailBBTCalculator")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Calculates BBT values for RocRail Locomotives")
SET(CPACK_PACKAGE_VENDOR "Markus Reitboeck")
SET(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE)

SET(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
SET(CPACK_NSIS_DISPLAY_NAME "Rocrail BBT Calculator")
SET(CPACK_NSIS_PACKAGE_NAME "Rocrail BBT Calculator")
SET(CPACK_NSIS_MODIFY_PATH ON)

SET(CPACK_GENERATOR NSIS)

INCLUDE(CPack)

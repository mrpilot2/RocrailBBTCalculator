FIND_PACKAGE(Catch2 REQUIRED)
INCLUDE(Catch)

QT_ADD_EXECUTABLE(ut.rocrailBBTCalculator
  coretest.cpp
)

TARGET_LINK_LIBRARIES(ut.rocrailBBTCalculator
  PRIVATE
    Catch2::Catch2WithMain
    Qt6::Widgets

    Aide::AideApplication

    bbtCalculatorCore
)

# Core builds a (widget based) main window, so run headless
CATCH_DISCOVER_TESTS(ut.rocrailBBTCalculator
  PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

TARGET_COMPILE_DEFINITIONS(ut.rocrailBBTCalculator
  PRIVATE PLAN_XML="${CMAKE_CURRENT_SOURCE_DIR}/plan.xml"
)

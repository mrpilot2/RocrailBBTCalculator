SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/forms)

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)

FIND_PACKAGE(Qt6LinguistTools REQUIRED)

QT6_ADD_TRANSLATION(QM
  translations/rocrailBBT_de.ts
  translations/rocrailBBT_en.ts
)

ADD_LIBRARY(bbtCalculatorGui STATIC)

CONFIGURE_FILE(application.qrc.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/application.qrc)

TARGET_SOURCES(bbtCalculatorGui
  PRIVATE
    blockmodel.cpp
    bbtmodel.cpp
    locmodel.cpp
    routemodel.cpp

    imagesortfilterproxymodel.cpp

    mainwindow.cpp
    mainwindowcontroller.cpp

    ${CMAKE_SOURCE_DIR}/src/include/gui/mainwindow.hpp
    ${CMAKE_SOURCE_DIR}/src/include/gui/mainwindowcontroller.hpp

    ${CMAKE_CURRENT_BINARY_DIR}/application.qrc

    ${QM}
)

TARGET_INCLUDE_DIRECTORIES(bbtCalculatorGui
  PRIVATE
    ${CMAKE_SOURCE_DIR}/src/include/gui/
  PUBLIC
    ${CMAKE_SOURCE_DIR}/src/include/
)

TARGET_LINK_LIBRARIES(bbtCalculatorGui
  PUBLIC
    Qt6::Widgets
)

TARGET_COMPILE_FEATURES(bbtCalculatorGui
  PUBLIC
    cxx_std_17
)
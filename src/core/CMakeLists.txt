
ADD_LIBRARY(bbtCalculatorCore STATIC)

TARGET_SOURCES(bbtCalculatorCore
  PRIVATE
    core.cpp

    calculation.cpp
    planparser.cpp
    workspace.cpp
)

TARGET_INCLUDE_DIRECTORIES(bbtCalculatorCore
  PRIVATE
    ${CMAKE_SOURCE_DIR}/src/include/core
  PUBLIC
    ${CMAKE_SOURCE_DIR}/src/include/
)

TARGET_LINK_LIBRARIES(bbtCalculatorCore
  PRIVATE
    Qt6::Xml

    bbtCalculatorGui
)

TARGET_COMPILE_FEATURES(bbtCalculatorCore
  PUBLIC
    cxx_std_17
)
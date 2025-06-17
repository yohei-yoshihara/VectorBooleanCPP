cmake_minimum_required(VERSION 3.28)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
project(vectorboolean)

add_library(vectorboolean
  src/vectorboolean/VectorBoolean.hpp
  src/vectorboolean/FBCommon.hpp
  src/vectorboolean/FBCommon.cpp
  src/vectorboolean/FBBezierPath.hpp
  src/vectorboolean/FBBezierPath.cpp
  src/vectorboolean/FBBezierContour.cpp
  src/vectorboolean/FBBezierContour.hpp
  src/vectorboolean/FBBezierCurve.cpp
  src/vectorboolean/FBBezierCurve.hpp
  src/vectorboolean/FBBezierGraph.cpp
  src/vectorboolean/FBBezierGraph.hpp
  src/vectorboolean/FBBezierIntersection.cpp
  src/vectorboolean/FBBezierIntersection.hpp
  src/vectorboolean/FBBezierIntersectRange.cpp
  src/vectorboolean/FBBezierIntersectRange.hpp
  src/vectorboolean/FBContourOverlap.cpp
  src/vectorboolean/FBContourOverlap.hpp
  src/vectorboolean/FBCurveLocation.cpp
  src/vectorboolean/FBCurveLocation.hpp
  src/vectorboolean/FBEdgeCrossing.cpp
  src/vectorboolean/FBEdgeCrossing.hpp
  src/vectorboolean/FBGeometry.cpp
  src/vectorboolean/FBGeometry.hpp
)
target_compile_features(vectorboolean PRIVATE cxx_std_23)
if (MSVC)
    target_compile_options(vectorboolean PRIVATE /W4)
else()
    target_compile_options(vectorboolean PRIVATE -Wall -Wno-missing-braces)
endif()
target_include_directories(vectorboolean PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# ***** example *****
add_executable(example
  examples/main.cpp
)
target_compile_features(example PRIVATE cxx_std_23)
target_link_libraries(example PRIVATE vectorboolean)

# ***** test *****
enable_testing()
add_subdirectory(tests)
add_test(NAME test_main COMMAND $<TARGET_FILE:test_main>)

cmake_minimum_required(VERSION 3.28)
project(tests_project VERSION 1.0 LANGUAGES CXX)

add_executable(test_main 
  test_main.cpp
  test_two_boxes.cpp
  test_complexshapes.cpp
  test_circle_overlapping_rectangle.cpp
  test_touched_rectangles.cpp
  test_arc_shapes.cpp

  utils.hpp utils.cpp
)
target_compile_features(test_main PUBLIC cxx_std_23)
target_link_libraries(test_main PRIVATE vectorboolean)

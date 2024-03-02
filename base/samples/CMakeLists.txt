add_executable(
  d3d11_sample WIN32
  "d3d11_sample.cpp"
)
target_include_directories(d3d11_sample PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
target_link_libraries(d3d11_sample PRIVATE base_win base_win_dx)

add_executable(
  d3d12_sample WIN32
  "d3d12_sample.cpp"
)
target_include_directories(d3d12_sample PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
target_link_libraries(d3d12_sample PRIVATE base_win base_win_dx)

add_executable(
  d3d11_sample WIN32
  "d3d11_sample.cpp"
)
target_include_directories(d3d11_sample PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
target_link_libraries(d3d11_sample PRIVATE base_win base_dx)

add_executable(
  d3d12_sample WIN32
  "d3d12_sample.cpp"
)
target_include_directories(d3d12_sample PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
target_link_libraries(d3d12_sample PRIVATE base_win base_dx)
message("================== ${CMAKE_CURRENT_BINARY_DIR}")
add_custom_command(TARGET d3d12_sample
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different 
    "${CMAKE_CURRENT_SOURCE_DIR}/../dx/shader" 
    "${CMAKE_CURRENT_BINARY_DIR}/$<$<CONFIG:Release>:Release>$<$<CONFIG:Debug>:Debug>")

add_executable(
  d2d1_sample WIN32
  "d2d1_sample.cpp"
)
target_include_directories(d2d1_sample PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
target_link_libraries(d2d1_sample PRIVATE base_win base_dx)

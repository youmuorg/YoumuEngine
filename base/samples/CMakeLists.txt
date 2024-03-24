# D2D1
add_executable(
  d2d1_sample WIN32
  "d2d1_sample.cpp"
)
target_include_directories(d2d1_sample PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
target_link_libraries(d2d1_sample PRIVATE base_win base_dx)

# D3D11
add_executable(
  d3d11_sample WIN32
  "d3d11_sample.cpp"
)
target_include_directories(d3d11_sample PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
target_link_libraries(d3d11_sample PRIVATE base_win base_dx)

# D3D12
add_executable(
  d3d12_triangle_sample WIN32
  "d3d12_triangle_sample.cpp"
)
target_include_directories(d3d12_triangle_sample PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
target_link_libraries(d3d12_triangle_sample PRIVATE base_win base_dx)
add_custom_command(TARGET d3d12_triangle_sample
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different 
    "${CMAKE_CURRENT_SOURCE_DIR}/../dx/shader" 
    "${CMAKE_CURRENT_BINARY_DIR}/$<$<CONFIG:Release>:Release>$<$<CONFIG:Debug>:Debug>")

add_executable(
  d3d12_cube_sample WIN32
  "d3d12_cube_sample.cpp"
)
target_include_directories(d3d12_cube_sample PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
target_link_libraries(d3d12_cube_sample PRIVATE base_win base_dx)
add_custom_command(TARGET d3d12_cube_sample
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different 
    "${CMAKE_CURRENT_SOURCE_DIR}/../dx/shader" 
    "${CMAKE_CURRENT_BINARY_DIR}/$<$<CONFIG:Release>:Release>$<$<CONFIG:Debug>:Debug>")

add_executable(
  d3d12_city_sample WIN32
  "d3d12_city_sample.cpp"
)
target_include_directories(d3d12_city_sample PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
target_link_libraries(d3d12_city_sample PRIVATE base_win base_dx)
add_custom_command(TARGET d3d12_city_sample
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different 
    "${CMAKE_CURRENT_SOURCE_DIR}/../dx/shader" 
    "${CMAKE_CURRENT_BINARY_DIR}/$<$<CONFIG:Release>:Release>$<$<CONFIG:Debug>:Debug>")
add_custom_command(TARGET d3d12_city_sample
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different 
    "${CMAKE_CURRENT_SOURCE_DIR}/../dx/model" 
    "${CMAKE_CURRENT_BINARY_DIR}/$<$<CONFIG:Release>:Release>$<$<CONFIG:Debug>:Debug>")
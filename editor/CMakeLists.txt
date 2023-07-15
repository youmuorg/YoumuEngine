set(EDITOR_NAME "editor")
add_executable(
  ${EDITOR_NAME} WIN32
  "main_win.cpp"
)
find_package(unifex CONFIG REQUIRED)
target_include_directories(${EDITOR_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../")
target_link_libraries(${EDITOR_NAME} PRIVATE base_win base_win_dx unifex::unifex)
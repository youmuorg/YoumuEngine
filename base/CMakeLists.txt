set(BASE_WIN_LIB base_win)
file(GLOB BASE_WIN_SOURCES "win/*.cpp")
file(GLOB BASE_WIN_HEADERS "win/*.h")

add_library(${BASE_WIN_LIB} STATIC 
  ${BASE_WIN_SOURCES}
  ${BASE_WIN_HEADERS}
)
target_include_directories(${BASE_WIN_LIB} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../)

file(GLOB BASE_DX_SOURCES "dx/*.cpp")
file(GLOB BASE_DX_HEADERS "dx/*.h")
set(BASE_DX_LIB base_dx)


add_library(${BASE_DX_LIB} STATIC
  ${BASE_DX_SOURCES}
  ${BASE_DX_HEADERS}
)
find_package(directxtex CONFIG REQUIRED)
target_link_libraries(${BASE_DX_LIB} PRIVATE Microsoft::DirectXTex)

find_package(directx-headers CONFIG REQUIRED)
target_link_libraries(${BASE_DX_LIB} PUBLIC Microsoft::DirectX-Guids Microsoft::DirectX-Headers)

target_link_libraries(${BASE_DX_LIB} PUBLIC base_win)

add_subdirectory("samples")
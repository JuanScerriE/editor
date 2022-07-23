# tests CMakeLists.txt

add_executable(Test1 Test1.cpp)

target_include_directories(Test1
  PUBLIC
    "${PROJECT_BINARY_DIR}/include"
)

target_link_libraries(Test1
  PUBLIC
    "${PROJECT_BINARY_DIR}/src/libfileview.a"
)

# src CMakeLists.txt

add_executable(editor Editor.cpp FileView.cpp)

target_include_directories(editor PUBLIC include)
target_include_directories(editor PRIVATE src)

find_package(OpenGL REQUIRED)
set(OpenGL_GL_PREFERENCE "GLVND")

target_link_libraries(editor PRIVATE glfw)
target_link_libraries(editor PRIVATE glad)
target_link_libraries(editor PRIVATE OpenGL::OpenGL)

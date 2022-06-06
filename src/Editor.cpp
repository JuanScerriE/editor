/*
 * - Open files.
 * - Cursor view for the file.
 * - A render cursor view.
 * - An event loop.
 * - A input handler.
 * - A buffer structure to add all the things and output all
 * at once.
 * - A way to translate between the actual view and the
 * render view. (Rendering is important to output
 * non-printable characters.)
 * - Input should be effecting the actual view then we
 * convert to the render view.
 * - Some backend to create the the term-ui easily.
 */

#include <Editor.hpp>

int main() {
  GLFWwindow* window;

  /* Initialize the library */
  if (!glfwInit())
    return -1;

  /* Create a windowed mode window and its OpenGL context
   */
  window =
      glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT);

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

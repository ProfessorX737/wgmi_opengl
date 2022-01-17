#ifndef CHICKEN3421_WINDOW_UTILS_HPP
#define CHICKEN3421_WINDOW_UTILS_HPP

#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace chicken3421 {

    /**
     *
     * Creates a new window with the given width, height, and title.
     *
     * @param width: Screen width in pixels
     * @param height: Screen height in pixels
     * @param title: The title for the window
     * @return A valid GLFWwindow pointer or null if window creation failed
     */
    GLFWwindow *make_window(int width, int height, const std::string &title);

    /**
     *
     * Given a valid GLFWwindow pointer, makes this window's OpenGL context current and loads the Glad function pointers into it
     *
     * @param win: A valid GLFWwindow pointer
     * @return True if there was no error, false if there was.
     */
    bool make_opengl_context(GLFWwindow *win);

    /**
     * Convenience function that combines make_window() and make_opengl_context().
     * Throws an exception on error.
     *
     * @see make_window
     * @see make_opengl_context
     *
     * @param width: initial width of the window
     * @param height: initial height of the window
     * @param title: initial title of the window
     * @return: a valid window ready for rendering into
     */
    GLFWwindow *make_opengl_window(int width, int height, const std::string &title);

    /**
     * Destroys a previously created window and OpenGL context
     *
     * @param w The window to destroy
     */
    void delete_opengl_window(GLFWwindow *w);

    /**
     * Enables OpenGL debug reporting. Must be called before make_window.
     * WARNING: This feature is experimental. It will load OpenGL 4.3 instead
     * of the course standard 3.3. Your driver may not support 4.3, or your
     * program may behave differently. You should not use it in your final
     * submissions, but it may help you in debugging.
     */
    void enable_debug_output();

}


#endif //CHICKEN3421_WINDOW_UTILS_HPP

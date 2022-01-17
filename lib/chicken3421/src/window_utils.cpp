#include <optional>
#include <iostream>

#include <chicken3421/error_utils.hpp>

#include <chicken3421/window_utils.hpp>

namespace {
    struct glfw_interop_t {
        glfw_interop_t() {
            glfwInit();
        }

        ~glfw_interop_t() {
            glfwTerminate();
        }
    };

    std::optional<glfw_interop_t> glfw_interop;
    bool debug_output_enabled = false;

    // Callback used to display OpenGL errors. Derived from implementation at:
    // https://learnopengl.com/In-Practice/Debugging
    void APIENTRY custom_debug_callback(
            GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei,
            const char *message, const void *) {
        // ignore non-significant error/warning codes
        if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

        std::cout << "OpenGL debug message (" << id << "): " << message << std::endl;

        std::cout << "    (";
        switch (source) {
            case 0x8246:
                std::cout << "source: API";
                break;
            case 0x8247:
                std::cout << "source: Window System";
                break;
            case 0x8248:
                std::cout << "source: Shader Compiler";
                break;
            case 0x8249:
                std::cout << "source: Third Party";
                break;
            case 0x824A:
                std::cout << "source: Application";
                break;
            case 0x824B:
                std::cout << "source: Other";
                break;
            default:
                std::cout << "source: Unknown";
                break;
        }
        std::cout << "; ";

        switch (type) {
            case 0x824C:
                std::cout << "type: Error";
                break;
            case 0x824D:
                std::cout << "type: Deprecated Behaviour";
                break;
            case 0x824E:
                std::cout << "type: Undefined Behaviour";
                break;
            case 0x824F:
                std::cout << "type: Portability";
                break;
            case 0x8250:
                std::cout << "type: Performance";
                break;
            case 0x8268:
                std::cout << "type: Marker";
                break;
            case 0x8269:
                std::cout << "type: Push Group";
                break;
            case 0x826A:
                std::cout << "type: Pop Group";
                break;
            case 0x8251:
                std::cout << "type: Other";
                break;
            default:
                std::cout << "type: Unknown";
                break;
        }
        std::cout << "; ";

        switch (severity) {
            case 0x9146:
                std::cout << "severity: high";
                break;
            case 0x9147:
                std::cout << "severity: medium";
                break;
            case 0x9148:
                std::cout << "severity: low";
                break;
            case 0x826B:
                std::cout << "severity: notification";
                break;
            default:
                std::cout << "severity: unknown";
                break;
        }
        std::cout << ")" << std::endl;
    }

    typedef void (APIENTRY *debugproc_t)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar *, const void *);

    // Registers debug output callback with OpenGL
    void register_debug_output() {
        GLint flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (flags & 0x00000002) { // GL_CONTEXT_FLAG_DEBUG_BIT
            std::cout << "Debug output enabled" << std::endl;
            glEnable(0x92e0); // GL_DEBUG_OUTPUT
            glEnable(0x8242); // GL_DEBUG_OUTPUT_SYNCHRONOUS
            // This is a bit nasty as we can't use GLAD - the version of GLAD included
            // only supports OpenGL 3.3. FWIW this is what OpenGL looks like if you
            // don't use a loader! haha.
            auto p_glDebugMessageCallback =
                    reinterpret_cast<void (*)(debugproc_t, const void *)>(
                            glfwGetProcAddress("glDebugMessageCallback"));
            auto p_glDebugMessageControl =
                    reinterpret_cast<void (*)(GLenum, GLenum, GLenum,
                                              GLsizei, const GLuint *, GLboolean)>(
                            glfwGetProcAddress("glDebugMessageControl"));
            p_glDebugMessageCallback(custom_debug_callback, nullptr);
            p_glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        }
    }
}

namespace chicken3421 {
    GLFWwindow *make_window(int width, int height, const std::string &title) {
        glfw_interop.emplace();

        if (debug_output_enabled) {
            // load 4.3 and request a debug context if enable_debug_output() called
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
        } else {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        }

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
#endif

        GLFWwindow *win = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);

        return win;
    }

    bool make_opengl_context(GLFWwindow *win) {
        glfwMakeContextCurrent(win);
        return gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    }

    GLFWwindow *make_opengl_window(int width, int height, const std::string &title) {
        GLFWwindow *win = make_window(width, height, title);
        expect(win, "Could not create a new window");

        bool success = make_opengl_context(win);
        expect(success, "Could not load OpenGL");

        if (debug_output_enabled) {
            register_debug_output();
        }
        return win;
    }

    void enable_debug_output() {
        // this flag is used in make_window
#ifndef __APPLE__
        debug_output_enabled = true;
#endif
    }


    void delete_opengl_window(GLFWwindow *w) {
        glfwDestroyWindow(w);
    }

}

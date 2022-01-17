#ifndef CHICKEN3421_GL_UTILS_HPP
#define CHICKEN3421_GL_UTILS_HPP

#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace chicken3421 {
    /**
     *
     * Reads in the shader source code in `path` and creates a new shader object of type `shader-type`
     *
     * @param path: filesystem path to the shader
     * @param shader_type: one of GL_VERTEX_SHADER, GL_FRAGMENT_SHADER
     * @return A handle to the new shader object
     */
    GLuint make_shader(const std::string &path, GLenum shader_type);

    /**
     *
     * Schedules a shader for deletion with OpenGL.
     *
     * Typically, once a rendering program has been linked together and is valid,
     * the source shaders no longer need to exist. In case like those,
     * it is "environmentally friendly" to clean up after ourselves
     *
     * @param shader: the shader to delete.
     */
    void delete_shader(GLuint shader);

    /**
     *
     * Get the log of the most recent shader operation.
     * Useful for debugging shader compilation, among other things
     *
     * @param shader The shader whose log to fetch
     * @return A string of the log.
     */
    std::string get_shader_log(GLuint shader);

    /**
     *
     * Creates a new program comprised only of the vertex shader and fragment shader
     *
     * @param vs: the vertex shader
     * @param fs: the fragment shader
     * @return: a renderable program
     */
    GLuint make_program(GLuint vs, GLuint fs);


    /**
     *
     * Deletes a rendering program
     *
     * @param program
     */
    void delete_program(GLuint program);

    /**
     *
     * Checks the validation status of the program called program.
     * Validation is defined to be:
     *    Given the current state of the OpenGL state machine (bound VAOs, textures, etc.)
     *    could this program execute and produce meaningful output?
     *
     * The validation status of a program therefore naturally changes between calls as other
     * gl functions modify the opengl context.
     *
     * @param program The program to validate
     * @return True if this program is valid given the current context, false otherwise
     */
    bool validate_program(GLuint program);

    /**
     *
     * Get the log of the most recent program operation.
     * Useful for debugging program validation, linking, use, etc.
     *
     * @param program The program whose log to fetch
     * @return A string of the log.
     */
    std::string get_program_log(GLuint program);

    /**
     *
     * Creates a single new Vertex Array Object
     *
     * @return: The new vertex array
     */
    GLuint make_vao();

    /**
     *
     * Schedules the deletion of a Vertex Array Object
     *
     * @param vao: The VAO to delete.
     */
    void delete_vao(GLuint vao);

    /**
     *
     * Creates a new single generic Buffer Object
     *
     * @return: The new buffer
     */
    GLuint make_buffer();

    /**
     * Schedules the deletion of the buffer object with OpenGL.
     *
     * @param buff: The buffer to delete.
     */
    void delete_buffer(GLuint buff);

    /**
     * Create an empty texture object.
     *
     * Unfortunately, texture set-up is so specific to one's needs we
     * cannot provide much more functionality than providing a nicer way to
     * create a texture object.
     *
     * Binding and filling is the responsibility of the caller.
     *
     * @return A handle to the created texture object
     */
    GLuint make_texture();

    /**
     *
     * Schedules the deletion of the texture object with OpenGL.
     *
     * @param tex: The texture to delete.
     */
    void delete_texture(GLuint tex);

    /**
     * Get the location index of the uniform `name` in `program`.
     *
     * @param program The program containing the uniforms
     * @param name The name of the uniform whose location you're interested in
     * @return a GLint containing the index of the uniform
     * @throws std::runtime_error if the program does not contain a uniform named `name`.
     */
    GLint get_uniform_location(GLuint program, const std::string &name);

    void set_uniform(GLuint program, const std::string &name, float value);

    void set_uniform(GLuint program, const std::string &name, int value);

    void set_uniform(GLuint program, const std::string &name, glm::vec4 value);

    void set_uniform(GLuint program, const std::string &name, glm::vec3 value);

    void set_uniform(GLuint program, const std::string &name, const glm::mat4 &value);

    /**
     * Create an empty framebuffer object.
     *
     * Unfortunately, framebuffer set-up is so specific to one's needs we
     * cannot provide much more functionality than providing a nicer way to
     * create an FBO.
     *
     * Binding and filling is the responsibility of the caller.
     *
     * @return A handle to the created framebuffer object
     */
    GLuint make_framebuffer();

    /**
     *
     * Schedules the deletion of the framebuffer object with OpenGL.
     *
     * @param fbo: The framebuffer to delete.
     */
    void delete_framebuffer(GLuint fbo);

    /**
     * Create an empty renderbuffer object.
     *
     * Unfortunately, much like most OpenGL objects, complete initialisation is so specific we cannot provide support
     * more than this.
     *
     * We leave full initialisation to the caller.
     *
     * @return The newly created renderbuffer object.
     */
    GLuint make_renderbuffer();

    /**
     *
     * Schedules the deletion of the renderbuffer object with OpenGL.
     *
     * @param rbo: The renderbuffer to delete.
     */
    void delete_renderbuffer(GLuint rbo);
}

#endif //CHICKEN3421_GL_UTILS_HPP

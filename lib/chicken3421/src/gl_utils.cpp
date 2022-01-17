#include <chicken3421/error_utils.hpp>

#include <chicken3421/file_utils.hpp>

#include <chicken3421/gl_utils.hpp>

#include <glm/ext.hpp>

namespace chicken3421 {
    GLuint make_shader(const std::string &path, GLenum shader_type) {
        const std::string s_src = read_file(path);

        GLuint shad = glCreateShader(shader_type);
        const char *shader_src = s_src.data();
        glShaderSource(shad, 1, &shader_src, nullptr);
        glCompileShader(shad);

        GLint did_compile;
        glGetShaderiv(shad, GL_COMPILE_STATUS, &did_compile);

        expect(did_compile, get_shader_log(shad));

        return shad;
    }

    void delete_shader(GLuint shader) {
        glDeleteShader(shader);
    }

    std::string get_program_log(GLuint program) {
        int log_len;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);

        std::string log = std::string(log_len, 0);
        glGetProgramInfoLog(program, log_len, nullptr, log.data());

        return log;
    }

    GLuint make_program(GLuint vs, GLuint fs) {
        GLuint prog = glCreateProgram();
        glAttachShader(prog, vs);
        glAttachShader(prog, fs);

        glLinkProgram(prog);
        GLint did_link;
        glGetProgramiv(prog, GL_LINK_STATUS, &did_link);
        expect(did_link, get_program_log(prog));


        return prog;
    }

    void delete_program(GLuint program) {
        glDeleteProgram(program);
    }

    bool validate_program(GLuint program) {
        glValidateProgram(program);
        GLint is_valid;
        glGetProgramiv(program, GL_VALIDATE_STATUS, &is_valid);

        return is_valid;
    }


    std::string get_shader_log(GLuint shader) {
        int log_len;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);

        std::string log = std::string(log_len, 0);
        glGetShaderInfoLog(shader, log_len, nullptr, log.data());

        return log;
    }

    GLuint make_vao() {
        GLuint vao;
        glGenVertexArrays(1, &vao);

        return vao;
    }

    void delete_vao(GLuint vao) {
        glDeleteVertexArrays(1, &vao);
    }

    GLuint make_buffer() {
        GLuint buff;
        glGenBuffers(1, &buff);
        return buff;
    }

    void delete_buffer(GLuint buff) {
        glDeleteBuffers(1, &buff);
    }

    GLuint make_texture() {
        GLuint tex;
        glGenTextures(1, &tex);
        return tex;
    }


    void delete_texture(GLuint tex) {
        glDeleteTextures(1, &tex);
    }

    GLint get_uniform_location(GLuint program, const std::string &name) {
        GLint loc = glGetUniformLocation(program, name.data());
        expect(loc != -1, "ERROR: No uniform variable named: " + name + "in program: " + std::to_string(loc));
        return loc;
    }

    void set_uniform(GLuint program, const std::string &name, float value) {
        auto location = get_uniform_location(program, name);
        glUniform1f(location, value);
    }

    void set_uniform(GLuint program, const std::string &name, int value) {
        auto location = get_uniform_location(program, name);
        glUniform1i(location, value);
    }

    void set_uniform(GLuint program, const std::string &name, glm::vec3 value) {
        auto location = get_uniform_location(program, name);
        glUniform3fv(location, 1, glm::value_ptr(value));
    }

    void set_uniform(GLuint program, const std::string &name, glm::vec4 value) {
        auto location = get_uniform_location(program, name);
        glUniform4fv(location, 1, glm::value_ptr(value));
    }

    void set_uniform(GLuint program, const std::string &name, const glm::mat4 &value) {
        auto location = get_uniform_location(program, name);
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    GLuint make_framebuffer() {
        GLuint fbo;
        glGenFramebuffers(1, &fbo);
        return fbo;
    }

    void delete_framebuffer(GLuint fbo) {
        glDeleteFramebuffers(1, &fbo);
    }


    GLuint make_renderbuffer() {
        GLuint rbo;
        glGenRenderbuffers(1, &rbo);
        return rbo;
    }


    void delete_renderbuffer(GLuint rbo) {
        glDeleteRenderbuffers(1, &rbo);

    }
}
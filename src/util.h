#ifndef TETRIS_UTIL_H
#define TETRIS_UTIL_H

#if defined(__APPLE__)
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>


class Texture {
public:
    const GLuint width, height;
    Texture() : width(0), height(0) {};
    Texture(GLenum format, GLuint width, GLuint height, GLubyte* image);
    
    void bind() const { glBindTexture(GL_TEXTURE_2D, id_); }

private:
    GLuint id_;
};


class Shader {
public:
    Shader(const GLchar *sourceVertex, const GLchar *sourceFragment);
    
    void setFloat(const GLchar *name, GLfloat value) const {
        glUniform1f(glGetUniformLocation(id_, name), value);
    }
    
    void setMat4(const GLchar *name, const glm::mat4 &matrix) const {
        glUniformMatrix4fv(glGetUniformLocation(id_, name), 1, GL_FALSE, glm::value_ptr(matrix));
    }
    
    void setVec3(const GLchar *name, glm::vec3 vec) const {
        glUniform3f(glGetUniformLocation(id_, name), vec.x, vec.y, vec.z);
    }
    
    void setVec2(const GLchar *name, glm::vec2 vec) const {
        glUniform2f(glGetUniformLocation(id_, name), vec.x, vec.y);
    }
    
    void use() const { glUseProgram(id_); }

private:
    GLuint id_;
};


struct Glyph {
    Texture texture;
    glm::ivec2 bearing;
    GLint64 advance;
};


std::vector<Glyph> loadFont(const std::string& path, unsigned int glyphHeight);
Texture loadRgbaTexture(const std::string& path);


#endif //TETRIS_UTIL_H

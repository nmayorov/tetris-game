#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "util.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "stb_image.h"


Shader::Shader(const GLchar *sourceVertex, const GLchar *sourceFragment) {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &sourceVertex, NULL);
    glCompileShader(vertexShader);
    
    GLchar infoLog[512];
    GLint success;
    
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (success == 0) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "Error compiling vector shader: " << infoLog << std::endl;
    }
    
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &sourceFragment, NULL);
    glCompileShader(fragmentShader);
    
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (success == 0) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "Error compiling fragment shader: " << infoLog << std::endl;
    }
    
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == 0) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "Error linking shader program: " << infoLog << std::endl;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    id_ = program;
}


Texture loadRgbaTexture(const std::string& path) {
    stbi_set_flip_vertically_on_load(1);
    int width, height, numChannels;
    GLubyte *image = stbi_load(path.c_str(), &width, &height, &numChannels, 4);
    Texture texture(GL_RGBA, width, height, image);
    stbi_image_free(image);
    return texture;
}


Texture::Texture(GLenum format, GLuint width, GLuint height, GLubyte* image) : width(width), height(height)  {
    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}


std::vector<Glyph> loadFont(const std::string& path, unsigned int glyphHeight) {
    FT_Library ft;
    FT_Init_FreeType(&ft);
    
    FT_Face face;
    FT_New_Face(ft, path.c_str(), 0, &face);
    
    FT_Set_Pixel_Sizes(face, 0, glyphHeight);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    std::vector<Glyph> glyphs;
    for (GLubyte c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cerr << "Failed to load glyph " << c << "." << std::endl;
            continue;
        }
        
        Texture texture(GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, face->glyph->bitmap.buffer);
        Glyph glyph = {texture,
                       glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                       face->glyph->advance.x >> 6};
    
        glyphs.push_back(glyph);
    }

    // Should handle face as well.
    FT_Done_FreeType(ft);
    
    return glyphs;
}

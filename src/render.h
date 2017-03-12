#ifndef TETRIS_RENDER_H
#define TETRIS_RENDER_H

#include <iostream>
#include <map>
#include <vector>

#if defined(__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#else 
#define GLEW_STATIC
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "tetris.h"
#include "util.h"


extern const glm::vec3 kColorBlack;
extern const glm::vec3 kColorWhite;


class SpriteRenderer {
public:
    SpriteRenderer(const glm::mat4& projection);
    
    void render(const Texture &texture, GLfloat x, GLfloat y, GLfloat width, GLfloat height, GLfloat mixCoeff = 0,
                const glm::vec3& mixColor = kColorBlack, GLfloat alphaMultiplier = 1);
    
private:
    Shader shader_;
    GLuint vao_;
};


class PieceRenderer {
public:
    PieceRenderer(GLfloat tileSize, const std::map<TileColor, Texture>& textures, SpriteRenderer& spriteRenderer)
            : tileSize_(tileSize), textures_(textures), spriteRenderer_(spriteRenderer)  {}
    
    void renderShape(const Piece &piece, GLfloat x, GLfloat y, GLfloat mixCoeff = 0,
                     const glm::vec3& mixColor = kColorBlack, GLfloat alphaMultiplier = 1, int startRow = 0) const;
    void renderInitialShape(const Piece& piece, GLfloat x, GLfloat y) const;
    void renderInitialShapeCentered(const Piece& piece, GLfloat x, GLfloat y, GLfloat width, GLfloat height) const;

private:
    GLfloat tileSize_;
    std::map<TileColor, Texture> textures_;
    SpriteRenderer& spriteRenderer_;
};


class BoardRenderer {
public:
    BoardRenderer(const glm::mat4& projection, GLfloat tileSize, GLfloat x, GLfloat y,
                  int nRows, int nCols, const std::map<TileColor, Texture>& tileTextures,
                  SpriteRenderer& spriteRenderer, PieceRenderer& pieceRenderer, PieceRenderer& ghostRenderer);
    
    void renderBackground() const;
    void renderTiles(const Board& board, GLfloat alphaMultiplier = 1) const;
    void renderPiece(const Piece& piece, int row, int col, double lockPercent, double alphaMultiplier = 1) const;
    void renderGhost(const Piece& piece, int ghostRow, int col) const;
    void playLinesClearAnimation(const Board& board, double percentFinished) const;

private:
    GLfloat tileSize_;
    GLfloat x_, y_;
    int nRows_, nCols_;
    
    const std::map<TileColor, Texture> tileTextures_;
    
    PieceRenderer& pieceRenderer_, ghostRenderer_;
    SpriteRenderer& spriteRenderer_;
    
    Shader backgroundShader_;
    std::vector<GLfloat> verticesBackground_;
    GLuint vaoBackground_;
};


class TextRenderer {
public:
    TextRenderer(const glm::mat4& projection, const std::map<GLubyte, Glyph>& font);
    
    void render(const std::string& text, GLfloat x, GLfloat y, glm::vec3 color) const;
    void renderCentered(const std::string& text, GLfloat x, GLfloat y, GLfloat width, const glm::vec3& color) const;
    
    GLint computeWidth(const std::string& text) const;
    GLint computeHeight(const std::string& text) const;
    
private:
    std::map<GLubyte, Glyph> font_;
    Shader shader_;
    
    GLuint vbo_, vao_;
};


#endif //TETRIS_RENDER_H

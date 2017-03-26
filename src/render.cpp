#include "render.h"
#define _USE_MATH_DEFINES
#include <math.h>


const char* kColoredPrimitiveVertexShader = R"glsl(
# version 330 core

layout (location = 0) in vec2 position;
uniform mat4 projection;

void main() {
    gl_Position = projection * vec4(position, 0, 1);
}

)glsl";



const char* kColoredPrimitiveFragmentShader = R"glsl(
# version 330 core

uniform vec3 inColor;
out vec4 color;

void main() {
    color = vec4(inColor, 1);
}

)glsl";


const char* kTileVertexShader = R"glsl(
# version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoord;

out vec2 texCoordFragment;

uniform vec2 shift;
uniform vec2 scale = vec2(1, 1);
uniform mat4 projection;

void main() {
    gl_Position = projection * vec4(scale * position + shift, 0, 1);
    texCoordFragment = texCoord;
}
)glsl";


const char* kTileFragmentShader = R"glsl(

# version 330 core

in vec2 texCoordFragment;
out vec4 color;

uniform sampler2D sampler;
uniform vec3 mixColor;
uniform float mixCoeff = 0;
uniform float alphaMultiplier = 1;

void main() {
    color = mix(texture(sampler, texCoordFragment), vec4(mixColor, 1), mixCoeff);
    color.a *= alphaMultiplier;
}

)glsl";


const char* kGlyphVertexShader = R"glsl(

#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoord;

out vec2 texCoordFragment;

uniform mat4 projection;

void main() {
    gl_Position = projection * vec4(position, 0, 1);
    texCoordFragment = texCoord;
}

)glsl";


const char* kGlyphFragmentShader = R"glsl(

#version 330 core

in vec2 texCoordFragment;
out vec4 color;

uniform vec3 textColor;
uniform sampler2D glyph;

void main() {
    float alpha = texture(glyph, texCoordFragment).r;
    color = vec4(textColor, alpha);
}

)glsl";


const glm::vec3 kColorBlack(0, 0, 0);
const glm::vec3 kColorWhite(1, 1, 1);


SpriteRenderer::SpriteRenderer(const glm::mat4& projection) : shader_(kTileVertexShader, kTileFragmentShader) {
    GLfloat vertices[] = {0, 0, 0, 1,
                          0, 1, 0, 0,
                          1, 0, 1, 1,
                          1, 1, 1, 0};
    
    GLuint vbo;
    glGenBuffers(1, &vbo);
    
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    
    shader_.use();
    shader_.setMat4("projection", projection);
}


void SpriteRenderer::render(const Texture& texture, GLfloat x, GLfloat y, GLfloat width, GLfloat height,
                            GLfloat mixCoeff, const glm::vec3& mixColor, GLfloat alphaMultiplier) {
    texture.bind();
    shader_.use();
    shader_.setVec2("shift", glm::vec2(x, y));
    shader_.setVec2("scale", glm::vec2(width, height));
    shader_.setFloat("mixCoeff", mixCoeff);
    shader_.setVec3("mixColor", mixColor);
    shader_.setFloat("alphaMultiplier", alphaMultiplier);
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


void PieceRenderer::renderShape(const Piece& piece, GLfloat x, GLfloat y, GLfloat mixCoeff, const glm::vec3& mixColor,
                                GLfloat alphaMultiplier, int startRow) const {
    if (piece.kind() == kNone)
        return;
    
    Texture texture = textures_.at(piece.color());
    
    int index = startRow * piece.bBoxSide();
    auto shape = piece.shape();
    for (int row = startRow; row < piece.bBoxSide(); ++row) {
        for (int col = 0; col < piece.bBoxSide(); ++col) {
            if (shape[index] != kEmpty)
                spriteRenderer_.render(texture, x + col * tileSize_, y + row * tileSize_,
                                       tileSize_, tileSize_, mixCoeff, mixColor, alphaMultiplier);
            
            ++index;
        }
    }
}


void PieceRenderer::renderInitialShape(const Piece& piece, GLfloat x, GLfloat y) const {
    if (piece.kind() == kNone)
        return;
    
    Texture texture = textures_.at(piece.color());
    
    int index = 0;
    auto shape = piece.initialShape();
    for (int row = 0; row < piece.nRows(); ++row) {
        for (int col = 0; col < piece.nCols(); ++col) {
            if (shape[index] != kEmpty)
                spriteRenderer_.render(texture, x + col * tileSize_, y + row * tileSize_,
                                       tileSize_, tileSize_);
            
            ++index;
        }
    }
}


void PieceRenderer::renderInitialShapeCentered(const Piece& piece, GLfloat x, GLfloat y,
                                               GLfloat width, GLfloat height) const {
    GLfloat pieceWidth = tileSize_ * piece.nCols();
    GLfloat pieceHeight = tileSize_ * piece.nRows();
    GLfloat xShift = 0.5f * (width - pieceWidth);
    GLfloat yShift = 0.5f * (height - pieceHeight);
    
    renderInitialShape(piece, x + xShift, y + yShift);
}


const glm::vec3 kBackgroundColor(0.05, 0.05, 0.05);
const glm::vec3 kGridColor(0.2, 0.2, 0.2);


BoardRenderer::BoardRenderer(const glm::mat4& projection, GLfloat tileSize, GLfloat x, GLfloat y,
                             int nRows, int nCols, const std::vector<Texture>& tileTextures,
                             SpriteRenderer &spriteRenderer, PieceRenderer &pieceRenderer, PieceRenderer &ghostRenderer)
        : tileSize_(tileSize),
          x_(x), y_(y),
          nRows_(nRows), nCols_(nCols),
          tileTextures_(tileTextures),
          pieceRenderer_(pieceRenderer), ghostRenderer_(ghostRenderer), spriteRenderer_(spriteRenderer),
          backgroundShader_(kColoredPrimitiveVertexShader, kColoredPrimitiveFragmentShader) {
    backgroundShader_.use();
    backgroundShader_.setMat4("projection", projection);
    
    GLfloat width = nCols_ * tileSize_;
    GLfloat height = nRows_ * tileSize_;
    
    verticesBackground_ = {x_, y_,
                           x_, y_ + height,
                           x_ + width, y_,
                           x_ + width, y_ + height};
    
    GLfloat yGrid = y_;
    for (int row = 0; row < nRows_ + 1; ++row) {
        verticesBackground_.push_back(x_);
        verticesBackground_.push_back(yGrid);
        verticesBackground_.push_back(x_ + width);
        verticesBackground_.push_back(yGrid);
        yGrid += tileSize_;
    }
    
    GLfloat xGrid = x_;
    for (int col = 0; col < nCols_ + 1; ++col) {
        verticesBackground_.push_back(xGrid);
        verticesBackground_.push_back(y_);
        verticesBackground_.push_back(xGrid);
        verticesBackground_.push_back(y_ + height);
        xGrid += tileSize_;
    }
    
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vaoBackground_);
    
    glBindVertexArray(vaoBackground_);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verticesBackground_.size() * sizeof(GLfloat), verticesBackground_.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}


void BoardRenderer::renderBackground() const {
    backgroundShader_.use();
    glBindVertexArray(vaoBackground_);
    
    backgroundShader_.setVec3("inColor", kBackgroundColor);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    backgroundShader_.setVec3("inColor", kGridColor);
    glDrawArrays(GL_LINES, 4, 2 * (nRows_ + nCols_ + 2));
}


void BoardRenderer::renderTiles(const Board &board, GLfloat alphaMultiplier) const {
    int row, col;
    GLfloat y, x;
    GLfloat y0 = y_;
    GLfloat x0 = x_;
    
    for (row = 0, y = y0; row < board.nRows; ++row, y += tileSize_) {
        for (col = 0, x = x0; col < board.nCols; ++col, x += tileSize_) {
            TileColor tile = board.tileAt(row, col);
            if (tile == kEmpty)
                continue;
            
            spriteRenderer_.render(tileTextures_.at(tile), x, y,
                                   tileSize_, tileSize_, 0, glm::vec3(), alphaMultiplier);
        }
    }
    
}


void BoardRenderer::renderPiece(const Piece &piece, int row, int col, double lockPercent,
                                double alphaMultiplier) const {
    int startRow = std::max(0, -row);
    GLfloat mixCoeff = 0.5f * sin(M_PI_2 * lockPercent);
    pieceRenderer_.renderShape(piece, x_ + col * tileSize_, y_ + row * tileSize_,
                               mixCoeff, kColorBlack, alphaMultiplier, startRow);
}


void BoardRenderer::renderGhost(const Piece &piece, int ghostRow, int col) const {
    int startRow = std::max(0, -ghostRow);
    ghostRenderer_.renderShape(piece, x_ + col * tileSize_, y_ + ghostRow * tileSize_,
                               0, kColorBlack, 0.7, startRow);
}


void BoardRenderer::playLinesClearAnimation(const Board &board, double percentFinished) const {
    double t = 0.3;
    
    glm::vec3 mixColor;
    GLfloat mixCoeff;
    
    if (percentFinished < t) {
        double s = sin(M_PI * percentFinished / t);
        mixColor = kColorWhite;
        mixCoeff = 0.8f * s;
    } else {
        mixColor = kBackgroundColor;
        mixCoeff = (percentFinished - t) / (1 - t);
    }

    for (int row : board.linesToClear()) {
        for (int col = 0; col < nCols_; ++col) {
            GLfloat x = x_ + col * tileSize_;
            GLfloat y = y_ + row * tileSize_;
            spriteRenderer_.render(tileTextures_.at(board.tileAt(row, col)),
                                    x, y, tileSize_, tileSize_, mixCoeff, mixColor, 1);
        }
    }
}


TextRenderer::TextRenderer(const glm::mat4& projection, const std::vector<Glyph>& font) :
        font_(font), shader_(kGlyphVertexShader, kGlyphFragmentShader) {
    shader_.use();
    shader_.setMat4("projection", projection);
    
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void TextRenderer::render(const std::string& text, GLfloat x, GLfloat y, glm::vec3 color) const {
    shader_.use();
    shader_.setVec3("textColor", color);
    glBindVertexArray(vao_);
    
    x = std::round(x);
    y = std::round(y);
    
    for (char c : text) {
        Glyph glyph = font_.at(c);
        
        GLfloat xBbox = x + glyph.bearing.x;
        GLfloat yBbox = y + (font_.at('A').bearing.y - glyph.bearing.y);
        
        GLfloat width = glyph.texture.width;
        GLfloat height = glyph.texture.height;
        
        GLfloat vertices[] = {xBbox, yBbox, 0, 0,
                              xBbox, yBbox + height, 0 ,1,
                              xBbox + width, yBbox, 1, 0,
                              xBbox + width, yBbox + height, 1, 1};
        
        glyph.texture.bind();
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        
        x += glyph.advance;
    }
}


void TextRenderer::renderCentered(const std::string& text, GLfloat x, GLfloat y, GLfloat width,
                                  const glm::vec3& color) const {
    GLfloat textWidth = computeWidth(text);
    GLfloat shift = 0.5f * (width - textWidth);
    render(text, std::round(x + shift), std::round(y), color);
}


GLint TextRenderer::computeWidth(const std::string& text) const {
    GLint width = 0;
    for (auto c = text.begin(); c != text.end() - 1; ++c) {
        width += font_.at(*c).advance;
    }
    width += font_.at(text.back()).texture.width;
    return width;
}


GLint TextRenderer::computeHeight(const std::string& text) const {
    GLint height = 0;
    for (char c : text) {
        Glyph glyph = font_.at(c);
        GLint textureHeight = static_cast<GLint>(glyph.texture.height);
        height = std::max(height, font_.at('H').bearing.y - glyph.bearing.y + textureHeight);
    }
    return height;
}

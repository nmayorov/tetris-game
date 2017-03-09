#include <map>
#include <string>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include "render.h"


const GLfloat kTileSize = 32;
const GLint kBoardNumRows = 20;
const GLint kBoardNumCols = 10;
const GLfloat kBoardWidth = kBoardNumCols * kTileSize;
const GLfloat kBoardHeight = kBoardNumRows * kTileSize;
const GLfloat kMargin = 10;
const GLfloat kHudWidth = 160;
const GLint kWidth = 3 * kMargin + kBoardWidth + kHudWidth;
const GLint kHeight = 2 * kMargin + kBoardHeight;
const GLfloat kHudX = kMargin;
const GLfloat kHudY = kMargin;
const GLfloat kBoardX = 2 * kMargin + kHudWidth;
const GLfloat kBoardY = kMargin;
const GLfloat kHudPieceBoxHeight = 2.5f * kTileSize;
const GLuint kFontSize = 18;


const double kGameTimeStep = 0.005;
const double kFps = 30;
const double kSecondsPerFrame = 1.0 / kFps;


Board board(kBoardNumRows, kBoardNumCols);
Tetris* tetris;


enum GameState {kGameStart, kGameRun, kGamePaused, kGameOver};
GameState gameState = kGameStart;


bool softDrop = false;
bool moveRight = false;
bool moveLeft = false;
int startLevel = 1;


GLFWwindow* setupGlContext() {
    if (!glfwInit())
        return nullptr;
    
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFWwindow *window = glfwCreateWindow(kWidth, kHeight, "TETRIS", nullptr, nullptr);
    
    if (window == nullptr)
        glfwTerminate();

	glfwMakeContextCurrent(window);

#if defined(WIN32)
	glewExperimental = GL_TRUE;
	glewInit();
#endif

    return window;
}


void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    switch (gameState) {
        case kGameRun:
            if (action == GLFW_PRESS) {
                switch (key) {
                    case GLFW_KEY_Z:
                        tetris->rotate(Rotation::kLeft);
                        break;
                    case GLFW_KEY_X:
                        tetris->rotate(Rotation::kRight);
                        break;
                    case GLFW_KEY_SPACE:
                        tetris->hardDrop();
                        break;
                    case GLFW_KEY_C:
                        tetris->hold();
                        break;
                    case GLFW_KEY_LEFT:
                        moveLeft = true;
                        break;
                    case GLFW_KEY_RIGHT:
                        moveRight = true;
                        break;
                    case GLFW_KEY_DOWN:
                        softDrop = true;
                        break;
                    case GLFW_KEY_ESCAPE:
                        gameState = kGamePaused;
                }
            } else if (action == GLFW_RELEASE) {
                switch (key) {
                    case GLFW_KEY_LEFT:
                        moveLeft = false;
                        break;
                    case GLFW_KEY_RIGHT:
                        moveRight = false;
                        break;
                    case GLFW_KEY_DOWN:
                        softDrop = false;
                        break;
                }
            }
            break;
        case kGamePaused:
            if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
                gameState = kGameRun;
            else if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
                gameState = kGameStart;
            break;
        case kGameOver:
            if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
                gameState = kGameStart;
            break;
        case kGameStart:
            if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
                moveRight = false;
                moveLeft = false;
                softDrop = false;
                tetris->restart(startLevel);
                gameState = kGameRun;
            } else if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
                startLevel = std::min(15, startLevel + 1);
            } else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
                startLevel = std::max(1, startLevel - 1);
            }
    }
}


void windowFocusCallback(GLFWwindow* window, int focused) {
    if (!focused && gameState == kGameRun)
        gameState = kGamePaused;
}


int main() {
    GLFWwindow* window = setupGlContext();
    
	if (window == nullptr)
        return EXIT_FAILURE;
    
    auto font = loadFont("resources/kenvector_future.ttf", kFontSize);    
    
	std::map<TileColor, Texture> tileTextures, ghostTextures;
    std::vector<std::string> colors = {"cyan", "blue", "orange", "yellow", "green", "purple", "red"};
    for (int color = kCyan; color <= kRed; ++color) {
        std::string path = "resources/tile_" + colors[color] + ".png";
        tileTextures.insert(std::make_pair(static_cast<TileColor>(color), loadRgbaTexture(path)));
        path = "resources/contour_" + colors[color] + ".png";
        ghostTextures.insert(std::make_pair(static_cast<TileColor>(color), loadRgbaTexture(path)));
    }
    
    Texture keyArrowLeft = loadRgbaTexture("resources/Keyboard_White_Arrow_Left.png");
    Texture keyArrowRight = loadRgbaTexture("resources/Keyboard_White_Arrow_Right.png");
    Texture keyArrowDown = loadRgbaTexture("resources/Keyboard_White_Arrow_Down.png");
    Texture keyArrowUp = loadRgbaTexture("resources/Keyboard_White_Arrow_Up.png");
    Texture keyZ = loadRgbaTexture("resources/Keyboard_White_Z.png");
    Texture keyX = loadRgbaTexture("resources/Keyboard_White_X.png");
    Texture keyC = loadRgbaTexture("resources/Keyboard_White_C.png");
    Texture keySpace = loadRgbaTexture("resources/Keyboard_White_Space.png");
    Texture keyEsc = loadRgbaTexture("resources/Keyboard_White_Esc.png");
    Texture keyEnter = loadRgbaTexture("resources/Keyboard_White_Enter.png");
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(kWidth), static_cast<GLfloat>(kHeight),
                                      0.0f, -1.0f, 1.0f);
    
    tetris = new Tetris(board, kGameTimeStep, static_cast<unsigned int>(glfwGetTime() * 1e4));
    
    glfwSetKeyCallback(window, keyCallback);
    glfwSetWindowFocusCallback(window, windowFocusCallback);
    
    TextRenderer textRenderer(projection, font);
    
    GLfloat letterHeight = textRenderer.computeHeight("A");
    GLfloat letterWidth = textRenderer.computeWidth("A");
    
    SpriteRenderer spriteRenderer(projection);
    PieceRenderer pieceRenderer(kTileSize, tileTextures, spriteRenderer);
    PieceRenderer ghostRenderer(kTileSize, ghostTextures, spriteRenderer);
    BoardRenderer boardRenderer(projection, kTileSize, kBoardX, kBoardY, kBoardNumRows, kBoardNumCols,
                                tileTextures, spriteRenderer, pieceRenderer, ghostRenderer);
    
    double timeLastGameUpdate = 0;
    double timeLastRender = 0;
    
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    
        if (gameState == kGameRun) {
            double time = glfwGetTime();
            if (time - timeLastGameUpdate >= kGameTimeStep) {
                timeLastGameUpdate = time;
                tetris->update(softDrop, moveRight, moveLeft);
            }
            
            if (tetris->isGameOver())
                gameState = kGameOver;
        }
        
        double time = glfwGetTime();
        if (time - timeLastRender >= kSecondsPerFrame) {
            timeLastRender = time;
            
            glClearColor(1, 1, 1, 1);
            glClear(GL_COLOR_BUFFER_BIT);
    
            textRenderer.renderCentered("NEXT", kHudX, kHudY, kHudWidth, kColorBlack);
            textRenderer.renderCentered("HOLD", kHudX, kHudY + 2 * kHudPieceBoxHeight, kHudWidth, kColorBlack);
            
            if (gameState != kGameStart) {
                pieceRenderer.renderInitialShapeCentered(
                        tetris->nextPiece(), kHudX, std::round(kHudY + 1.5f * letterHeight),
                        kHudWidth, kHudPieceBoxHeight);
    
                pieceRenderer.renderInitialShapeCentered(
                        tetris->heldPiece(), kHudX,
                        std::round(kHudY + 2 * kHudPieceBoxHeight + 1.5f * letterHeight),
                        kHudWidth, kHudPieceBoxHeight);
            }
    
            int level, linesCleared, score;
            if (gameState == kGameStart) {
                level = startLevel;
                linesCleared = 0;
                score = 0;
            } else {
                level = tetris->level();
                linesCleared = tetris->linesCleared();
                score = tetris->score();
            }
            
            GLfloat y = 0.6f * kHeight;
            textRenderer.renderCentered("LEVEL", kHudX, y, kHudWidth, kColorBlack);
            y += 1.4 * letterHeight;
            textRenderer.renderCentered(std::to_string(level), kHudX, y, kHudWidth, kColorBlack);
            
            y += 2.5 * letterHeight;
            textRenderer.renderCentered("LINES", kHudX, y, kHudWidth, kColorBlack);
            y += 1.4 * letterHeight;
            textRenderer.renderCentered(std::to_string(linesCleared), kHudX, y, kHudWidth, kColorBlack);
            
            y += 2.5 * letterHeight;
            textRenderer.renderCentered("SCORE", kHudX, y, kHudWidth, kColorBlack);
            y += 1.4 * letterHeight;
            textRenderer.renderCentered(std::to_string(score), kHudX, y, kHudWidth, kColorBlack);
    
            boardRenderer.renderBackground();
            
            switch (gameState) {
                case kGameRun:
                    boardRenderer.renderTiles(board);
                    if (tetris->isPausedForLineClear()) {
                        boardRenderer.playLinesClearAnimation(board, tetris->lineClearPausePercent());
                    } else {
                        boardRenderer.renderGhost(board.piece(), board.ghostRow(), board.pieceCol());
                        boardRenderer.renderPiece(board.piece(), board.pieceRow(), board.pieceCol(),
                                                  tetris->lockPercent());
                    }
                    break;
                case kGamePaused: {
                    boardRenderer.renderTiles(board, 0.4);
                    boardRenderer.renderPiece(board.piece(), board.pieceRow(), board.pieceCol(), 0, 0.4);
    
                    GLfloat y = kBoardY + 0.38f * kBoardHeight;
                    
                    textRenderer.renderCentered("PAUSED", kBoardX, y, kBoardWidth, kColorWhite);
                    
                    
                    y = kBoardY + 0.5f * kBoardHeight;
                    GLfloat xName = kBoardX + 0.1f * kBoardWidth;
                    GLfloat xIcon = kBoardX + 0.9f * kBoardWidth;
    
                    GLfloat dyAlignment = 0.5f * (keyArrowLeft.height - letterHeight);
                    textRenderer.render("CONTINUE", xName, y, kColorWhite);
                    spriteRenderer.render(keyEsc, xIcon - keyEsc.width, y - dyAlignment, keyEsc.width, keyEsc.height);
                    
                    y += 5.5 * letterHeight;
                    textRenderer.render("START SCREEN", xName, y, kColorWhite);
                    dyAlignment = 0.75f * (keyEnter.height - letterHeight);
                    spriteRenderer.render(keyEnter, xIcon - keyEnter.width, y - dyAlignment,
                                          keyEnter.width, keyEnter.height);
                    
                    break;
                }
                case kGameStart: {
                    GLfloat y = kBoardY + 0.05f * kBoardHeight;
    
                    textRenderer.renderCentered("CONTROLS", kBoardX, y, kBoardWidth, kColorWhite);
    
                    y += 4 * letterHeight;
    
                    GLfloat xName = kBoardX + 0.1f * kBoardWidth;
                    GLfloat xIcon = kBoardX + 0.9f * kBoardWidth;
                    GLfloat dyAlignment = 0.5f * (keyArrowLeft.height - letterHeight);
                    GLfloat dyBetweenRows = 3.8f * letterHeight;
    
                    textRenderer.render("MOVE", xName, y, kColorWhite);
                    
                    GLfloat iconsWidth = keyArrowLeft.width + keyArrowRight.width;
                    spriteRenderer.render(keyArrowLeft, xIcon - iconsWidth, y - dyAlignment, keyArrowLeft.width,
                                          keyArrowLeft.height);
                    spriteRenderer.render(keyArrowRight, xIcon - iconsWidth + keyArrowLeft.width,
                                          y - dyAlignment, keyArrowLeft.width, keyArrowLeft.height);
    
                    y += dyBetweenRows;
                    textRenderer.render("ROTATE", xName, y, kColorWhite);
                    iconsWidth = keyZ.width + keyX.width;
                    spriteRenderer.render(keyZ, xIcon - iconsWidth, y - dyAlignment, keyZ.width, keyZ.height);
                    spriteRenderer.render(keyX, xIcon - iconsWidth + keyZ.width , y - dyAlignment,
                                          keyX.width, keyZ.height);
    
                    y += dyBetweenRows;
                    textRenderer.render("SOFT DROP", xName, y, kColorWhite);
                    spriteRenderer.render(keyArrowDown, xIcon - keyArrowDown.width, y - dyAlignment,
                                          keyArrowDown.width, keyArrowDown.height);
    
                    y += dyBetweenRows;
                    textRenderer.render("HARD DROP", xName, y, kColorWhite);
                    spriteRenderer.render(keySpace, xIcon - keySpace.width, y - dyAlignment, keySpace.width,
                                          keySpace.height);
    
                    y += dyBetweenRows;
                    textRenderer.render("HOLD", xName, y, kColorWhite);
                    spriteRenderer.render(keyC, xIcon - keyC.width, y - dyAlignment, keyC.width, keyC.height);
    
                    y += dyBetweenRows;
                    textRenderer.render("PAUSE", xName, y, kColorWhite);
                    spriteRenderer.render(keyEsc, xIcon - keyEsc.width, y - dyAlignment, keyEsc.width, keyEsc.height);
    
    
                    y = kBoardY + 0.585f * kBoardHeight;
                    GLfloat lineWidth = textRenderer.computeWidth("USE") + keyArrowDown.width + keyArrowUp.width
                                       + 2 * letterWidth + textRenderer.computeWidth("TO SELECT");
                    GLfloat x = kBoardX + 0.5f * (kBoardWidth - lineWidth);

                    textRenderer.render("USE", x, y, kColorWhite);
                    x += textRenderer.computeWidth("USE") + letterWidth;
                    spriteRenderer.render(keyArrowDown, x, y - dyAlignment, keyArrowDown.width, keyArrowDown.height);
                    x += keyArrowDown.width;
                    spriteRenderer.render(keyArrowUp, x, y - dyAlignment, keyArrowUp.width, keyArrowUp.height);
                    x += keyArrowUp.width + letterWidth;
                    textRenderer.render("TO SELECT", x, y, kColorWhite);
                    y += dyBetweenRows;
                    textRenderer.renderCentered("THE LEVEL", kBoardX, y, kBoardWidth, kColorWhite);
                    
                    y = kBoardY + 0.8f * kBoardHeight;
                    lineWidth = textRenderer.computeWidth("PRESS") + keyEnter.width + 2 * letterWidth +
                                textRenderer.computeWidth("TO START");
                    x = kBoardX + 0.5f * (kBoardWidth - lineWidth);
                    dyAlignment = 0.7f * (keyEnter.height - letterHeight);
                    
                    textRenderer.render("PRESS", x, y, kColorWhite);
                    x += textRenderer.computeWidth("PRESS") + letterWidth;
                    spriteRenderer.render(keyEnter, x, y - dyAlignment, keyEnter.width, keyEnter.height);
                    x += keyEnter.width + letterWidth;
                    textRenderer.render("TO START", x, y, kColorWhite);
                    break;
                }
                case kGameOver: {
                    boardRenderer.renderTiles(board, 0.4);
    
                    GLfloat y = kBoardY + 0.4f * kBoardHeight;
                    textRenderer.renderCentered("GAME OVER", 2 * kMargin + kHudWidth, y, kBoardWidth,
                                                kColorWhite);
    
                    y = kBoardY + 0.53f * kBoardHeight;
                    GLfloat lineWidth = textRenderer.computeWidth("PRESS") + keyEnter.width + 2 * letterWidth +
                                        textRenderer.computeWidth("TO");
                    GLfloat x = kBoardX + 0.5f * (kBoardWidth - lineWidth);
                    GLfloat dyAlignment = 0.7f * (keyEnter.height - letterHeight);
    
                    textRenderer.render("PRESS", x, y, kColorWhite);
                    x += textRenderer.computeWidth("PRESS") + letterWidth;
                    spriteRenderer.render(keyEnter, x, y - dyAlignment, keyEnter.width, keyEnter.height);
                    x += keyEnter.width + letterWidth;
                    textRenderer.render("TO", x, y, kColorWhite);
                    y += 3.5 * letterHeight;
                    textRenderer.renderCentered("CONTINUE", kBoardX, y, kBoardWidth, kColorWhite);
                }
            }
            glfwSwapBuffers(window);
        }
    }
    
    return EXIT_SUCCESS;
}

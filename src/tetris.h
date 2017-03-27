#ifndef TETRIS_TETRIS_H
#define TETRIS_TETRIS_H

#include <algorithm>
#include <cassert>
#include <vector>
#include <random>
#include <iostream>


const int kNumPieces = 7;

enum TileColor{kEmpty = -1, kCyan, kBlue, kOrange, kYellow, kGreen, kPurple, kRed};
enum PieceKind{kNone = -1, kPieceI, kPieceJ, kPieceL, kPieceO, kPieceS, kPieceT, kPieceZ};
enum class Rotation {kRight, kLeft};
enum class Motion {kNone, kRight, kLeft};


class Piece {
public:
    explicit Piece(PieceKind kind);
    
    PieceKind kind() const {return kind_; }
    TileColor color() const {return color_; }
    int bBoxSide() const { return bBoxSide_; }
    int nRows() const { return nRows_; }
    int nCols() const { return nCols_; }
    
    const std::vector<TileColor>& shape() const { return shape_; }
    const std::vector<TileColor>& initialShape() const { return initialShape_; }
    
    void rotate(Rotation rotation);
    const std::vector<std::pair<int, int>> kicks(Rotation rotation) const;
    
private:
    static const int kNumStates_;
    static const std::vector<std::vector<std::pair<int, int>>> kKicksIRight_, kKicksILeft_;
    static const std::vector<std::vector<std::pair<int, int>>> kKicksOtherRight_, kicksOtherLeft_;
    
    PieceKind kind_;
    TileColor color_;
    
    int nRows_, nCols_;
    std::vector<TileColor> initialShape_;
    
    int bBoxSide_;
    std::vector<TileColor> shape_;
    
    int state_;
    std::vector<std::vector<std::pair<int, int>>> kicksRight_;
    std::vector<std::vector<std::pair<int, int>>> kicksLeft_;
};


class Board {
public:
    const int nRows, nCols;
    
    Board(int nRows, int nCols);
    
    void clear();
    
    TileColor tileAt(int row, int col) const { return tiles_[(row + kRowsAbove_) * nCols + col]; };
    
    bool frozePiece();
    bool spawnPiece(PieceKind kind);
    
    bool moveHorizontal(int dCol);
    bool moveVertical(int dRow);
    bool rotate(Rotation rotation);
    int hardDrop();
    
    bool isOnGround() const;
    
    int numLinesToClear() const { return linesToClear_.size(); };
    void clearLines();
    
    const std::vector<int>& linesToClear() const { return linesToClear_; }
    Piece piece() const { return piece_; }
    int pieceRow() const { return row_; }
    int pieceCol() const { return col_; }
    int ghostRow() const { return ghostRow_; }
    
private:
    static const int kRowsAbove_;
    
    std::vector<TileColor> tiles_;
    
    Piece piece_;
    int row_, col_;
    int ghostRow_;
    
    std::vector<TileColor> tilesAfterClear_;
    std::vector<int> linesToClear_;
    
    void setTile(int row, int col, TileColor color);
    bool isTileFilled(int row, int col) const;
    bool isPositionPossible(int row, int col, const Piece &piece) const;
    void updateGhostRow();
    void findLinesToClear();
};



class Tetris {
public:
    Tetris(Board& board, double timeStep, unsigned int randomSeed);
    
    void restart(int level);
    bool isGameOver() const { return gameOver_; }
    
    void update(bool softDrop, bool moveRight, bool moveLeft);
    void rotate(Rotation rotation);
    void hardDrop();
    void hold();
    
    double lockPercent() const { return lockingTimer_ / kLockDownTimeLimit_; }
    bool isPausedForLinesClear() const { return pausedForLinesClear_; }
    double linesClearPausePercent() const { return linesClearTimer_ / kPauseAfterLineClear_; }
    
    int level() const { return level_; }
    int linesCleared() const { return linesCleared_; }
    int score() const { return score_; }
    Piece nextPiece() const { return Piece(bag_[nextPiece_]); }
    Piece heldPiece() const { return Piece(heldPiece_); }
    
private:
    static const int kLinesToClearPerLevel_;
    static const int kMaxLevel_;
    static const double kMoveDelay_;
    static const double kMoveRepeatDelay_;
    static const double kSoftDropSpeedFactor_;
    static const double kLockDownTimeLimit_;
    static const int kLockDownMovesLimit_;
    static const double kPauseAfterLineClear_;
    
    Board& board_;
    
    bool gameOver_;
    
    double timeStep_;
    
    std::default_random_engine rng_;
    std::vector<PieceKind> bag_;
    int nextPiece_;
    
    PieceKind heldPiece_;
    bool canHold_;
    
    int level_;
    int linesCleared_;
    int score_;
    
    double secondsPerLine_;
    double moveDownTimer_;
    
    Motion motion_;
    bool  moveLeftPrev_, moveRightPrev_;
    double moveRepeatDelayTimer_;
    double moveRepeatTimer_;
    
    bool isOnGround_;
    double lockingTimer_;
    int nMovesWhileLocking_;
    
    bool pausedForLinesClear_;
    double linesClearTimer_;
    
    void moveHorizontal(int dCol);
    void checkLock();
    void lock();
    void spawnPiece();
    void updateScore(int linesCleared);
    
    double secondsPerLineForLevel(int level) const { return std::pow(0.8 - (level - 1) * 0.007, level - 1); }
};

#endif //TETRIS_TETRIS_H

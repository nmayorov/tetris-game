#include "tetris.h"


const int Piece::kNumStates_ = 4;


const std::vector<std::vector<std::pair<int, int>>> Piece::kKicksIRight_ = {
    { {0, 0}, {0, -2}, {0, 1}, {1, -2}, {-2, 1} },
    { {0, 0}, {0, -1}, {0, 2}, {-2, -1}, {1, 2} },
    { {0, 0}, {0, 2}, {0, -1}, {-1, 2}, {2, -1} },
    { {0, 0}, {0, 1}, {0, -2}, {2, 1}, {-1, -2} }
};


const std::vector<std::vector<std::pair<int, int>>> Piece::kKicksILeft_ = {
    { {0, 0}, {0, -1}, {0, 2}, {-2, -1}, {1, 2} },
    { {0, 0}, {0, 2}, {0, -1}, {-1, 2}, {2, -1} },
    { {0, 0}, {0, 1}, {0, -2}, {2, 1}, {-1, -2} },
    { {0, 0}, {0, -2}, {0, 1}, {1, -2}, {-2, 1} }
};


const std::vector<std::vector<std::pair<int, int>>> Piece::kKicksOtherRight_ = {
    { {0, 0}, {0, 1}, {-1, -1}, {2, 0}, {2, -1} },
    { {0, 0}, {0, 1}, {1, 1}, {-2, 0}, {-2, 1} },
    { {0, 0}, {0, 1}, {-1, 1}, {2, 0}, {2, 1} },
    { {0, 0}, {0, -1}, {1, -1}, {-2, 0}, {-2, -1} }
};


const std::vector<std::vector<std::pair<int, int>>> Piece::kicksOtherLeft_ = {
    { {0, 0}, {0, 1}, {-1, 1}, {2, 0}, {2, 1} },
    { {0, 0}, {0, -1}, {1, 1}, {-2, 0}, {-2, 1} },
    { {0, 0}, {0, -1}, {-1, -1}, {2, 0}, {2, -1} },
    { {0, 0}, {0, -1}, {1, -1}, {-2, 0}, {-2, -1} }
};


Piece::Piece(PieceKind kind) : kind_(kind), color_(static_cast<TileColor>(kind)), state_(0) {
    TileColor e = kEmpty;
    TileColor c = color_;
    switch (kind) {
        case kNone:
            nRows_ = 0;
            nCols_ = 0;
            bBoxSide_ = 0;
            break;
        case kPieceI:
            nRows_ = 1;
            nCols_ = 4;
            bBoxSide_ = 4;
            initialShape_ = {c, c, c, c};
            shape_ = {e, e, e, e,
                      c, c, c, c,
                      e, e, e, e,
                      e, e, e, e};
            break;
        case kPieceJ:
            nRows_ = 2;
            nCols_ = 3;
            bBoxSide_ = 3;
            initialShape_ = {c, e, e,
                             c, c, c};
            shape_ = {c, e, e,
                      c, c, c,
                      e, e, e};
            break;
        case kPieceL:
            nRows_ = 2;
            nCols_ = 3;
            bBoxSide_ = 3;
            initialShape_ = {e, e, c,
                             c, c, c};
            shape_ = {e, e, c,
                      c, c, c,
                      e, e, e};
            break;
        case kPieceO:
            nRows_= 2;
            nCols_ = 2;
            bBoxSide_ = 2;
            initialShape_ = {c, c,
                             c, c};
            shape_ = initialShape_;
            break;
        case kPieceS:
            nRows_ = 2;
            nCols_ = 3;
            bBoxSide_ = 3;
            initialShape_ = {e, c, c,
                             c, c, e};
            shape_ = {e, c, c,
                      c, c, e,
                      e, e, e};
            break;
        case kPieceT:
            nRows_ = 2;
            nCols_ = 3;
            bBoxSide_ = 3;
            initialShape_ = {e, c, e,
                             c, c, c};
            shape_ = {e, c, e,
                      c, c, c,
                      e, e, e};
            break;
        case kPieceZ:
            nRows_ = 2;
            nCols_ = 3;
            bBoxSide_ = 3;
            initialShape_ = {c, c, e,
                             e, c, c};
            shape_ = {c, c, e,
                      e, c, c,
                      e, e, e};
            break;
    }
    
    if (kind == kPieceO || kind == kNone)
        return;
    
    if (kind == kPieceI) {
        kicksRight_ = kKicksIRight_;
        kicksLeft_ = kKicksILeft_;
    } else {
        kicksRight_ = kKicksOtherRight_;
        kicksLeft_ = kicksOtherLeft_;
    }
}


void Piece::rotate(Rotation rotation) {
    if (kind_ == kPieceO)
        return;
    
    std::vector<TileColor> newShape(shape_.size());
    int index = 0;
    switch (rotation) {
        case Rotation::kRight:
            state_ += 1;
            for (int col = bBoxSide_ - 1; col >= 0; --col) {
                for (int row = 0; row < bBoxSide_; ++row) {
                    newShape[row * bBoxSide_ + col] = shape_[index];
                    ++index;
                }
            }
            break;
        case Rotation::kLeft:
            state_ -= 1;
            for (int col = 0; col < bBoxSide_; ++col) {
                for (int i = bBoxSide_ - 1; i >= 0; --i) {
                    newShape[i * bBoxSide_ + col] = shape_[index];
                    ++index;
                }
            }
    }
    shape_ = newShape;
    
    if (state_ == -1)
        state_ = kNumStates_ - 1;
    else if (state_ == kNumStates_)
        state_ = 0;
}


const std::vector<std::pair<int, int>> Piece::kicks(Rotation rotation) const {
    switch (rotation) {
        case Rotation::kRight:
            return kicksRight_[state_];
        case Rotation::kLeft:
            return kicksLeft_[state_];
    }
}


const int Board::kRowsAbove_ = 2;


Board::Board(int nRows, int nCols) : nRows(nRows), nCols(nCols),
                                     tiles_((nRows + kRowsAbove_) * nCols, kEmpty), piece_(kNone) {}


void Board::clear() {
    std::fill(tiles_.begin(), tiles_.end(), kEmpty);
}


bool Board::frozePiece() {
    auto shape = piece_.shape();
    bool belowSkyline = false;
    int index = 0;
    for (int row = row_; row < row_ + piece_.bBoxSide(); ++row) {
        for (int col = col_; col < col_ + piece_.bBoxSide(); ++col) {
            if (shape[index] != kEmpty) {
                if (row >= 0)
                    belowSkyline = true;
                
                setTile(row, col, shape[index]);
            }
            ++index;
        }
    }
    findLinesToClear();
    piece_ = Piece(kNone);
    return belowSkyline;
}


bool Board::spawnPiece(PieceKind kind) {
    piece_ = Piece(kind);
    row_ = -2;
    col_ = (nCols - piece_.bBoxSide()) / 2;
    
    if (!isPositionPossible(row_, col_, piece_))
        return false;
    
    int maxMoveDown = kind == kPieceI ? 1 : 2;
    for (int moveDown = 0; moveDown < maxMoveDown; ++moveDown) {
        if (!isPositionPossible(row_ + 1, col_, piece_))
            break;
        ++row_;
    }
    updateGhostRow();
    return true;
}


bool Board::moveHorizontal(int dCol) {
    if (isPositionPossible(row_, col_ + dCol, piece_)) {
        col_ += dCol;
        updateGhostRow();
        return true;
    }
    
    return false;
}


bool Board::moveVertical(int dRow) {
    if (isPositionPossible(row_ + dRow, col_, piece_)) {
        row_ += dRow;
        return true;
    }
    
    return false;
}


bool Board::rotate(Rotation rotation) {
    if (piece_.kind() == kPieceO || piece_.kind() == kNone)
        return false;
    
    Piece testPiece(piece_);
    testPiece.rotate(rotation);
    
    for (const auto kick : piece_.kicks(rotation)) {
        int dRow = kick.first;
        int dCol = kick.second;
        if (isPositionPossible(row_ + dRow, col_ + dCol, testPiece)) {
            piece_ = testPiece;
            row_ += dRow;
            col_ += dCol;
            updateGhostRow();
            return true;
        }
            
    }
    
    return false;
}


int Board::hardDrop() {
    int rowsPassed = ghostRow_ - row_;
    row_ = ghostRow_;
    return rowsPassed;
}


bool Board::isOnGround() const {
    return !isPositionPossible(row_ + 1, col_, piece_);
}


void Board::clearLines() {
    if (linesToClear_.empty())
        return;
    
    linesToClear_.clear();
    tiles_ = tilesAfterClear_;
}


void Board::setTile(int row, int col, TileColor color) {
    tiles_[(row + kRowsAbove_) * nCols + col] = color;
}


bool Board::isTileFilled(int row, int col) const {
    if (col < 0 || col >= nCols || row < -kRowsAbove_ || row >= nRows)
        return true;
    
    return tileAt(row, col) != kEmpty;
}
        

bool Board::isPositionPossible(int row, int col, const Piece &piece) const {
    if (piece.kind() == kNone)
        return false;
    
    auto shape = piece.shape();
    int index = 0;
    for (int pieceRow = 0; pieceRow < piece.bBoxSide(); ++pieceRow) {
        for (int pieceCol = 0; pieceCol < piece.bBoxSide(); ++pieceCol) {
            if (shape[index] != kEmpty && isTileFilled(row + pieceRow, col + pieceCol))
                return false;
            
            ++index;
        }
    }
    
    return true;
}


void Board::updateGhostRow() {
    ghostRow_ = row_;
    while (isPositionPossible(ghostRow_ + 1, col_, piece_))
        ++ghostRow_;
}


void Board::findLinesToClear() {
    linesToClear_.clear();
    tilesAfterClear_ = tiles_;
    
    int linesCleared = 0;
    int index = tiles_.size() - 1;
    for (int row = nRows - 1; row >= -kRowsAbove_; --row) {
        bool fullRow = true;
        for (int col = 0; col < nCols; ++col) {
            if (!isTileFilled(row, col)) {
                fullRow = false;
                break;
            }
        }
        
        if (fullRow) {
            linesToClear_.push_back(row);
            linesCleared++;
            index -= nCols;
        } else if (linesCleared > 0) {
            int indexShift = linesCleared * nCols;
            for (int col = 0; col < nCols; ++col) {
                tilesAfterClear_[index + indexShift] = tiles_[index];
                --index;
            }
        } else {
            index -= nCols;
        }
    }
    
    std::fill(tilesAfterClear_.begin(), tilesAfterClear_.begin() + linesCleared * nCols, kEmpty);
}


const int Tetris::kLinesToClearPerLevel_ = 10;
const int Tetris::kMaxLevel_ = 15;
const double Tetris::kMoveDelay_ = 0.05;
const double Tetris::kMoveRepeatDelay_ = 0.15;
const double Tetris::kSoftDropSpeedFactor_ = 20;
const double Tetris::kLockDownTimeLimit_ = 0.4;
const int Tetris::kLockDownMovesLimit_ = 15;
const double Tetris::kPauseAfterLineClear_ = 0.3;


Tetris::Tetris(Board &board, double timeStep, unsigned int randomSeed) :
        board_(board), timeStep_(timeStep), rng_(randomSeed), bag_(2 * kNumPieces), nextPiece_(0), heldPiece_(kNone) {
    bag_[0] = kPieceI;
    bag_[1] = kPieceJ;
    bag_[2] = kPieceL;
    bag_[3] = kPieceO;
    bag_[4] = kPieceS;
    bag_[5] = kPieceT;
    bag_[6] = kPieceZ;
    std::copy(bag_.begin(), bag_.begin() + kNumPieces, bag_.begin() + kNumPieces);
    restart(1);
}


void Tetris::restart(int level) {
    board_.clear();
    gameOver_ = false;
    level_ = level;
    secondsPerLine_ = secondsPerLineForLevel(level);
    linesCleared_ = 0;
    score_ = 0;
    canHold_ = true;
    motion_ = Motion::kNone;
    moveLeftPrev_ = false;
    moveRightPrev_ = false;
    moveDownTimer_ = 0;
    moveRepeatTimer_ = 0;
    moveRepeatDelayTimer_ = 0;
    
    isOnGround_ = false;
    lockingTimer_ = 0;
    pausedForLinesClear_ = false;
    linesClearTimer_ = 0;
    
    std::shuffle(bag_.begin(), bag_.begin() + kNumPieces, rng_);
    std::shuffle(bag_.begin() + kNumPieces, bag_.end(), rng_);
    
    std::uniform_int_distribution<int> holdPieceSelector(0, kNumPieces - 1);
    heldPiece_ = bag_[holdPieceSelector(rng_)];
    
    spawnPiece();
}


void Tetris::update(bool softDrop, bool moveRight, bool moveLeft) {
    if (pausedForLinesClear_) {
        linesClearTimer_ += timeStep_;
        
        if (linesClearTimer_ < kPauseAfterLineClear_)
            return;
    
        updateScore(board_.numLinesToClear());
        board_.clearLines();
        spawnPiece();
        pausedForLinesClear_ = false;
    }
    
    moveDownTimer_ += timeStep_;
    moveRepeatTimer_ += timeStep_;
    moveRepeatDelayTimer_ += timeStep_;
    
    if (isOnGround_)
        lockingTimer_ += timeStep_;
    else
        lockingTimer_ = 0;
    
    bool moveLeftInput = moveLeft;
    bool moveRightInput = moveRight;
    
    if (moveLeft && moveRight) {
        if (!moveRightPrev_)
            moveLeft = false;
        else if (!moveLeftPrev_)
            moveRight = false;
        else if (motion_ == Motion::kLeft)
            moveRight = false;
        else
            moveLeft = false;
    }
    
    if (moveRight) {
        if (motion_ != Motion::kRight) {
            moveRepeatDelayTimer_ = 0;
            moveRepeatTimer_ = 0;
            moveHorizontal(1);
        } else if (moveRepeatDelayTimer_ >= kMoveRepeatDelay_ && moveRepeatTimer_ >= kMoveDelay_) {
            moveRepeatTimer_ = 0;
            moveHorizontal(1);
        }
        motion_ = Motion::kRight;
    } else if (moveLeft) {
        if (motion_ != Motion::kLeft) {
            moveRepeatDelayTimer_ = 0;
            moveRepeatTimer_ = 0;
            moveHorizontal(-1);
        } else if (moveRepeatDelayTimer_ >= kMoveRepeatDelay_ && moveRepeatTimer_ >= kMoveDelay_) {
            moveRepeatTimer_ = 0;
            moveHorizontal(-1);
        }
        motion_ = Motion::kLeft;
    } else {
        motion_ = Motion::kNone;
    }
    
    moveLeftPrev_ = moveLeftInput;
    moveRightPrev_ = moveRightInput;
    
    double speedFactor_ = softDrop ? kSoftDropSpeedFactor_ : 1;
    if (moveDownTimer_ >= secondsPerLine_ / speedFactor_) {
        if (board_.moveVertical(1) && softDrop)
            score_ += level_;
        moveDownTimer_ = 0;
    }
    
    checkLock();
}


void Tetris::moveHorizontal(int dCol) {
    if (board_.moveHorizontal(dCol) && isOnGround_) {
        lockingTimer_ = 0;
        nMovesWhileLocking_ += 1;
    }
}


void Tetris::rotate(Rotation rotation) {
    if (board_.rotate(rotation) && isOnGround_) {
        lockingTimer_ = 0;
        nMovesWhileLocking_ += 1;
    }
    
    checkLock();
}


void Tetris::hardDrop() {
    if (board_.piece().kind() == kNone)
        return;
    score_ += 2 * level_ * board_.hardDrop();
    lock();
}


void Tetris::hold() {
    if (!canHold_ || pausedForLinesClear_)
        return;
    
    PieceKind currentPiece = board_.piece().kind();
    board_.spawnPiece(heldPiece_);
    heldPiece_ = currentPiece;
    
    canHold_ = false;
}


void Tetris::checkLock() {
    if (!board_.isOnGround()) {
        isOnGround_ = false;
        return;
    }
    
    isOnGround_ = true;
    
    if (lockingTimer_ >= kLockDownTimeLimit_ || nMovesWhileLocking_ >= kLockDownMovesLimit_)
        lock();
}


void Tetris::lock() {
    lockingTimer_ = 0;
    isOnGround_ = false;
    canHold_ = true;
    
    if (!board_.frozePiece()) {
        gameOver_ = true;
        return;
    }
    
    if (board_.numLinesToClear() == 0) {
        spawnPiece();
        return;
    }
    
    pausedForLinesClear_ = true;
    linesClearTimer_ = 0;
}


void Tetris::spawnPiece() {
    gameOver_ = !board_.spawnPiece(bag_[nextPiece_]);
    ++nextPiece_;
    if (nextPiece_ == kNumPieces) {
        std::copy(bag_.begin() + kNumPieces, bag_.end(), bag_.begin());
        std::shuffle(bag_.begin() + kNumPieces, bag_.end(), rng_);
        nextPiece_ = 0;
    }
    nMovesWhileLocking_ = 0;
}



void Tetris::updateScore(int linesCleared) {
    int deltaScore = 0;
    switch (linesCleared) {
        case 1:
            deltaScore = 100;
            break;
        case 2:
            deltaScore = 300;
            break;
        case 3:
            deltaScore = 400;
            break;
        case 4:
            deltaScore = 800;
            break;
        default:
            assert(false);
    }
    linesCleared_ += linesCleared;
    score_ += deltaScore * level_;
    if (level_ < kMaxLevel_ && linesCleared_ >= kLinesToClearPerLevel_ * level_) {
        ++level_;
        secondsPerLine_ = secondsPerLineForLevel(level_);
    }
}

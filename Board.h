// Created by Taylor Schmitt on 11/19/24.
#pragma once

#include "Tile.h"
using namespace std;
#include <SFML/Graphics/RenderWindow.hpp>

class Board {
    int rowNum;
    int colNum;
    int mineNum;
    bool debugMode = false;
    bool isPaused = false;
    bool gameRunning = true;
    bool gameOver = false;
    bool mineClicked = false;
    bool allNonMinesRevealed = true;
    bool didPlayerWin = false;
    sf::Sprite& HappyFaceButton = spriteManager.getSprite("Happy Face Button");
    sf::Sprite& debugButton = spriteManager.getSprite("Debug");
    vector<std::vector<Tile*> > board;
    SpriteManager spriteManager;

public:
    Board(){};

    Board(int rowNum, int colNum, int mineNum) {
        this->rowNum = rowNum;
        this->colNum = colNum;
        this->mineNum = mineNum;
    }


    void initializeBoard() {
        board.resize(rowNum, vector<Tile*>(colNum, nullptr));
        for (int row = 0; row < rowNum; row++) {
            for (int col = 0; col < colNum; col++) {
                board[row][col] = new Tile();
            }
        }

        mineAssignment();

        for (int row = 0; row < rowNum; ++row) {
            for (int col = 0; col < colNum; ++col) {
                Tile* tile = board[row][col];
                vector<Tile*> neighbors = getAdjacentTiles(tile, row, col);
                for (Tile* neighbor : neighbors) {
                    tile->addAdjacentTile(neighbor);
                }
            }
        }

        for (int row = 0; row < rowNum; ++row) {
            for (int col = 0; col < colNum; ++col) {
                board[row][col]->calculateAdjacentMines();
            }
        }
    }

    Tile* getTile(int row, int col) {
        if (row >= 0 && row < rowNum && col >= 0 && col < colNum) {
            return board[row][col];
        }
        return nullptr;
    }


    void drawBoard(sf::RenderWindow &window) {
        for (int row = 0; row < rowNum; row++) {
            for (int col = 0; col < colNum; col++) {
                Tile* tile = getTile(row, col);

                if (tile != nullptr) {
                    tile->drawTile(window, col * 32, row * 32);
                }
            }
        }
    }


    void drawPause(sf::RenderWindow &window) {
        sf::Sprite pauseButton;
        if (isPaused == true) {
            pauseButton = spriteManager.getSprite("Play");
        } else {
            pauseButton = spriteManager.getSprite("Pause");
        }
        pauseButton.setPosition(colNum * 32 - 240, 32 * (rowNum + 0.5));
        window.draw(pauseButton);
    }


    void drawDebug(sf::RenderWindow &window) {
        debugButton.setPosition(colNum * 32 - 304, 32 * (rowNum + 0.5));
        window.draw(debugButton);
    }



    void drawLeaderButton(sf::RenderWindow &window) {
        sf::Sprite& leaderButton = spriteManager.getSprite("Leader Board Button");
        leaderButton.setPosition(colNum * 32 - 176, 32 * (rowNum + 0.5));
        window.draw(leaderButton);
    }


    void drawHappyFace(sf::RenderWindow &window, bool gameRunning) {
        if (gameRunning == true) {
            HappyFaceButton = spriteManager.getSprite("Happy Face Button");
        }
        HappyFaceButton.setPosition(sf::Vector2f((colNum * 32) / 2 - 32, (rowNum + 0.5) * 32));
        window.draw(HappyFaceButton);
    }


    void drawCoolFace(sf::RenderWindow &window) {
        HappyFaceButton = spriteManager.getSprite("Cool Face Button");
        HappyFaceButton.setPosition(((colNum/2) * 32) - 32, 32 * (rowNum + 0.5));
        window.draw(HappyFaceButton);
    }


    void drawSadFace(sf::RenderWindow &window) {
        HappyFaceButton = spriteManager.getSprite("Sad Face Button");
        HappyFaceButton.setPosition(((colNum/2) * 32) - 32, 32 * (rowNum + 0.5));
        window.draw(HappyFaceButton);
    }


    void revealAdjacentTile(Tile* tile) {
        if (tile == nullptr || tile->isRevealed() || tile->hasAFlag()) {
            return;
        }
        tile->revealTile();
        if (tile->getAdjacentMines() > 0) {
            return;
        }
        for (Tile* neighbor : tile->getAdjacentTiles()) {
            revealAdjacentTile(neighbor);
        }
    }


    void handleTileClick(int row, int col) {
        Tile* clickedTile = getTile(row, col);
        if (clickedTile == nullptr || clickedTile->hasAFlag()) {
            return;
        }
        clickedTile->revealTile();

        if (clickedTile->getAdjacentMines() == 0 && clickedTile->isAMine() == false) {
            for (Tile* neighbor : clickedTile->getAdjacentTiles()) {
                if (neighbor != nullptr) {
                    if (!neighbor->isRevealed() && !neighbor->hasAFlag()) {
                        revealAdjacentTile(neighbor);
                    }
                }
            }
        }
    }


    vector<Tile*> getAdjacentTiles(Tile* tile, int row, int col) {
        vector<Tile*> adjacentTiles;

        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                //skip the current tile itself
                if (i == 0 && j == 0) continue;
                int adjRow = row + i;
                int adjCol = col + j;

                if (adjRow >= 0 && adjRow < rowNum & adjCol >= 0 && adjCol < colNum) {
                    adjacentTiles.push_back(getTile(adjRow, adjCol));
                }
            }
        }
        return adjacentTiles;
    }


    int getRowNum() {
        return rowNum;
    }


    int getColNum() {
        return colNum;
    }

    int getMineNum() {
        return mineNum;
    }


    void mineAssignment();


    void toggleDebugMode() {
        debugMode = !debugMode;
        if (debugMode) {
            revealAllTiles();
        } else {
            hideNonRevealedTiles();
        }
    }


    void revealAllTiles() {
        for (int row = 0; row < rowNum; ++row) {
            for (int col = 0; col < colNum; ++col) {
                Tile* tile = getTile(row, col);
                if (tile != nullptr) {
                    tile->revealTile();
                }
            }
        }
    }


    void hideNonRevealedTiles() {
        for(int row = 0; row < rowNum; ++row) {
            for(int col = 0; col < colNum; ++col) {
                Tile* tile = getTile(row, col);
                if (tile != nullptr && tile->isRevealed()) {
                    tile->resetTile();
                }
            }
        }
    }

    void revealTile(int row, int column) {
        Tile* tile = getTile(row, column);

        if (tile != nullptr || tile->isRevealed() || tile->hasAFlag()) {
            return;
        }

        tile->revealTile();
        if (tile->getAdjacentMines() > 0) {
            return;
        }
        //if no adjacent mines, keep revealing adjacent tiles
        for (Tile* neighbor : tile->getAdjacentTiles()) {
            if (neighbor != nullptr && !neighbor->isRevealed() && !neighbor->isAMine()) {
                revealTile(neighbor->getRow(), neighbor->getColumn());
            }
        }
    }


    void togglePause() {
        isPaused = !isPaused;
        if (isPaused == true) {
            applyPaused();
        } else {
            restoreBoard();
            isPaused = false;
        }
    }


    void pauseForWinOrLose() {
        isPaused = true;
    }


    void applyPaused() {
        for (int row = 0; row < rowNum; row++) {
            for (int col = 0; col < colNum; col++) {
                Tile* tile = getTile(row, col);
                if (tile != nullptr) {
                    tile->temporaryRevealed();
                }
            }
        }
    }

    void restoreBoard() {
        for (int row = 0; row < rowNum; row++) {
            for (int col = 0; col < colNum; col++) {
                Tile* tile = getTile(row, col);
                if (tile != nullptr) {
                    tile->restoreTileState();
                }
            }
        }
    }


    bool winCheck() {
        cout << "winCheck" << endl;

        for (int row = 0; row < rowNum; ++row) {
            for (int col = 0; col < colNum; ++col) {
                Tile* tile = getTile(row, col);
                if (tile == nullptr) {
                    continue;
                }

                if (tile->isAMine() && tile->isRevealed()) {
                    mineClicked = true;
                }

                if (!tile->isAMine() && !tile->isRevealed()) {
                    allNonMinesRevealed = false;
                }
            }
        }

        if (mineClicked == true) {
            mineClicked = false;
            cout << "MINECOUNT: " << mineClicked << endl;
            endGame(false);//player loses
            return false;
        }
        if (allNonMinesRevealed) {
            endGame(true);//player wins
            return false;
        }
        return true;
    }

    bool checkPlayerWin() {
        if(mineClicked == true) {
            didPlayerWin = false;
        } else if(allNonMinesRevealed == true) {
            didPlayerWin = true;
        }
        return didPlayerWin;
    }

    bool endGame(bool won) {
        pauseForWinOrLose();
        for (int row = 0; row < rowNum; ++row) {
            for (int col = 0; col < colNum; ++col) {
                Tile* tile = getTile(row, col);
                if (tile == nullptr) continue;

                if (won && tile->isAMine() && !tile->hasAFlag()) {
                    tile->setFlag();
                }

                if (!won && tile->isAMine() && !tile->isRevealed()) {
                    tile->revealTile();
                }
            }
        }
        if (won) {
            cout << "WON" << endl;
            HappyFaceButton = spriteManager.getSprite("Cool Face Button");
        } else {
            cout << "LOST" << endl;
            HappyFaceButton = spriteManager.getSprite("Sad Face Button");
        }

        gameRunning = false;
        return won;
    }


    bool isGameRunning() {
        return gameRunning;
    }


    void resetBoard() {
        gameRunning = true;
        isPaused = false;
        mineClicked = false;


        for (int row = 0; row < rowNum; row++) {
            for (int col = 0; col < colNum; col++) {
                Tile* tile = getTile(row, col);
                if (tile != nullptr) {
                    tile->setDebugRevealed(false);
                    tile->resetSpriteToHidden();
                }
            }
        }

        mineAssignment();

        for (int row = 0; row < rowNum; row++) {
            for (int col = 0; col < colNum; col++) {
                Tile* tile = getTile(row, col);
                if (tile != nullptr) {
                    tile->getAdjacentTiles().clear();

                    for (int i = -1; i <= 1; i++) {
                        for (int j = -1; j <= 1; j++) {
                            if (i == 0 && j == 0) {
                                continue;
                            }
                            int adjRow = row + i;
                            int adjCol = col + j;

                            if (adjRow >= 0 && adjRow < rowNum && adjCol >= 0 && adjCol < colNum) {
                                Tile* adj = getTile(adjRow, adjCol);
                                if (adj != nullptr) {
                                    tile->addAdjacentTile(adj);
                                }
                            }
                        }
                    }
                }
            }
        }

        for (int row = 0; row < rowNum; row++) {
            for (int col = 0; col < colNum; col++) {
                Tile* tile = getTile(row, col);
                if (tile != nullptr) {
                    tile->calculateAdjacentMines();
                }
            }
        }
    }
};
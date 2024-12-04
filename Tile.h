// Created by Taylor Schmitt on 11/19/24.

#pragma once
#include <vector>
#include "SpriteManager.h"
using namespace std;


class Tile {
    bool isMine = false;
    bool hasFlag = false;
    bool revealed = false;
    int row, column;
    int minesAdjacent = 0;
    bool debugRevealed = false;
    vector<Tile*> adjacentTiles;
    sf::Sprite sprite;
    SpriteManager spriteManager;
    sf::Sprite topLayer;

public:
    Tile() {
        sprite = SpriteManager::getSprite("HiddenTile");
    }

    int getRow() {
        return row;
    }

    int getColumn() {
        return column;
    }

    bool getDebugRevealed() {
        return debugRevealed;
    }

    void preloadSprites() {
        spriteManager.loadSprite("Cool Face Button", "files/images/face_win.png");
        spriteManager.loadSprite("Sad Face Button", "files/images/face_lose.png");
        spriteManager.loadSprite("Happy Face Button", "files/images/face_happy.png");
        spriteManager.loadSprite("Leader Board Button", "files/images/leaderboard.png");
        spriteManager.loadSprite("Debug", "files/images/debug.png");
        spriteManager.loadSprite("Pause", "files/images/pause.png");
        spriteManager.loadSprite("Play", "files/images/play.png");
        spriteManager.loadSprite("HiddenTile", "files/images/tile_hidden.png");
        spriteManager.loadSprite("RevealedTile", "files/images/tile_revealed.png");
        spriteManager.loadSprite("Flag", "files/images/flag.png");
        spriteManager.loadSprite("Mine", "files/images/mine.png");
        for (int i = 1; i <= 8; ++i) {
            spriteManager.loadSprite("Number" + to_string(i), "files/images/number_" + to_string(i) + ".png");
        }
    }

    bool isAMine() {
        return isMine;
    }
    bool hasAFlag() {
        return hasFlag;
    }
    bool isRevealved() {
        return revealed;
    }

    void setMine() {
        isMine = true;
        updateTile();
    }

    void setFlag() {
        if (!revealed) {
            if (hasFlag) {
                hasFlag = false;
            } else {
                hasFlag = true;
            }
            updateTile();
        }
    }

    void setDebugRevealed(bool revealed) {
        debugRevealed = revealed;
    }

    void resetSpriteToHidden() {
        sprite = spriteManager.getSprite("HiddenTile");
    }

    void revealTile() {
        if (!revealed && !hasFlag) {
            revealed = true;
            updateTile();
        }
    }

    bool isRevealed() const {
        return revealed;
    }

    void addAdjacentTile(Tile* neighbor) {
        adjacentTiles.push_back(neighbor);
    }

    vector<Tile*>& getAdjacentTiles() {
        return adjacentTiles;
    }

    void calculateAdjacentMines() {
        minesAdjacent = 0;
        for (Tile* neighbor : adjacentTiles) {
            if (neighbor->isAMine()) {
                minesAdjacent++;
            }
        }
    }

    int getAdjacentMines() {
        return minesAdjacent;
    }

    sf::Sprite getSprite() {
        return sprite;
    }

    void resetTile() {
        //isMine = false;
        hasFlag = false;
        revealed = false;
        //minesAdjacent = 0;
        //adjacentTiles.clear();
        updateTile();
        sprite = spriteManager.getSprite("HiddenTile");
        topLayer = sf::Sprite();
    }

    void drawTile(sf::RenderWindow &window, int x, int y) {
        sprite.setPosition(static_cast<float>(x), static_cast<float>(y));
        window.draw(sprite);
        if(topLayer.getTexture()) {
            topLayer.setPosition(static_cast<float>(x), static_cast<float>(y));
            window.draw(topLayer);
        }
    }

    void updateTile() {
        if(revealed) {
            sprite = spriteManager.getSprite("RevealedTile");
            if(isMine) {
                topLayer = spriteManager.getSprite("Mine");
            } else if (minesAdjacent > 0) {
                topLayer = spriteManager.getSprite("Number" + to_string(minesAdjacent));
            } else {
                topLayer = sf::Sprite();
            }
        } else if (hasFlag == true) {
            sprite = spriteManager.getSprite("HiddenTile");
            topLayer = spriteManager.getSprite("Flag");
        } else {
            sprite = spriteManager.getSprite("HiddenTile");
        }
    }

    void temporaryRevealed() { //for the pause button
        sprite = spriteManager.getSprite("RevealedTile");
    }

    void restoreTileState() { //for play button
        updateTile();
    }

    void revealForDebug() {
        if (!revealed) {
            debugRevealed = true;
            sprite = spriteManager.getSprite("RevealedTile");
            if(isMine) {
                topLayer = spriteManager.getSprite("Mine");
            } else if (minesAdjacent > 0) {
                topLayer = spriteManager.getSprite("Number" + to_string(minesAdjacent));
            } else {
                topLayer = sf::Sprite();
            }
        }
    }
};
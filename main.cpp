#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>
#include "Board.h"
#include "LeaderBoard.h"
#include <string>
using namespace std;


void setText(sf::Text &text, float x, float y) {
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width/2.0f,textRect.top + textRect.height/2.0f);
    text.setPosition(sf::Vector2f(x, y));
}

map<int, sf::Sprite> parseDigits(sf::Sprite digits) {
    map<int, sf::Sprite> digitsMap;
    for(int i = 0; i <= 10; i++) {
        sf::IntRect rectangle(i*21, 0, 21, 32);
        digits.setTextureRect(rectangle);
        sf::Sprite sprite = digits;
        digitsMap.emplace(i, sprite);
    }
    return digitsMap;
}

void Board::mineAssignment() {
    int placedMines = 0;
    while (placedMines < mineNum) {
        int row = rand() % rowNum;
        int col = rand() % colNum;
        Tile* tile = getTile(row, col);
        if (tile != nullptr && !tile->isAMine()) {
            tile->setMine();
            placedMines++;
        }
    }
}


int main() {
    //welcome window:
    string Rows, Columns, Mines;
    fstream file("files/config.cfg");
    getline(file, Columns);
    int columns = stoi(Columns);
    getline(file, Rows);
    int rows = stoi(Rows);
    getline(file, Mines);
    int mines = stoi(Mines);

    int width = (columns*32);
    int height = (rows*32)+100;

    //creating welcome window
    sf::RenderWindow window(sf::VideoMode(width, height), "Minesweeper");

    sf::Font font;
    if (!font.loadFromFile("files/font.ttf")) {
        return EXIT_FAILURE;
    }

    //welcome text
    sf::Text welcomeText("WELCOME TO MINESWEEPER!", font, 24);
    welcomeText.setStyle(sf::Text::Bold | sf::Text::Underlined);
    setText(welcomeText, width/2, (height/2) - 150);

    //enter your name text
    sf::Text nameInput("Enter your name:", font, 20);
    nameInput.setStyle(sf::Text::Bold);
    setText(nameInput, width/2, (height/2) - 75);

    //username
    string username;
    sf::Text usernameText("", font, 18);

    //cursor
    sf::Text cursor("|", font, 18);
    cursor.setStyle(sf::Text::Bold);
    cursor.setFillColor(sf::Color::Yellow);

    ///leaderboard display
    sf::Text leaderboardHeader("LEADERBOARD", font, 20);
    leaderboardHeader.setStyle(sf::Text::Bold | sf::Text::Underlined);
    setText(leaderboardHeader, (columns * 16)/2, (rows * 16+50)/2 - 120);
    bool isLeaderboardOpen = false;


    //welcome window:
    while(window.isOpen()) {
        sf::Event event;
        while(window.pollEvent(event)) {
            if(event.type == sf::Event::Closed) {
                window.close();
            }

            if(event.type == sf::Event::TextEntered) {
                char key;
                if(event.text.unicode < 128) {
                    key = static_cast<char>(event.text.unicode);
                    if(isalpha(key)) { //check for letter input
                        if (!username.empty()) {
                            username[0] = toupper(username[0]); //upper and lowercase fixes
                            for(int i = 1 ; i<username.size() ; i++) {
                                username[i] = tolower(username[i]);
                            }
                        }
                        if(username.size() <= 9) {
                            username += key;
                        }
                    }
                }

                usernameText.setString(username);
                usernameText.setStyle(sf::Text::Bold);
                usernameText.setFillColor(sf::Color::Yellow);
                setText(usernameText, width/2, height/2 - 45);
            }

            //delete character
            if(event.type == sf::Event::KeyPressed) {
                if(event.key.scancode == sf::Keyboard::Scan::Backspace && !username.empty()) {
                    username.pop_back();
                }
            }

            //enter the game
            if(event.type == sf::Event::KeyPressed) {
                if(event.key.scancode == sf::Keyboard::Scan::Enter && !username.empty()) {
                    window.close();
                }
            }

            //updating cursor position
            if (username.empty()) {
                setText(cursor, width/2, height/2 - 45);
            } else {
                setText(cursor, usernameText.getPosition().x + (usernameText.getGlobalBounds().width)/2 + 5,  usernameText.getPosition().y);
            }
        }

        window.clear(sf::Color::Blue);
        window.draw(welcomeText);
        window.draw(nameInput);
        window.draw(usernameText);
        window.draw(cursor);
        window.display();
    }



    //game window
    if(!window.isOpen()) {

        Tile Tiles;
        Tiles.preloadSprites();
        Board gameBoard(rows, columns, mines);
        gameBoard.initializeBoard();

        sf::RenderWindow gameWindow(sf::VideoMode(width, height), "Minesweeper");

        ////////timer
        auto startTime = chrono::high_resolution_clock::now();
        auto pauseTime = chrono::high_resolution_clock::now();
        auto elapsedPauseTime = chrono::duration_cast<chrono::seconds>(chrono::high_resolution_clock::now() - pauseTime).count();
        bool paused = false;

        SpriteManager digitSprite;
        digitSprite.loadSprite("Digits", "files/images/digits.png");
        sf::Sprite digits;
        digits = digitSprite.getSprite("Digits");

        map<int, sf::Sprite> digitsMap = parseDigits(digits);
        ///////end timer


        ////mine digit display
        int mineCount = mines;
        SpriteManager mineCountSprite;
        sf::Sprite mineCountDisplay;
        mineCountDisplay = mineCountSprite.getSprite("Digits");
        map<int, sf::Sprite> minesCountDisplayMap = parseDigits(mineCountDisplay);
        //////end mine digit display

        int count = mineCount;
        bool gameEnded = false;
        leaderboard leaderboard;

        while(gameWindow.isOpen()) {
            sf::Event event;
            while(gameWindow.pollEvent(event)) {

                if(event.type == sf::Event::Closed) {
                    gameWindow.close();
                }

                //revealing a tile and adjacent tiles if necessary
                if(event.type == sf::Event::MouseButtonPressed && gameBoard.isGameRunning() && event.mouseButton.button == sf::Mouse::Left) {
                    int mouseX = event.mouseButton.x;
                    int mouseY = event.mouseButton.y;
                    int posCol = mouseX/32;
                    int posRow = mouseY/32;

                    if (posRow >= 0 && posRow < gameBoard.getRowNum() && posCol >= 0 && posCol < gameBoard.getColNum()) {
                        Tile* clickedTile = gameBoard.getTile(posRow, posCol);
                        if (clickedTile != nullptr) {
                            clickedTile->revealTile();
                            gameBoard.handleTileClick(posRow, posCol);

                            bool gameStillRunning = gameBoard.winCheck();

                            if (!gameStillRunning) {
                                if (!gameBoard.winCheck()) {
                                    gameEnded = true;
                                }
                            }
                        }
                    }
                }

                //debug button
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    int mouseX = event.mouseButton.x;
                    int mouseY = event.mouseButton.y;
                    int debugButtonX = gameBoard.getColNum() * 32 - 304;
                    int debugButtonY = (gameBoard.getRowNum() + 0.5) * 32;
                    int debugButtonWidth = 64;
                    int debugButtonHeight = 64;

                    if (mouseX >= debugButtonX && mouseX <= debugButtonX + debugButtonWidth && mouseY >= debugButtonY && mouseY <= debugButtonY + debugButtonHeight) {
                        gameBoard.toggleDebugMode();
                    }
                }

                //adding a flag
                if (event.type == sf::Event::MouseButtonPressed && gameBoard.isGameRunning() && event.mouseButton.button == sf::Mouse::Right) {
                    int mouseX = event.mouseButton.x;
                    int mouseY = event.mouseButton.y;
                    int posCol = mouseX/32;
                    int posRow = mouseY/32;

                    if (posRow >= 0 && posRow < gameBoard.getRowNum() && posCol >= 0 && posCol < gameBoard.getColNum()) {
                        Tile* clickedTile = gameBoard.getTile(posRow, posCol);
                        if (clickedTile != nullptr) {
                            clickedTile->setFlag();
                            if (clickedTile->hasAFlag()) {
                                mineCount--;
                            } else {
                                mineCount++;
                            }
                        }
                    }
                }

                //pause button
                if(event.type == sf::Event::MouseButtonPressed && gameBoard.isGameRunning() && event.mouseButton.button == sf::Mouse::Left) {
                    int X = event.mouseButton.x;
                    int Y = event.mouseButton.y;
                    int pauseButtonX = (columns * 32) - 240;
                    int pauseButtonY = (rows + 0.5) * 32;
                    int pauseButtonWidth = 64;
                    int pauseButtonHeight = 64;

                    if (X >= pauseButtonX && X <= pauseButtonX + pauseButtonWidth && Y >= pauseButtonY && Y <= pauseButtonY + pauseButtonHeight){
                        paused = !paused;
                        gameBoard.togglePause();
                        if(paused) {
                            cout << "paused" << endl;
                            pauseTime = chrono::high_resolution_clock::now();
                        } else {
                            cout << "unpaused" << endl;
                            auto unpausedTime = chrono::steady_clock::now();
                            elapsedPauseTime += chrono::duration_cast<chrono::seconds>(unpausedTime - pauseTime).count();
                        }
                    }
                }

                //reset button
                if(event.type == sf::Event::MouseButtonPressed && (gameBoard.winCheck() || !gameBoard.winCheck()) && event.mouseButton.button == sf::Mouse::Left) {
                    int mousePosX = event.mouseButton.x;
                    int mousePosY = event.mouseButton.y;
                    int resetButtonX = ((columns/2.0) * 32) - 32;
                    int resetButtonY = (rows + 0.5f) * 32;
                    int resetButtonWidth = 64;
                    int resetButtonHeight = 64;

                    if (mousePosX >= resetButtonX && mousePosX <= resetButtonX + resetButtonWidth && mousePosY >= resetButtonY && mousePosY <= resetButtonY + resetButtonHeight)  {
                        //cout << "Game Reset" << endl;
                        // gameBoard.drawHappyFace(gameWindow, true);
                        gameBoard.resetBoard(gameWindow);
                        mineCount = mines;
                        startTime = chrono::high_resolution_clock::now();
                        elapsedPauseTime = 0;
                        paused = false;
                        gameEnded = false;
                    }
                }


                //leaderboard button
                if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    int mousePosX = event.mouseButton.x;
                    int mousePosY = event.mouseButton.y;
                    int LeaderBoardButtonX = (columns * 32) - 176;
                    int LeaderBoardButtonY = (rows + 0.5f) * 32;
                    int LeaderBoardButtonWidth = 64;
                    int LeaderBoardButtonHeight = 64;

                    if(mousePosX >= LeaderBoardButtonX && mousePosX <= LeaderBoardButtonX + LeaderBoardButtonWidth && mousePosY >= LeaderBoardButtonY && mousePosY <= LeaderBoardButtonY + LeaderBoardButtonHeight) {
                        isLeaderboardOpen = !isLeaderboardOpen;
                        sf::RenderWindow leaderboardWindow(sf::VideoMode(columns * 16, (rows * 16) +50), "Minesweeper");
                        if(isLeaderboardOpen) {
                            while(leaderboardWindow.isOpen()) {
                                sf::Event event;
                                while(leaderboardWindow.pollEvent(event)) {
                                    if(event.type == sf::Event::Closed) {
                                        leaderboardWindow.close();
                                    }
                                }
                                leaderboardWindow.clear(sf::Color::Blue);
                                leaderboardWindow.draw(leaderboardHeader);
                                leaderboard.displayLeaderboard(leaderboardWindow, font);
                            }
                        }
                    }
                }
            }
            //timer variables
            auto gameDuration = chrono::duration_cast<chrono::seconds>(chrono::high_resolution_clock::now() - startTime);
            int totalTime = gameDuration.count();

            int minutes, seconds;

            if(!paused && gameBoard.isGameRunning()) {
                totalTime -= elapsedPauseTime;
                minutes = totalTime / 60;
                seconds = totalTime % 60;
            }

            int minute0 = minutes / 10 % 10;
            int minute1 = minutes % 10;
            int second0 = seconds / 10 % 10;
            int second1 = seconds % 10;

            //leaderboard
            if (gameEnded && !gameBoard.isGameRunning()) {
                gameEnded = false;
                mineCount = 0;
                gameWindow.clear(sf::Color::White);

                gameBoard.drawBoard(gameWindow);
                gameBoard.drawPause(gameWindow);
                gameBoard.drawDebug(gameWindow);
                gameBoard.drawLeaderButton(gameWindow);
                gameBoard.drawHappyFace(gameWindow, gameBoard.winCheck());

                digitsMap[minute0].setPosition((columns * 32) - 97, (530 + 0.5f) + 16);
                gameWindow.draw(digitsMap[minute0]);
                digitsMap[minute1].setPosition((columns * 32) - 76, (530 + 0.5f) + 16);
                gameWindow.draw(digitsMap[minute1]);
                digitsMap[second0].setPosition((columns * 32) - 54, (530 + 0.5f) + 16);
                gameWindow.draw(digitsMap[second0]);
                digitsMap[second1].setPosition((columns * 32) - 33, (530 + 0.5f) + 16);
                gameWindow.draw(digitsMap[second1]);
                if(count < 0) {
                    int county = count*-1;
                    int countHundreds = county / 100 % 10;
                    int countTens = county / 10 % 10;
                    int countSingles = county % 10;
                    int count0 = 10;
                    digitsMap[countHundreds].setPosition(33, 32*(rows+0.5) + 16);
                    gameWindow.draw(digitsMap[countHundreds]);
                    digitsMap[countTens].setPosition(54, 32*(rows + 0.5) + 16);
                    gameWindow.draw(digitsMap[countTens]);
                    digitsMap[countSingles].setPosition(75, 32*(rows + 0.5) + 16);
                    gameWindow.draw(digitsMap[countSingles]);
                    digitsMap[count0].setPosition(12, 32*(rows + 0.5) + 16);
                    gameWindow.draw(digitsMap[count0]);
                } else {
                    int hundreds, tens, singles;
                    hundreds = mineCount / 100 % 10;
                    tens = mineCount / 10 % 10;
                    singles = mineCount % 10;

                    digitsMap[hundreds].setPosition(33, 32*(rows+0.5) + 16);
                    gameWindow.draw(digitsMap[hundreds]);
                    digitsMap[tens].setPosition(54, 32*(rows + 0.5) + 16);
                    gameWindow.draw(digitsMap[tens]);
                    digitsMap[singles].setPosition(75, 32*(rows + 0.5) + 16);
                    gameWindow.draw(digitsMap[singles]);
                }

                gameWindow.display();


                auto endTime = chrono::high_resolution_clock::now();
                int playTime = chrono::duration_cast<chrono::seconds>(endTime - startTime).count();

                bool gameWon = gameBoard.checkPlayerWin();
                if (gameWon) {
                    leaderboard.addPlayer(username, playTime);

                    isLeaderboardOpen = true;
                    sf::RenderWindow leaderboardWindow(sf::VideoMode(columns * 16, (rows * 16) + 50), "Leaderboard");
                    while (leaderboardWindow.isOpen()) {
                        sf::Event event;
                        while (leaderboardWindow.pollEvent(event)) {
                            if (event.type == sf::Event::Closed) {
                                leaderboardWindow.close();
                                isLeaderboardOpen = false;
                            }
                        }
                        leaderboardWindow.clear(sf::Color::Blue);
                        leaderboardWindow.draw(leaderboardHeader);
                        leaderboard.displayLeaderboard(leaderboardWindow, font);
                    }
                }
            }



            gameWindow.clear(sf::Color::White);

            ////more timer
            digitsMap[minute0].setPosition((columns * 32) - 97, (530 + 0.5f) + 16);
            gameWindow.draw(digitsMap[minute0]);

            digitsMap[minute1].setPosition((columns * 32) - 76, (530 + 0.5f) + 16);
            gameWindow.draw(digitsMap[minute1]);

            digitsMap[second0].setPosition((columns * 32) - 54, (530 + 0.5f) + 16);
            gameWindow.draw(digitsMap[second0]);

            digitsMap[second1].setPosition((columns * 32) - 33, (530 + 0.5f) + 16);
            gameWindow.draw(digitsMap[second1]);
            ///// end more of timer


            ////more mine count display
            if(count < 0) {
                int county = count*-1;
                int countHundreds = county / 100 % 10;
                int countTens = county / 10 % 10;
                int countSingles = county % 10;
                int count0 = 10;
                digitsMap[countHundreds].setPosition(33, 32*(rows+0.5) + 16);
                gameWindow.draw(digitsMap[countHundreds]);
                digitsMap[countTens].setPosition(54, 32*(rows + 0.5) + 16);
                gameWindow.draw(digitsMap[countTens]);
                digitsMap[countSingles].setPosition(75, 32*(rows + 0.5) + 16);
                gameWindow.draw(digitsMap[countSingles]);
                digitsMap[count0].setPosition(12, 32*(rows + 0.5) + 16);
                gameWindow.draw(digitsMap[count0]);
            } else {
                int hundreds, tens, singles;
                hundreds = mineCount / 100 % 10;
                tens = mineCount / 10 % 10;
                singles = mineCount % 10;

                digitsMap[hundreds].setPosition(33, 32*(rows+0.5) + 16);
                gameWindow.draw(digitsMap[hundreds]);
                digitsMap[tens].setPosition(54, 32*(rows + 0.5) + 16);
                gameWindow.draw(digitsMap[tens]);
                digitsMap[singles].setPosition(75, 32*(rows + 0.5) + 16);
                gameWindow.draw(digitsMap[singles]);
            }
            ///end mine count display

            gameBoard.drawBoard(gameWindow);
            gameBoard.drawPause(gameWindow);
            gameBoard.drawDebug(gameWindow);
            gameBoard.drawLeaderButton(gameWindow);
            gameBoard.drawHappyFace(gameWindow, gameBoard.checkPlayerWin());
            gameWindow.display();
        }
        return 0;
    }

    return 0;
}
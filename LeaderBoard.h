// Created by Taylor Schmitt on 12/2/24.
#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <algorithm>
#include <iomanip>
using namespace std;

struct Player {
    string name;
    int playTime;
    int ranking;

    Player(string name, int playTime, int ranking) {
        this->name = name;
        this->playTime = playTime;
        this->ranking = ranking;
    }

    string getName() {
        return name;
    }

    int getPlayTime() {
        return playTime;
    }

    int getRanking() {
        return ranking;
    }

    string getFormattedTime() const {
        int minutes = playTime / 60;
        int seconds = playTime % 60;
        stringstream ss;
        ss << setw(2) << setfill('0') << minutes << ":"
           << setw(2) << setfill('0') << seconds;
        return ss.str();
    }
};




class leaderboard {
    string fileName = "files/leaderboard.txt";
    vector<Player> players;

public:
    leaderboard() {
        readLeaderboardFile();
    }


    vector<Player> getPlayers() {
        return players;
    }


    void readLeaderboardFile() {
        players.clear();
        ifstream leaderboardFile(fileName);

        string line;
        while (getline(leaderboardFile, line)) {
            //cout << "reading leaderboard file" << endl;
            size_t comma = line.find(',');
            if (comma != string::npos) {
                string time = line.substr(0, comma);
                string name = line.substr(comma + 1);

                size_t colonPos = time.find(':');
                if (colonPos != string::npos) {
                    int minutes = stoi(time.substr(0, colonPos));
                    int seconds = stoi(time.substr(colonPos + 1));
                    int playTime = minutes * 60 + seconds;

                    players.emplace_back(name, playTime, 0);
                }
            }

        }

        leaderboardFile.close();
        sortPlayers();
        updateRankings();
    }


    void writeToFile(string fileName) {
        ofstream leaderboardFile(fileName);
        for (auto &player : players) {
            leaderboardFile << player.getFormattedTime() << "," << player.getName() << "\n";
        }
        leaderboardFile.close();
    }


    int parseTime(const string& time) {
        size_t colon = time.find(':');
        if (colon != string::npos) {
            int minutes = stoi(time.substr(0, colon));
            int seconds = stoi(time.substr(colon + 1));
            return minutes * 60 + seconds;
        }
        return 0;
    }


    void printLeaderboard() {
        for (auto i = 0; i < players.size(); i++) {
            cout << players[i].ranking << ". " << players[i].getFormattedTime() << ", " << players[i].name << endl;
        }
    }


    void addPlayer(const string& name, int playTime) {
        readLeaderboardFile();
        for (const auto& player : players) {
            if (player.name == name && player.playTime == playTime) {
                return;
            }
        }

        players.emplace_back(name, playTime, 0);
        sortPlayers();
        updateRankings();

        if (players.size() > 5) {
            players.pop_back();
        }

        writeToFile(fileName);
        printLeaderboard();
    }


    bool fastestPlayer(string playerName) {
        return players[0].name == playerName;
    }


    void updateRankings() {
        for (auto i = 0; i < players.size(); ++i) {
            if (i == 0) {
                if (players[i].name.back() != '*') {
                    players[i].name += "*";
                }
            } else {
                if (!players[i].name.empty() && players[i].name.back() == '*') {
                    players[i].name.pop_back();
                }
            }
            players[i].ranking = i + 1;
        }
    }


    void sortPlayers() {
        sort(players.begin(), players.end(), [](const Player &a, const Player &b) {
            return a.playTime < b.playTime;
        });
    }


    void displayLeaderboard(sf::RenderWindow& window, sf::Font& font) {
        stringstream leaderboardStream;
        for (auto i = 0; i < players.size(); ++i) {
            leaderboardStream << players[i].ranking << ".\t" << players[i].getFormattedTime() << "\t"<< players[i].name << "\n\n";
        }

        sf::Text leaderboardText(leaderboardStream.str(), font, 20);
        sf::FloatRect textRect = leaderboardText.getLocalBounds();
        leaderboardText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f - 25);
        leaderboardText.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);

        window.draw(leaderboardText);
        window.display();
    }
};

#endif //LEADERBOARD_H

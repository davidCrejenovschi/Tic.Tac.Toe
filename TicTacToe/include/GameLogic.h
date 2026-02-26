#pragma once

class GameLogic {
private:
    char** board;
    char playerChar;
    int unmarkedCell;
    bool isGameOver = false;
    bool isMyTurn;

    bool verifyWin(int r, int c);
    bool verifyDraw();

public:
    GameLogic();
    ~GameLogic();

    char** getBoardData();
    char getPlayerChar();
    bool getIsMyTurn();
    void reset(char playerInit);
    char markBoard(int row, int col);
    char markOpponentMove(int row, int col);
};
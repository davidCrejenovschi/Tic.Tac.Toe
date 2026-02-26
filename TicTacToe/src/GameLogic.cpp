#include "GameLogic.h"

bool GameLogic::verifyWin(int r, int c) {
    char p = board[r][c];
    if (board[r][0] == p && board[r][1] == p && board[r][2] == p) return true;
    if (board[0][c] == p && board[1][c] == p && board[2][c] == p) return true;
    if (r == c) {
        if (board[0][0] == p && board[1][1] == p && board[2][2] == p) return true;
    }
    if (r + c == 2) {
        if (board[0][2] == p && board[1][1] == p && board[2][0] == p) return true;
    }
    return false;
}

bool GameLogic::verifyDraw() {
    return unmarkedCell == 0;
}

GameLogic::GameLogic() {
    board = new char* [3];
    for (int i = 0; i < 3; i++) {
        board[i] = new char[3];
    }
}

GameLogic::~GameLogic() {
    for (int i = 0; i < 3; i++) {
        delete[] board[i];
    }
    delete[] board;
}

char** GameLogic::getBoardData() {
    return board;
}

char GameLogic::getPlayerChar() {
    return playerChar;
}

bool GameLogic::getIsMyTurn() {
    return isMyTurn;
}

void GameLogic::reset(char playerInit) {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            board[i][j] = ' ';

    playerChar = playerInit;
    isMyTurn = (playerChar == 'X');
    unmarkedCell = 9;
    isGameOver = false;
}

char GameLogic::markBoard(int row, int col) {
    if (!isMyTurn || isGameOver) return ' ';

    if (board[row][col] == ' ') {
        board[row][col] = playerChar;
        unmarkedCell--;
        isMyTurn = false;

        if (verifyWin(row, col)) {
            isGameOver = true;
            return playerChar;
        }
        else if (verifyDraw()) {
            isGameOver = true;
            return 'D';
        }
    }
    else {
        return 'E';
    }

    return ' ';
}

char GameLogic::markOpponentMove(int row, int col) {
    if (isGameOver) return ' ';

    char oppChar = (playerChar == 'X') ? 'O' : 'X';
    board[row][col] = oppChar;
    unmarkedCell--;
    isMyTurn = true;

    if (verifyWin(row, col)) {
        isGameOver = true;
        return oppChar;
    }
    else if (verifyDraw()) {
        isGameOver = true;
        return 'D';
    }

    return ' ';
}
#include <iostream>


class GameLogic {

private:
    char** board;
    char playerChar;
    int unmakedCell;
    bool isGameOver = false;
    bool isMyTurn;


public:


    GameLogic(char playerInit) {

        board = new char* [3];

        for (int i = 0; i < 3; i++) {
            board[i] = new char[3];
        }

        reset(playerInit);
    }

    ~GameLogic() {

        for (int i = 0; i < 3; i++) {
            delete[] board[i];
        }
        delete[] board;

    }

    char** getBoardData() {
        return board;
    }

    void reset(char playerInit) {
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                board[i][j] = ' ';

        playerChar = playerInit;
        isMyTurn = (playerChar == 'X');
        unmakedCell = 9;
        isGameOver = false;
    }

    char markBoard(int row, int col) {

        if (!isMyTurn) return ' ';

        if (board[row][col] == ' ') {
            board[row][col] = playerChar;
            unmakedCell--;
            isMyTurn = !isMyTurn;
            if (verifyWin(row, col)) {
                std::cout << "Player " << playerChar << " wins!" << std::endl;
                isGameOver = true;
                return playerChar;
            }
            else if (verifyDraw()) {
                std::cout << "It's a draw!" << std::endl;
                isGameOver = true;
                return 'D';
            }
        }
        else {
            return 'E';
        }

        return ' ';
    }


private:

    bool verifyWin(int r, int c) {

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

    bool verifyDraw() {
        return unmakedCell == 0;
    }

};
#pragma once

#include <SDL.h>
#include <string>

enum class UIState {
    MENU,
    WAITING_FOR_ACCEPT,
    RECEIVING_INVITE,
    GAME,
    MESSAGE_SCREEN
};

class GameGUI {
private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    bool isRunning = true;

    char ipBuffer[128] = "";
    std::string playerCode = "127.0.0.1";
    std::string connectionError = "";

    UIState currentState = UIState::MENU;

    bool requestConnect = false;
    bool requestAccept = false;
    bool requestReject = false;
    bool requestLeave = false;
    bool requestBackToMenu = false;
    int clickedCell = -1;

    int timeRemaining = 10;
    std::string inviteSenderIP = "";
    std::string displayMessage = "";

    char** boardData = nullptr;
    char myChar = ' ';
    bool isMyTurn = false;
    bool isConnectingStatus = false;
    bool isLocalModeUI = false;

    float w = 0.0f;
    float h = 0.0f;
    float fontScale = 1.0f;
    float btnHeight = 0.0f;
    float contentWidth = 0.0f;
    bool isPortrait = false;

    void calculateScaling();
    void drawHeader();
    void renderMenu();
    void renderConnecting();
    void renderGame();
    void renderMessageScreen();

public:
    GameGUI();

    bool init();
    void handleEvents();
    void render();

    bool getRunning();
    void setUIState(UIState state);
    UIState getUIState();
    void setPlayerCode(const std::string& code);
    std::string getConnectIP();
    void setTimeRemaining(int t);
    void setInviteSenderIP(std::string ip);
    void setDisplayMessage(std::string msg);
    void setConnectionError(std::string err);
    bool getIsMyTurn();
    void setIsConnecting(bool s);
    void setIsLocalMode(bool l);

    void setGameData(char** b, char c, bool turn);

    bool popConnectRequest();
    bool popAcceptRequest();
    bool popRejectRequest();
    bool popLeaveRequest();
    bool popBackToMenuRequest();
    int popClickedCell();

    void clean();
};
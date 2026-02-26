#include "GameGUI.h"
#include "NetworkManager.h"
#include "GameLogic.h"
#include <thread>
#include <atomic>

const int APP_PORT = 7777;

int main(int argc, char* argv[]) {
    GameGUI gui;
    NetworkManager net;
    GameLogic logic;

    gui.init();
    net.Init();

    std::string myIP = net.GetLocalIP();
    gui.setPlayerCode(myIP);

    bool isHosting = net.StartHosting(APP_PORT);
    bool isLocalMode = false;

    std::atomic<bool> isConnectingFlag(false);
    std::atomic<int> connectionResult(0);

    const int FPS = 60;
    const int frameDelay = 1000 / FPS;
    Uint32 frameStart;
    int frameTime;

    Uint32 connectStartTime = 0;

    while (gui.getRunning()) {
        frameStart = SDL_GetTicks();
        gui.handleEvents();

        gui.setIsConnecting(isConnectingFlag.load());
        gui.setIsLocalMode(isLocalMode);

        if (gui.getUIState() == UIState::MENU) {
            if (isHosting && net.CheckForIncomingConnection()) {
                gui.setUIState(UIState::RECEIVING_INVITE);
                gui.setInviteSenderIP("Incoming Player");
            }

            if (gui.popConnectRequest() && !isConnectingFlag.load()) {
                std::string targetIP = gui.getConnectIP();

                if (targetIP.empty()) {
                    gui.setConnectionError("PLEASE ENTER AN IP");
                }
                else if (targetIP == myIP || targetIP == "127.0.0.1") {
                    gui.setConnectionError("");
                    isLocalMode = true;
                    logic.reset('X');
                    gui.setGameData(logic.getBoardData(), 'B', true);
                    gui.setUIState(UIState::GAME);
                }
                else {
                    gui.setConnectionError("WAITING TO CONNECT...");
                    isConnectingFlag.store(true);
                    connectionResult.store(0);

                    std::thread([&net, targetIP, &isConnectingFlag, &connectionResult]() {
                        if (net.Connect(targetIP, APP_PORT)) {
                            connectionResult.store(1);
                        }
                        else {
                            connectionResult.store(2);
                        }
                        isConnectingFlag.store(false);
                        }).detach();
                }
            }

            if (connectionResult.load() == 1) {
                connectionResult.store(0);
                gui.setConnectionError("");
                isLocalMode = false;
                gui.setUIState(UIState::WAITING_FOR_ACCEPT);
                connectStartTime = SDL_GetTicks();
                GamePacket invite = { 0, 0, 0 };
                net.SendPacket(invite);
            }
            else if (connectionResult.load() == 2) {
                connectionResult.store(0);
                gui.setConnectionError("INVALID IP OR PLAYER OFFLINE");
            }
        }

        if (gui.popAcceptRequest()) {
            GamePacket accept = { 1, 0, 0 };
            if (net.SendPacket(accept)) {
                logic.reset('O');
                gui.setGameData(logic.getBoardData(), logic.getPlayerChar(), logic.getIsMyTurn());
                gui.setUIState(UIState::GAME);
            }
            else {
                net.Disconnect();
                gui.setUIState(UIState::MENU);
            }
        }

        if (gui.popRejectRequest()) {
            GamePacket reject = { 2, 0, 0 };
            net.SendPacket(reject);
            net.Disconnect();
            gui.setUIState(UIState::MENU);
        }

        if (gui.popLeaveRequest()) {
            if (!isLocalMode) {
                GamePacket surrender = { 4, 0, 0 };
                net.SendPacket(surrender);
                net.Disconnect();
                gui.setDisplayMessage("YOU SURRENDERED");
            }
            else {
                gui.setDisplayMessage("LOCAL GAME ENDED");
            }
            gui.setUIState(UIState::MESSAGE_SCREEN);
            isLocalMode = false;
        }

        if (gui.popBackToMenuRequest()) {
            net.Disconnect();
            isLocalMode = false;
            gui.setUIState(UIState::MENU);
        }

        if (gui.getUIState() == UIState::GAME) {
            int cell = gui.popClickedCell();
            if (cell != -1) {
                int r = cell / 3;
                int c = cell % 3;

                if (isLocalMode) {
                    char res = 'E';
                    if (gui.getIsMyTurn()) {
                        res = logic.markBoard(r, c);
                    }
                    else {
                        res = logic.markOpponentMove(r, c);
                    }

                    if (res != 'E') {
                        gui.setGameData(logic.getBoardData(), 'B', !gui.getIsMyTurn());
                        if (res == 'X' || res == 'O') {
                            std::string winMsg = std::string(1, res) + " WINS!";
                            gui.setDisplayMessage(winMsg);
                            gui.setUIState(UIState::MESSAGE_SCREEN);
                        }
                        else if (res == 'D') {
                            gui.setDisplayMessage("IT'S A DRAW!");
                            gui.setUIState(UIState::MESSAGE_SCREEN);
                        }
                    }
                }
                else {
                    char res = logic.markBoard(r, c);
                    if (res != 'E') {
                        GamePacket move = { 3, r, c };
                        net.SendPacket(move);
                        gui.setGameData(logic.getBoardData(), logic.getPlayerChar(), logic.getIsMyTurn());

                        if (res == logic.getPlayerChar()) {
                            gui.setDisplayMessage("YOU WIN!");
                            gui.setUIState(UIState::MESSAGE_SCREEN);
                        }
                        else if (res == 'D') {
                            gui.setDisplayMessage("IT'S A DRAW!");
                            gui.setUIState(UIState::MESSAGE_SCREEN);
                        }
                    }
                }
            }
        }

        GamePacket incoming;
        if (!isLocalMode && net.ReceivePacket(incoming)) {
            switch (incoming.type) {
            case 0:
                break;

            case 1:
                if (gui.getUIState() == UIState::WAITING_FOR_ACCEPT) {
                    logic.reset('X');
                    gui.setGameData(logic.getBoardData(), logic.getPlayerChar(), logic.getIsMyTurn());
                    gui.setUIState(UIState::GAME);
                }
                break;

            case 2:
                if (gui.getUIState() == UIState::WAITING_FOR_ACCEPT) {
                    gui.setDisplayMessage("CONNECTION REJECTED");
                    gui.setUIState(UIState::MESSAGE_SCREEN);
                    net.Disconnect();
                }
                break;

            case 3:
                if (gui.getUIState() == UIState::GAME) {
                    char res = logic.markOpponentMove(incoming.data1, incoming.data2);
                    gui.setGameData(logic.getBoardData(), logic.getPlayerChar(), logic.getIsMyTurn());
                    if (res != ' ') {
                        gui.setDisplayMessage(res == 'D' ? "DRAW!" : "OPPONENT WINS!");
                        gui.setUIState(UIState::MESSAGE_SCREEN);
                    }
                }
                break;

            case 4:
                gui.setDisplayMessage("OPPONENT LEFT");
                gui.setUIState(UIState::MESSAGE_SCREEN);
                net.Disconnect();
                break;

            case 6:
                gui.setDisplayMessage("PLAYER BUSY");
                gui.setUIState(UIState::MESSAGE_SCREEN);
                net.Disconnect();
                break;

            case 7:
                if (gui.getUIState() == UIState::RECEIVING_INVITE) {
                    gui.setDisplayMessage("INVITE CANCELED");
                    gui.setUIState(UIState::MESSAGE_SCREEN);
                    net.Disconnect();
                }
                break;
            }
        }

        if (gui.getUIState() == UIState::WAITING_FOR_ACCEPT) {
            Uint32 elapsed = SDL_GetTicks() - connectStartTime;
            int secondsLeft = 10 - (elapsed / 1000);
            gui.setTimeRemaining(secondsLeft);

            if (elapsed > 10000) {
                GamePacket cancel = { 7, 0, 0 };
                net.SendPacket(cancel);
                net.Disconnect();
                gui.setDisplayMessage("NO RESPONSE");
                gui.setUIState(UIState::MESSAGE_SCREEN);
            }
        }

        if (!isLocalMode && !net.IsConnected()) {
            UIState s = gui.getUIState();
            if (s == UIState::GAME) {
                gui.setDisplayMessage("CONNECTION LOST");
                gui.setUIState(UIState::MESSAGE_SCREEN);
            }
            else if (s == UIState::WAITING_FOR_ACCEPT || s == UIState::RECEIVING_INVITE) {
                gui.setUIState(UIState::MENU);
            }
        }

        gui.render();

        frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) SDL_Delay(frameDelay - frameTime);
    }

    net.Cleanup();
    gui.clean();
    return 0;
}
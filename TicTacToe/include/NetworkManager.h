#pragma once

#include <SDL_net.h>
#include <string>
#include "GamePacket.h"

class NetworkManager {
private:
    TCPsocket serverSocket = nullptr;
    TCPsocket clientSocket = nullptr;
    SDLNet_SocketSet socketSet = nullptr;

public:
    NetworkManager();

    bool Init();
    bool StartHosting(int port);
    bool Connect(const std::string& hostIP, int port);
    bool CheckForIncomingConnection();
    bool SendPacket(GamePacket packet);
    bool ReceivePacket(GamePacket& outPacket);
    void Disconnect();
    void StopHosting();
    bool IsConnected();
    std::string GetLocalIP();
    void Cleanup();
};
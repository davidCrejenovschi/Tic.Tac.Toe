#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#undef INADDR_ANY
#undef INADDR_BROADCAST
#undef INADDR_NONE
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#endif

#include "NetworkManager.h"
#include <thread>
#include <atomic>
#include <memory>

struct ConnectContext {
    std::atomic<bool> timeoutTriggered{ false };
    std::atomic<bool> isFinished{ false };
    TCPsocket socket{ nullptr };
};

NetworkManager::NetworkManager() {}

bool NetworkManager::Init() {
    if (SDLNet_Init() == -1) return false;
    socketSet = SDLNet_AllocSocketSet(2);
    return true;
}

bool NetworkManager::StartHosting(int port) {
    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, NULL, port) == -1) return false;
    serverSocket = SDLNet_TCP_Open(&ip);
    if (!serverSocket) return false;
    SDLNet_TCP_AddSocket(socketSet, serverSocket);
    return true;
}

bool NetworkManager::Connect(const std::string& hostIP, int port) {
    auto ctx = std::make_shared<ConnectContext>();

    std::thread([ctx, hostIP, port]() {
        IPaddress ip;
        if (SDLNet_ResolveHost(&ip, hostIP.c_str(), port) != -1) {
            TCPsocket tempSocket = SDLNet_TCP_Open(&ip);
            if (tempSocket) {
                if (ctx->timeoutTriggered.load()) {
                    SDLNet_TCP_Close(tempSocket);
                }
                else {
                    ctx->socket = tempSocket;
                }
            }
        }
        ctx->isFinished.store(true);
        }).detach();

    int waitTime = 10000;
    while (waitTime > 0 && !ctx->isFinished.load()) {
        SDL_Delay(50);
        waitTime -= 50;
    }

    ctx->timeoutTriggered.store(true);

    if (ctx->socket) {
        clientSocket = ctx->socket;
        SDLNet_TCP_AddSocket(socketSet, clientSocket);
        return true;
    }

    return false;
}

bool NetworkManager::CheckForIncomingConnection() {
    if (!serverSocket) return false;
    if (SDLNet_CheckSockets(socketSet, 0) > 0 && SDLNet_SocketReady(serverSocket)) {
        TCPsocket temp = SDLNet_TCP_Accept(serverSocket);
        if (temp) {
            if (clientSocket) {
                GamePacket busy = { 6, 0, 0 };
                SDLNet_TCP_Send(temp, &busy, sizeof(GamePacket));
                SDLNet_TCP_Close(temp);
            }
            else {
                clientSocket = temp;
                SDLNet_TCP_AddSocket(socketSet, clientSocket);
                return true;
            }
        }
    }
    return false;
}

bool NetworkManager::SendPacket(GamePacket packet) {
    if (!clientSocket) return false;
    return SDLNet_TCP_Send(clientSocket, &packet, sizeof(GamePacket)) >= (int)sizeof(GamePacket);
}

bool NetworkManager::ReceivePacket(GamePacket& outPacket) {
    if (!clientSocket) return false;
    if (SDLNet_CheckSockets(socketSet, 0) > 0 && SDLNet_SocketReady(clientSocket)) {
        int res = SDLNet_TCP_Recv(clientSocket, &outPacket, sizeof(GamePacket));
        if (res <= 0) {
            Disconnect();
            return false;
        }
        return true;
    }
    return false;
}

void NetworkManager::Disconnect() {
    if (clientSocket) {
        SDLNet_TCP_DelSocket(socketSet, clientSocket);
        SDLNet_TCP_Close(clientSocket);
        clientSocket = nullptr;
    }
}

void NetworkManager::StopHosting() {
    Disconnect();
    if (serverSocket) {
        SDLNet_TCP_DelSocket(socketSet, serverSocket);
        SDLNet_TCP_Close(serverSocket);
        serverSocket = nullptr;
    }
}

bool NetworkManager::IsConnected() {
    return clientSocket != nullptr;
}

std::string NetworkManager::GetLocalIP() {
    std::string localIP = "127.0.0.1";

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return localIP;
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        WSACleanup();
        return localIP;
    }
#else
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) return localIP;
#endif

    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(53);
    inet_pton(AF_INET, "8.8.8.8", &serv.sin_addr);

    int err = connect(sock, (const struct sockaddr*)&serv, sizeof(serv));
    if (err == 0) {
        struct sockaddr_in name;
#ifdef _WIN32
        int namelen = sizeof(name);
#else
        socklen_t namelen = sizeof(name);
#endif
        err = getsockname(sock, (struct sockaddr*)&name, &namelen);
        if (err == 0) {
            char buffer[INET_ADDRSTRLEN];
            if (inet_ntop(AF_INET, &name.sin_addr, buffer, sizeof(buffer)) != nullptr) {
                localIP = buffer;
            }
        }
    }

#ifdef _WIN32
    closesocket(sock);
    WSACleanup();
#else
    close(sock);
#endif

    return localIP;
}

void NetworkManager::Cleanup() {
    StopHosting();
    if (socketSet) {
        SDLNet_FreeSocketSet(socketSet);
        socketSet = nullptr;
    }
    SDLNet_Quit();
}
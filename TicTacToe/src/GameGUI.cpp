#include "GameGUI.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <sstream>

GameGUI::GameGUI() {}

void GameGUI::calculateScaling() {
    w = ImGui::GetIO().DisplaySize.x;
    h = ImGui::GetIO().DisplaySize.y;
    if (w < 1.0f) w = 1.0f;
    if (h < 1.0f) h = 1.0f;

    isPortrait = (h > w);

    if (isPortrait) {
        contentWidth = w * 0.85f;
        btnHeight = h * 0.07f;
        if (btnHeight < 50.0f) btnHeight = 50.0f;
        fontScale = w / 450.0f;
    }
    else {
        contentWidth = w * 0.5f;
        btnHeight = h * 0.08f;
        if (btnHeight > 100.0f) btnHeight = 100.0f;
        if (btnHeight < 60.0f) btnHeight = 60.0f;
        fontScale = h / 700.0f;
    }

    if (fontScale <= 0.0f) fontScale = 0.01f;
}

void GameGUI::drawHeader() {
    float margin = 20.0f * fontScale;

    ImGui::SetCursorPos(ImVec2(margin, margin));

    if (currentState == UIState::GAME) {
        if (isLocalModeUI) {
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.3f, 1.0f), "CURRENT TURN:");
            ImGui::SetCursorPos(ImVec2(margin, margin + (25.0f * fontScale)));
            std::string turnStr = isMyTurn ? "X" : "O";
            ImGui::Text("%s", turnStr.c_str());
        }
        else {
            ImGui::TextColored(ImVec4(0.3f, 0.8f, 0.3f, 1.0f), "PLAYING AS:");
            ImGui::SetCursorPos(ImVec2(margin, margin + (25.0f * fontScale)));
            std::string pc = "";
            pc += myChar;
            ImGui::Text("%s", pc.c_str());
        }
    }
    else {
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "PLAYER ID:");
        ImGui::SetCursorPos(ImVec2(margin, margin + (25.0f * fontScale)));
        ImGui::Text("%s", playerCode.c_str());
    }

    std::string btnText = (currentState == UIState::GAME) ? "LEAVE" : "EXIT";

    float pad = 10.0f * fontScale;
    float exitTextW = ImGui::CalcTextSize(btnText.c_str()).x;
    float exitW = exitTextW + (50.0f * fontScale);
    float exitH = ImGui::GetFontSize() + (pad * 2.0f);

    ImGui::SetCursorPos(ImVec2(w - exitW - margin, margin));

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
    if (ImGui::Button(btnText.c_str(), ImVec2(exitW, exitH))) {
        if (currentState == UIState::GAME) {
            requestLeave = true;
        }
        else {
            isRunning = false;
        }
    }
    ImGui::PopStyleColor();
}

void GameGUI::renderMenu() {
    float startY = isPortrait ? h * 0.50f : h * 0.45f;

    ImGui::SetWindowFontScale(fontScale * 3.0f);
    float titleW = ImGui::CalcTextSize("TIC TAC TOE").x;
    ImGui::SetCursorPos(ImVec2((w - titleW) * 0.5f, h * 0.20f));
    ImGui::Text("TIC TAC TOE");

    ImGui::SetWindowFontScale(fontScale * 1.3f);

    ImGui::SetCursorPos(ImVec2((w - contentWidth) * 0.5f, startY));
    ImGui::SetNextItemWidth(contentWidth);

    float fontSize = ImGui::GetFontSize();
    float inputPadY = (btnHeight - fontSize) * 0.5f;
    if (inputPadY < 5.0f) inputPadY = 5.0f;

    if (isConnectingStatus) {
        ImGui::BeginDisabled();
    }
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(15.0f, inputPadY));
    ImGui::InputTextWithHint("##IP", "Enter IP Address...", ipBuffer, sizeof(ipBuffer));
    ImGui::PopStyleVar();
    if (isConnectingStatus) {
        ImGui::EndDisabled();
    }

    float gap = 25.0f * fontScale;
    ImGui::SetCursorPos(ImVec2((w - contentWidth) * 0.5f, startY + btnHeight + gap));

    if (isConnectingStatus) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
        ImGui::Button("CONNECT", ImVec2(contentWidth, btnHeight));
        ImGui::PopStyleColor(3);
    }
    else {
        if (ImGui::Button("CONNECT", ImVec2(contentWidth, btnHeight))) {
            requestConnect = true;
        }
    }

    if (!connectionError.empty()) {
        ImGui::SetWindowFontScale(fontScale * 1.1f);
        float errW = ImGui::CalcTextSize(connectionError.c_str()).x;
        ImGui::SetCursorPos(ImVec2((w - errW) * 0.5f, startY + (btnHeight * 2.0f) + gap + (10.0f * fontScale)));

        if (isConnectingStatus) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
        }
        else {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
        }

        ImGui::Text("%s", connectionError.c_str());
        ImGui::PopStyleColor();
    }
}

void GameGUI::renderConnecting() {
    if (currentState == UIState::WAITING_FOR_ACCEPT) {
        ImGui::SetWindowFontScale(fontScale * 2.0f);
        std::string text = "CONNECTING...";
        float textW = ImGui::CalcTextSize(text.c_str()).x;
        ImGui::SetCursorPos(ImVec2((w - textW) * 0.5f, h * 0.40f));
        ImGui::Text("%s", text.c_str());

        ImGui::SetWindowFontScale(fontScale * 4.0f);
        std::string timeStr = std::to_string(timeRemaining);
        float timeW = ImGui::CalcTextSize(timeStr.c_str()).x;
        ImGui::SetCursorPos(ImVec2((w - timeW) * 0.5f, h * 0.55f));
        ImGui::Text("%s", timeStr.c_str());
    }
    else if (currentState == UIState::RECEIVING_INVITE) {
        ImGui::SetWindowFontScale(fontScale * 1.5f);
        std::string text = "INCOMING MATCH FROM:";
        float textW = ImGui::CalcTextSize(text.c_str()).x;
        ImGui::SetCursorPos(ImVec2((w - textW) * 0.5f, h * 0.35f));
        ImGui::Text("%s", text.c_str());

        ImGui::SetWindowFontScale(fontScale * 2.0f);
        float ipW = ImGui::CalcTextSize(inviteSenderIP.c_str()).x;
        ImGui::SetCursorPos(ImVec2((w - ipW) * 0.5f, h * 0.45f));
        ImGui::Text("%s", inviteSenderIP.c_str());

        float btnW = contentWidth * 0.45f;
        float gap = contentWidth * 0.10f;
        float startX = (w - contentWidth) * 0.5f;
        float btnY = h * 0.60f;

        ImGui::SetWindowFontScale(fontScale * 1.3f);

        ImGui::SetCursorPos(ImVec2(startX, btnY));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.3f, 1.0f));
        if (ImGui::Button("ACCEPT", ImVec2(btnW, btnHeight))) {
            requestAccept = true;
        }
        ImGui::PopStyleColor();

        ImGui::SetCursorPos(ImVec2(startX + btnW + gap, btnY));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
        if (ImGui::Button("REJECT", ImVec2(btnW, btnHeight))) {
            requestReject = true;
        }
        ImGui::PopStyleColor();
    }
}

void GameGUI::renderGame() {
    float gridWidth = isPortrait ? w * 0.9f : h * 0.6f;
    float cellSize = gridWidth / 3.0f;
    float startX = (w - gridWidth) * 0.5f;
    float startY = (h - gridWidth) * 0.5f;

    if (!isLocalModeUI) {
        std::string turnText = isMyTurn ? "YOUR TURN" : "OPPONENT'S TURN";
        ImVec4 turnColor = isMyTurn ? ImVec4(0.3f, 0.8f, 0.3f, 1.0f) : ImVec4(0.8f, 0.3f, 0.3f, 1.0f);

        ImGui::SetWindowFontScale(fontScale * 1.5f);
        float tw = ImGui::CalcTextSize(turnText.c_str()).x;
        ImGui::SetCursorPos(ImVec2((w - tw) * 0.5f, startY - (60.0f * fontScale)));
        ImGui::TextColored(turnColor, "%s", turnText.c_str());
    }

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImU32 lineColor = ImGui::GetColorU32(ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
    float thickness = 4.0f * fontScale;

    drawList->AddLine(ImVec2(startX + cellSize, startY), ImVec2(startX + cellSize, startY + gridWidth), lineColor, thickness);
    drawList->AddLine(ImVec2(startX + cellSize * 2, startY), ImVec2(startX + cellSize * 2, startY + gridWidth), lineColor, thickness);
    drawList->AddLine(ImVec2(startX, startY + cellSize), ImVec2(startX + gridWidth, startY + cellSize), lineColor, thickness);
    drawList->AddLine(ImVec2(startX, startY + cellSize * 2), ImVec2(startX + gridWidth, startY + cellSize * 2), lineColor, thickness);

    if (boardData) {
        float padding = cellSize * 0.2f;
        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3; c++) {
                char val = boardData[r][c];
                float cellX = startX + c * cellSize;
                float cellY = startY + r * cellSize;

                if (val == 'X') {
                    ImU32 xColor = ImGui::GetColorU32(ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
                    drawList->AddLine(ImVec2(cellX + padding, cellY + padding), ImVec2(cellX + cellSize - padding, cellY + cellSize - padding), xColor, thickness * 1.5f);
                    drawList->AddLine(ImVec2(cellX + cellSize - padding, cellY + padding), ImVec2(cellX + padding, cellY + cellSize - padding), xColor, thickness * 1.5f);
                }
                else if (val == 'O') {
                    ImU32 oColor = ImGui::GetColorU32(ImVec4(0.3f, 0.6f, 0.9f, 1.0f));
                    drawList->AddCircle(ImVec2(cellX + cellSize * 0.5f, cellY + cellSize * 0.5f), (cellSize * 0.5f) - padding, oColor, 32, thickness * 1.5f);
                }
            }
        }
    }

    if (ImGui::IsMouseClicked(0)) {
        if (isMyTurn || isLocalModeUI) {
            ImVec2 mousePos = ImGui::GetMousePos();
            if (mousePos.x > startX && mousePos.x < startX + gridWidth &&
                mousePos.y > startY && mousePos.y < startY + gridWidth) {
                int col = static_cast<int>((mousePos.x - startX) / cellSize);
                int row = static_cast<int>((mousePos.y - startY) / cellSize);
                clickedCell = row * 3 + col;
            }
        }
    }
}

void GameGUI::renderMessageScreen() {
    ImGui::SetWindowFontScale(fontScale * 2.0f);

    std::stringstream ss(displayMessage);
    std::string line;
    float currentY = h * 0.35f;
    float lineHeight = ImGui::GetFontSize() * 1.1f;

    while (std::getline(ss, line, '\n')) {
        float textW = ImGui::CalcTextSize(line.c_str()).x;
        ImGui::SetCursorPos(ImVec2((w - textW) * 0.5f, currentY));
        ImGui::Text("%s", line.c_str());
        currentY += lineHeight;
    }

    ImGui::SetWindowFontScale(fontScale * 1.3f);
    float btnY = h * 0.65f;

    if (currentY > btnY - (btnHeight / 2)) {
        btnY = currentY + btnHeight;
    }

    ImGui::SetCursorPos(ImVec2((w - contentWidth) * 0.5f, btnY));

    if (ImGui::Button("BACK TO MAIN MENU", ImVec2(contentWidth, btnHeight))) {
        requestBackToMenu = true;
    }
}

bool GameGUI::init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) return false;

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
    window = SDL_CreateWindow("Tic Tac Toe", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, window_flags);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!window || !renderer) return false;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

#ifdef __ANDROID__
    SDL_RWops* file = SDL_RWFromFile("SNPro-Medium.ttf", "rb");
    if (file) {
        Sint64 size = SDL_RWsize(file);
        void* data = SDL_malloc(size);
        SDL_RWread(file, data, size, 1);
        SDL_RWclose(file);

        ImFontConfig fontConfig;
        fontConfig.FontDataOwnedByAtlas = true;
        io.Fonts->AddFontFromMemoryTTF(data, size, 24.0f, &fontConfig);
    }
    else {
        io.Fonts->AddFontDefault();
    }
#else
    io.Fonts->AddFontFromFileTTF("Assets/Fonts/SN_Pro/SNPro-Medium.ttf", 24.0f);
#endif

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.1f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.15f, 0.35f, 0.85f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.45f, 0.95f, 1.0f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.2f, 1.0f);
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);

    return true;
}

void GameGUI::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT) isRunning = false;
    }

#ifdef __ANDROID__
    if (ImGui::GetIO().WantTextInput) {
        if (!SDL_IsTextInputActive()) {
            SDL_StartTextInput();
        }
    }
    else {
        if (SDL_IsTextInputActive()) {
            SDL_StopTextInput();
        }
    }
#endif
}

void GameGUI::render() {
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    calculateScaling();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(w, h));
    ImGui::Begin("BG", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus);

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, btnHeight * 0.25f);
    float pad = 10.0f * fontScale;
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(pad, pad));
    ImGui::SetWindowFontScale(fontScale);

    if (currentState != UIState::MESSAGE_SCREEN) {
        drawHeader();
    }

    if (currentState == UIState::MENU) {
        renderMenu();
    }
    else if (currentState == UIState::WAITING_FOR_ACCEPT || currentState == UIState::RECEIVING_INVITE) {
        renderConnecting();
    }
    else if (currentState == UIState::GAME) {
        renderGame();
    }
    else if (currentState == UIState::MESSAGE_SCREEN) {
        renderMessageScreen();
    }

    ImGui::PopStyleVar(2);
    ImGui::End();

    ImGui::Render();
    SDL_SetRenderDrawColor(renderer, 20, 20, 25, 255);
    SDL_RenderClear(renderer);
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);
}

bool GameGUI::getRunning() { return isRunning; }
void GameGUI::setUIState(UIState state) { currentState = state; }
UIState GameGUI::getUIState() { return currentState; }
void GameGUI::setPlayerCode(const std::string& code) { playerCode = code; }
std::string GameGUI::getConnectIP() { return std::string(ipBuffer); }
void GameGUI::setTimeRemaining(int t) { timeRemaining = t; }
void GameGUI::setInviteSenderIP(std::string ip) { inviteSenderIP = ip; }
void GameGUI::setDisplayMessage(std::string msg) { displayMessage = msg; }
void GameGUI::setConnectionError(std::string err) { connectionError = err; }
bool GameGUI::getIsMyTurn() { return isMyTurn; }
void GameGUI::setIsConnecting(bool s) { isConnectingStatus = s; }
void GameGUI::setIsLocalMode(bool l) { isLocalModeUI = l; }

void GameGUI::setGameData(char** b, char c, bool turn) {
    boardData = b;
    myChar = c;
    isMyTurn = turn;
}

bool GameGUI::popConnectRequest() { if (requestConnect) { requestConnect = false; return true; } return false; }
bool GameGUI::popAcceptRequest() { if (requestAccept) { requestAccept = false; return true; } return false; }
bool GameGUI::popRejectRequest() { if (requestReject) { requestReject = false; return true; } return false; }
bool GameGUI::popLeaveRequest() { if (requestLeave) { requestLeave = false; return true; } return false; }
bool GameGUI::popBackToMenuRequest() { if (requestBackToMenu) { requestBackToMenu = false; return true; } return false; }
int GameGUI::popClickedCell() { int c = clickedCell; clickedCell = -1; return c; }

void GameGUI::clean() {
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
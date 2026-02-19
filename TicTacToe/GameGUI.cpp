#pragma once

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <SDL.h>
#include <stdio.h>
#include <string>
#include <algorithm> // Pentru std::max

class GameGUI {
private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    bool isRunning = true;
    char ipBuffer[128] = "";
    std::string playerCode = "UC-8829-X";

public:
    GameGUI() {}

    bool init() {
        if (SDL_Init(SDL_INIT_VIDEO) != 0) return false;

        SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

        // Dimensiune de start
        window = SDL_CreateWindow("Tic Tac Toe Client", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, window_flags);

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

        if (!window || !renderer) return false;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        // Font de baza 24px
        ImFont* font = io.Fonts->AddFontFromFileTTF("Assets/Fonts/SN_Pro/SNPro-Medium.ttf", 24.0f);
        if (font == nullptr) {
            printf("Warning: Font not found.\n");
        }

        ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
        ImGui_ImplSDLRenderer2_Init(renderer);

        ImGui::StyleColorsDark();

        ImGuiStyle& style = ImGui::GetStyle();
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.1f, 1.0f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.15f, 0.35f, 0.85f, 1.0f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.45f, 0.95f, 1.0f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.2f, 1.0f);

        // Fortam centrarea textului in butoane la nivel global
        style.ButtonTextAlign = ImVec2(0.5f, 0.5f);

        return true;
    }

    void handleEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) isRunning = false;
        }
    }

    void onConnectAttempt(const char* targetIP) {
        printf("Conectare la: %s\n", targetIP);
    }

    void render() {
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // 1. Dimensiuni
        float w = ImGui::GetIO().DisplaySize.x;
        float h = ImGui::GetIO().DisplaySize.y;

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(w, h));
        ImGui::Begin("BG", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollWithMouse);

        // --- LOGICA SCALARE ---
        bool isPortrait = (h > w);
        float contentWidth = 0.0f;
        float btnHeight = 0.0f;
        float fontScale = 1.0f;
        float startY = 0.0f;

        if (isPortrait) {
            // === MOBILE ===
            contentWidth = w * 0.85f;
            btnHeight = h * 0.07f;
            if (btnHeight < 50.0f) btnHeight = 50.0f;

            fontScale = w / 450.0f;
            startY = h * 0.50f;
        }
        else {
            // === DESKTOP ===
            contentWidth = w * 0.5f;
            btnHeight = h * 0.08f;

            if (btnHeight > 100.0f) btnHeight = 100.0f;
            if (btnHeight < 60.0f) btnHeight = 60.0f;

            fontScale = h / 700.0f;
            startY = h * 0.45f;
        }

        // --- APLICAM STILURILE ---
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, btnHeight * 0.25f);

        // FIX ALINIERE: Scalam si padding-ul!
        // Asta asigura ca textul nu e impins aiurea cand fontul e mic/mare.
        float pad = 10.0f * fontScale;
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(pad, pad));

        ImGui::SetWindowFontScale(fontScale);

        // ----------------------------------------
        // HEADER (Player + Exit)
        // ----------------------------------------
        float margin = 20.0f * fontScale;

        ImGui::SetCursorPos(ImVec2(margin, margin));
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "PLAYER ID:");
        ImGui::SetCursorPos(ImVec2(margin, margin + (25.0f * fontScale)));
        ImGui::Text("%s", playerCode.c_str());

        // --- EXIT BUTTON (Calculat Perfect) ---
        // Nu mai ghicim inaltimea. O calculam: Font + Padding.
        // Asta garanteaza centrarea verticala.
        float exitTextW = ImGui::CalcTextSize("EXIT").x;
        float exitW = exitTextW + (50.0f * fontScale); // Latime generoasa

        // Inaltimea o lasam pe cea a fontului + padding-ul setat mai sus * 2 (sus/jos)
        // Sau folosim GetFontSize() * un factor sigur
        float exitH = ImGui::GetFontSize() + (pad * 2.0f);

        ImGui::SetCursorPos(ImVec2(w - exitW - margin, margin));

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
        // Folosim Button simplu care isi ia inaltimea automat din font+padding
        // SAU fortam dimensiunea calculata corect
        if (ImGui::Button("EXIT", ImVec2(exitW, exitH))) {
            isRunning = false;
        }
        ImGui::PopStyleColor();

        // ----------------------------------------
        // TITLU
        // ----------------------------------------
        const char* title = "TIC TAC TOE";
        ImGui::SetWindowFontScale(fontScale * 3.0f);
        float titleW = ImGui::CalcTextSize(title).x;

        ImGui::SetCursorPos(ImVec2((w - titleW) * 0.5f, h * 0.20f));
        ImGui::Text("%s", title);

        // ----------------------------------------
        // ZONA INPUT + CONNECT
        // ----------------------------------------
        ImGui::SetWindowFontScale(fontScale * 1.3f);

        // 1. Input Box
        ImGui::SetCursorPos(ImVec2((w - contentWidth) * 0.5f, startY));
        ImGui::SetNextItemWidth(contentWidth);

        // Calculam padding vertical pentru Input ca sa fie la fel de inalt ca butonul
        float fontSize = ImGui::GetFontSize();
        float inputPadY = (btnHeight - fontSize) * 0.5f;
        if (inputPadY < 5.0f) inputPadY = 5.0f;

        // Suprascriem padding-ul doar pentru Input
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(15.0f, inputPadY));
        ImGui::InputTextWithHint("##IP", "Enter IP Address...", ipBuffer, sizeof(ipBuffer));
        ImGui::PopStyleVar();

        // 2. Connect Button
        float gap = 25.0f * fontScale;
        ImGui::SetCursorPos(ImVec2((w - contentWidth) * 0.5f, startY + btnHeight + gap));

        if (ImGui::Button("CONNECT", ImVec2(contentWidth, btnHeight))) {
            onConnectAttempt(ipBuffer);
        }

        // Clean up
        ImGui::PopStyleVar(2); // Rounding + Default Padding
        ImGui::End();

        ImGui::Render();
        SDL_SetRenderDrawColor(renderer, 20, 20, 25, 255);
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    bool getRunning() { return isRunning; }

    void clean() {
        ImGui_ImplSDLRenderer2_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
};
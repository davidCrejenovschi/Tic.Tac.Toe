#include "GameGUI.cpp"

int main(int argc, char* argv[]) {
    GameGUI gui;
    if (!gui.init()) return -1;

    while (gui.getRunning()) {
        gui.handleEvents();
        gui.render();
    }

    gui.clean();
    return 0;
}
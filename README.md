# ❌⭕ Tic Tac Toe

<p align="center">
  <img src="https://github.com/user-attachments/assets/36911a16-b68a-4ba0-a645-246c022f2b38" alt="Project Demo">
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C++-00599C?logo=c%2B%2B&logoColor=white" alt="C++">
  <img src="https://img.shields.io/badge/Library-SDL2-red" alt="SDL2">
  <img src="https://img.shields.io/badge/UI-ImGui-lightgrey" alt="ImGui">
</p>

## 📦 Supported Platforms
- **Windows:** `.exe` 
- **Android:** `.apk`

---

You can now play Tic Tac Toe on a screen! The backend is written in C++, and the user interface is a hybrid built using SDL2 and ImGui.

It features local network (LAN) multiplayer, meaning you and your friends will actually have to meet up in person to play. Sure, it was way easier for me to code it this way, but let's call it a feature: nowadays, the real "pandemic" is everyone just playing online from their own isolated rooms. At least this game forces you to actually hang out in the same place! PC and mobile devices seamlessly can play together.

---

## 📜 Credits & License
* Built using the **[SDL2 Library](https://www.libsdl.org/)** and **[Dear ImGui](https://github.com/ocornut/imgui)**.
* The font used in the application is located in the `assets/fonts/SN_Pro/` directory. Please refer to the specific license file included in that folder for usage rights.
* _GIF created using [VEED.io](https://www.veed.io/)._
* Created strictly for **learning purposes**

---

## 🛠️ Local Development Setup

Follow these steps to configure the source code and build the project on your machine using **Visual Studio Code**.

### 1. Clone the Repository
Open your terminal and run:
```bash
git clone https://github.com/davidCrejenovschi/Tic.Tac.Toe.git
cd Tic.Tac.Toe
```

### 2. Install Prerequisites (SDL2)
This project requires the **SDL2** library for graphics and window management. **ImGui** files are already included within the source code, but SDL2 must be installed on your system.

* **Windows:** Download the development libraries from the [official SDL2 website](https://www.libsdl.org/) or use a package manager like `vcpkg`. Make sure your compiler (e.g., MinGW or MSVC) can find the SDL2 include and lib folders.
* **Linux (Ubuntu/Debian):**
    ```bash
    sudo apt-get update
    sudo apt-get install libsdl2-dev
    ```
* **macOS (Homebrew):**
    ```bash
    brew install sdl2
    ```

### 3. Setup Visual Studio Code
1. Install the **C/C++** and **CMake Tools** extensions from the VS Code Marketplace.
2. Open the project folder in VS Code:
    ```bash
    code .
    ```

### 4. Configure and Build
1. Once opened, VS Code should detect the `CMakeLists.txt` file.
2. Open the Command Palette (`Ctrl+Shift+P` on Windows/Linux, `Cmd+Shift+P` on macOS) and run **CMake: Configure**.
3. Select your active compiler kit (e.g., GCC, Clang, or MSVC) when prompted.
4. Click the **Build** button in the bottom blue status bar (or press `F7`).

### 5. Run the Game
After a successful build, you can start the application directly from VS Code:
* Click the **Play/Launch** button in the bottom status bar (or press `Shift + F5`).
* Alternatively, run the generated executable from the integrated terminal:
    ```bash
    ./build/TicTacToe
    ```
*(Note: On Windows, it might be `.\build\Debug\TicTacToe.exe` depending on your CMake configuration).*

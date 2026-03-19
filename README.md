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

Follow these steps to configure the source code and build the project on your machine.

### 1. Clone the Repository
Open your terminal and run:
```bash
git clone [https://github.com/davidCrejenovschi/Tic.Tac.Toe.git](https://github.com/davidCrejenovschi/Tic.Tac.Toe.git)
cd Tic.Tac.Toe
```

### 2. Install Prerequisites (SDL2)
This project requires the **SDL2** library for graphics and window management. **ImGui** files are included within the source code, but SDL2 must be installed on your system.

* **Windows:** Download the development libraries from the [official SDL2 website](https://www.libsdl.org/) or use a package manager like `vcpkg`.
* **Linux (Ubuntu/Debian):**
    ```bash
    sudo apt-get update
    sudo apt-get install libsdl2-dev
    ```
* **macOS (Homebrew):**
    ```bash
    brew install sdl2
    ```

### 3. Build the Project
If the project uses **CMake**, run the following commands in the root directory:
```bash
mkdir build
cd build
cmake ..
cmake --build .
```
*(Alternatively, if you are using Visual Studio, open the `.sln` file and build the solution).*

### 4. Run the Executable
Once the compilation is finished, you can launch the game:

* **Windows:**
    ```bash
    .\TicTacToe.exe
    ```
* **Linux / macOS:**
    ```bash
    ./TicTacToe
    ```
---

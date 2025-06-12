# Assimp-FBX-OpenGL-Loader

### 🏫 Spring 2025 Computer Graphics Term Project

**Load, skin and render FBX models with skeletal animation in OpenGL via Assimp.**

[🇰🇷 Read in Korean](./README_KR.md)

## 🔑 Keywords  
OpenGL, FBX, Assimp, Loader, Skeletal Animation, C++, GLEW, FreeGLUT

---

## 🚀 Features
- 🔹 Load `.fbx` character models with embedded textures (via Assimp)
- 🔹 Parse bone hierarchy & skinning weights
- 🔹 Real-time skeletal animation playback
- 🔹 Camera rotation & animation speed control
- 🔹 Key-based dance animation switching (1→6 keys)

---

## ⚙️ Build & Run

### Requirements
- Visual Studio (tested on 2022)
- Assimp (via `vcpkg`, version 5.4.0 used)
- NuGet packages:
  - `glew`
  - `freeglut`
  - `glm`

### Build Steps
1. Clone the repository
2. Open `DanceAnimator.sln` in Visual Studio
3. Restore NuGet packages (Right-click Solution > Restore NuGet Packages)
4. Install Assimp via vcpkg:
   ```bash
   ./vcpkg install assimp
    ```
5. Run the project: Ctrl + F5


## 🎮 Controls

- **[1] → [6]** : Switch between 6 different dance animations  
- **← / → Arrow Keys** : Rotate the camera  
- **↑ / ↓ Arrow Keys** : Adjust animation playback speed  
- Current dance mode and speed are shown on screen

### 🕺 Dance Modes

| Key | Dance Name              |
|-----|-------------------------|
| 1   | Moonwalk                |
| 2   | Flare                   |
| 3   | Headspin                |
| 4   | Break Spin              |
| 5   | Mime                    |
| 6   | One-Hand Freeze Combo   |

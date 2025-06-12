# Assimp-FBX-OpenGL-Loader

**Load, skin and render FBX models with skeletal animation in OpenGL via Assimp.**

[🇰🇷 Read in Korean(한국어)](./README_KR.md)

<div align="center">
  <img src="https://github.com/user-attachments/assets/75c1f3c1-2bf7-458d-bd32-96314e74e6b5" alt="Project Thumbnail" width="600"/>
</div>


<br />

## 🔑 Keywords  
OpenGL, FBX, Assimp, Loader, Skeletal Animation, C++, GLEW, FreeGLUT, mixamo

<br />

## 🚀 Features

https://github.com/user-attachments/assets/c5bd6d7b-379e-4678-a750-4db8e5458892

031150845f2f

- 🔹 Load `.fbx` character models with embedded textures (via Assimp)
- 🔹 Parse bone hierarchy & skinning weights
- 🔹 Real-time skeletal animation playback
- 🔹 Camera rotation & animation speed control
- 🔹 Key-based dance animation switching (1→6 keys)

<br />

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

<br />

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


### Asset Notice

Character and animation assets are provided by Adobe Mixamo.This project is for non-commercial educational and demo purposes only.

⚠️ Redistribution of Mixamo assets as standalone files is prohibited.

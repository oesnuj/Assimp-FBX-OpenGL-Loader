# Assimp-FBX-OpenGL-Loader

### 컴퓨터 그래픽스 텀 프로젝트 (2025년 1학기)

**Assimp와 OpenGL을 활용하여 FBX 모델의 로딩, 스키닝, 그리고 골격 애니메이션 기능을 구현한 데모 프로젝트입니다.**

[🇺🇸 View in English](./README.md)

---

## ✨ 주요 기능
- `.fbx` 형식의 3D 캐릭터 모델 및 임베디드 텍스처 로드
- 본(Bone) 계층 구조 및 스키닝 가중치 파싱
- 실시간 골격 애니메이션 재생
- 키보드 입력으로 애니메이션 전환 및 속도 조절
- 카메라 시점 전환 기능 포함 (좌/우 방향키)

---

## ⚙️ 빌드 및 실행 방법

### 필수 환경
- Visual Studio 2022 이상
- `vcpkg`를 통한 Assimp 설치 (권장 버전: 5.4.0)
- NuGet 패키지:
  - `glew`
  - `glm`
  - `freeglut`

### 실행 절차
1. 레포지토리를 클론한 후, `DanceAnimator.sln`을 Visual Studio로 열기
2. NuGet 패키지 복원 (솔루션 우클릭 → "Restore NuGet Packages")
3. Assimp 설치 (vcpkg):
   ```bash
   ./vcpkg install assimp
   ```
4. Visual Studio에서 `Ctrl + F5`를 눌러 실행

---

## 🎮 조작 방법
- **[1] ~ [6]** : 다른 댄스 애니메이션으로 전환
- **← / → 방향키** : 카메라 좌우 회전
- **↑ / ↓ 방향키** : 애니메이션 재생 속도 조절
- 현재 선택된 댄스 모드와 속도는 화면 좌측 상단에 표시됨

### 댄스 목록
| 키 | 애니메이션 이름            |
|----|-----------------------------|
| 1  | 문워크 (Moonwalk)           |
| 2  | 플레어 (Flare)              |
| 3  | 헤드스핀 (Headspin)         |
| 4  | 브레이크 스핀 (Break Spin) |
| 5  | 마임 (Mime)                 |
| 6  | 한손 프리즈 콤보 (One-Hand Freeze Combo) |
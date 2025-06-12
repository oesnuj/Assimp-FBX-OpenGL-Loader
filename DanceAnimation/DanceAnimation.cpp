#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Model.h"
#include "Animator.h"
#include <assimp/version.h>
#include <iostream>

// --- Shader Sources ---
const char *vShader = R"(
#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoords;
layout(location = 3) in ivec4 inBoneIDs;
layout(location = 4) in vec4 inWeights;

uniform mat4 bones[100];
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;

void main() {
    vec4 weights = inWeights;
    mat4 boneTransform =
        weights.x * bones[inBoneIDs.x] +
        weights.y * bones[inBoneIDs.y] +
        weights.z * bones[inBoneIDs.z] +
        weights.w * bones[inBoneIDs.w];

    vec4 skinnedPos = boneTransform * vec4(inPos, 1.0);
    vec3 skinnedNorm = normalize(mat3(boneTransform) * inNormal);

    vec4 worldPos = model * skinnedPos;
    FragPos = vec3(worldPos);
    Normal = mat3(transpose(inverse(model))) * skinnedNorm;
    TexCoord = inTexCoords;
    gl_Position = projection * view * worldPos;
}
)";

const char *fShader = R"(
#version 330 core

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 ambient = 0.3 * lightColor;

    vec3 lighting = ambient + diffuse;
    vec4 texColor = texture(texture_diffuse1, TexCoord);
    FragColor = vec4(lighting * texColor.rgb, texColor.a);
}
)";

// --- Global Variables ---
unsigned int shader = 0;
Model *myModel = nullptr;
Animator *animator = nullptr;

// 상태 메시지
std::string statusMsg = "Dance Mode : 01";

// 창 크기
int windowWidth = 800;
int windowHeight = 600;

// 매트릭스 및 카메라
glm::mat4 projection;
glm::mat4 view;
glm::mat4 model;
float angle = 0.0f;

// 재생 속도
float playbackSpeed = 0.03f;

// --- Function Definitions ---

unsigned int createShaderProgram(const char *vSrc, const char *fSrc)
{
    unsigned int v = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v, 1, &vSrc, nullptr);
    glCompileShader(v);

    int success;
    char infoLog[512];
    glGetShaderiv(v, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(v, 512, nullptr, infoLog);
        std::cout << "Vertex Shader Error: " << infoLog << std::endl;
    }

    unsigned int f = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f, 1, &fSrc, nullptr);
    glCompileShader(f);
    glGetShaderiv(f, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(f, 512, nullptr, infoLog);
        std::cout << "Fragment Shader Error: " << infoLog << std::endl;
    }

    unsigned int prog = glCreateProgram();
    glAttachShader(prog, v);
    glAttachShader(prog, f);
    glLinkProgram(prog);
    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(prog, 512, nullptr, infoLog);
        std::cout << "Shader Program Error: " << infoLog << std::endl;
    }

    glDeleteShader(v);
    glDeleteShader(f);
    return prog;
}

void drawText(int x, int y, const std::string &text)
{
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(1.0f, 1.0f, 0.2f);
    glRasterPos2i(x, windowHeight - y);
    for (char c : text)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
}

void updateViewMatrix()
{
    float radius = 5.0f;
    float camX = sin(glm::radians(angle)) * radius;
    float camZ = cos(glm::radians(angle)) * radius;
    float camY = 2.0f;

    view = glm::lookAt(
        glm::vec3(camX, camY, camZ),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
}

void onSpecialKey(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_LEFT:
        std::cout << "[입력] ← 왼쪽 화살표 키 입력됨: 카메라 좌회전" << std::endl;
        angle -= 5.0f;
        updateViewMatrix();
        break;

    case GLUT_KEY_RIGHT:
        std::cout << "[입력] → 오른쪽 화살표 키 입력됨: 카메라 우회전" << std::endl;
        angle += 5.0f;
        updateViewMatrix();
        break;

    case GLUT_KEY_DOWN:
        playbackSpeed -= 0.01f;
        if (playbackSpeed < 0.01f)
            playbackSpeed = 0.01f;
        std::cout << "[입력] ↓ 아래 화살표 키 입력됨: 재생 속도 감소 → 현재 속도: " << playbackSpeed << std::endl;
        break;

    case GLUT_KEY_UP:
        playbackSpeed += 0.01f;
        if (playbackSpeed > 0.2f)
            playbackSpeed = 0.2f;
        std::cout << "[입력] ↑ 위 화살표 키 입력됨: 재생 속도 증가 → 현재 속도: " << playbackSpeed << std::endl;
        break;

    default:
        std::cout << "[경고] 인식되지 않은 특수키 입력됨. Key Code: " << key << std::endl;
    }

    glutPostRedisplay();
}

int currentDanceIndex = 0;
void onKeyboard(unsigned char key, int x, int y)
{
    if (key < '1' || key > '6')
        return;
    int idx = key - '1';

    // ✅ 같은 번호 누르면 무시
    if (idx == currentDanceIndex)
    {
        std::cout << "[Info] 같은 모드를 다시 눌렀습니다. 무시합니다." << std::endl;
        return;
    }

    std::string file = "assets/Dance0" + std::to_string(idx + 1) + ".fbx";

    delete animator;
    delete myModel;
    statusMsg.clear();

    myModel = new Model(file);
    Animation *clip = myModel->GetAnimation();
    if (!clip)
    {
        std::cerr << "[Error] No animation in " << file << std::endl;
        return;
    }
    animator = new Animator(clip, myModel->GetGlobalInverseTransform());
    statusMsg = "Dance Mode : 0" + std::to_string(idx + 1);
    currentDanceIndex = idx; // ✅ 현재 선택된 모드 업데이트
    std::cout << "[Debug] 댄스 모드 " << std::to_string(idx + 1) << "번이 선택되었습니다" << std::endl;
    std::cout << "[Debug] Reloaded " << file << std::endl;
    glutPostRedisplay();
}

void display()
{
    glClearColor(0.27f, 0.51f, 0.71f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader);
    GLint locTex = glGetUniformLocation(shader, "texture_diffuse1");
    if (locTex >= 0)
        glUniform1i(locTex, 0);

    glUniform3f(glGetUniformLocation(shader, "lightPos"), 5.0f, 10.0f, 5.0f);
    glUniform3f(glGetUniformLocation(shader, "viewPos"), 0.0f, 2.0f, 5.0f);
    glUniform3f(glGetUniformLocation(shader, "lightColor"), 1.0f, 1.0f, 1.0f);

    GLint locHas = glGetUniformLocation(shader, "hasDiffuseTexture");
    if (locHas >= 0)
        glUniform1i(locHas, 1);

    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &model[0][0]);

    if (myModel)
    {
        auto transforms = animator->GetFinalBoneMatrices();
        for (int i = 0; i < transforms.size(); ++i)
        {
            std::string name = "bones[" + std::to_string(i) + "]";
            int loc = glGetUniformLocation(shader, name.c_str());
            if (loc != -1)
                glUniformMatrix4fv(loc, 1, GL_FALSE, &transforms[i][0][0]);
            else
                std::cout << "[Warning] Uniform " << name << " not found!" << std::endl;
        }
        myModel->Draw(shader);
    }

    if (!statusMsg.empty())
    {
        std::string danceNum = statusMsg.substr(statusMsg.size() - 2);
        float speedRatio = playbackSpeed / 0.03f;

        char textBuf[128];
        snprintf(textBuf, sizeof(textBuf), "Dance Mode : %s | Speed : %.2fx", danceNum.c_str(), speedRatio);

        glUseProgram(0);
        drawText(10, 20, textBuf);
        glUseProgram(shader);
    }

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    windowWidth = w;
    windowHeight = h;
    glViewport(0, 0, w, h);
    projection = glm::perspective(glm::radians(45.0f), float(w) / h, 0.1f, 100.0f);
}

void idle()
{
    angle += 0.5f;
    if (animator)
    {
        float dt = 0.016f * playbackSpeed;
        animator->UpdateAnimation(dt);
    }
    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    std::cout << "Assimp Version: "
              << aiGetVersionMajor() << "."
              << aiGetVersionMinor() << "."
              << aiGetVersionRevision() << std::endl;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("202111602 김준서 컴퓨터 그래픽스 텀프로젝트");
    glewInit();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    myModel = new Model("assets/Dance01.fbx");
    std::cout << "[Debug] 댄스 모드  1번이 선택되었습니다." << std::endl;
    if (myModel)
    {
        // myModel->PrintInfo();
        animator = new Animator(myModel->GetAnimation());
    }

    shader = createShaderProgram(vShader, fShader);

    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    angle = 0.0f;
    updateViewMatrix();
    model = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f));

    glutSpecialFunc(onSpecialKey);
    glutKeyboardFunc(onKeyboard);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutMainLoop();

    delete myModel;
    return 0;
}

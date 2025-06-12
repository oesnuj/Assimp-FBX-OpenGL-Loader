#include "Model.h"
#include <iostream>
#include "stb_image.h"
#include "MatrixUtil.h"
#include "Animation.h"

void AddBoneData(Vertex& vertex, int boneID, float weight) {
    for (int i = 0; i < 4; ++i) {
        if (vertex.Weights[i] == 0.0f) {
            vertex.BoneIDs[i] = boneID;
            vertex.Weights[i] = weight;
            return;
        }
    }
    std::cout << "[WARN] Bone weight overflow on vertex!" << std::endl;
}



Model::Model(const std::string& path) {
    LoadModel(path);
}

Model::~Model() {
    for (auto mesh : meshes) {
        delete mesh;
    }
    if (animation) delete animation;
}

void Model::Draw(unsigned int shaderProgram) {
    //std::cout << "[Mesh::Draw] textures.size() = " << texturesLoaded.size() << std::endl;

    if (!texturesLoaded.empty()) {
        //std::cout << "[Mesh::Draw] First texture ID = " << texturesLoaded[0].ID << std::endl;
        //std::cout << "[Mesh::Draw] First texture type = " << texturesLoaded[0].type << std::endl;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texturesLoaded[0].ID);

        // 바인딩 후 확인
        GLint boundTex;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &boundTex);
        //std::cout << "[Mesh::Draw] After binding, BoundTexID = " << boundTex << std::endl;
    }
    else {
        std::cout << "[Mesh::Draw] No textures available!" << std::endl;
    }

    for (size_t i = 0; i < meshes.size(); i++) {
        if (i < materials.size()) {
            materials[i].Bind(shaderProgram);
        }
        meshes[i]->Draw();
        if (i < materials.size()) {
            materials[i].Unbind();
        }
    }
}

void Model::PrintInfo() {
    std::cout << "Model loaded with " << meshes.size() << " meshes:" << std::endl;
    for (size_t i = 0; i < meshes.size(); i++) {
        std::cout << "  Mesh " << i << ": ";
        meshes[i]->PrintInfo();
    }
}

void Model::LoadModel(const std::string& path) {
    Assimp::Importer importer;
    importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_LimitBoneWeights |
        aiProcess_GenNormals
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "Assimp Error: " << importer.GetErrorString() << std::endl;
        return;
    }
    //std::cout << "Animations: " << scene->mNumAnimations << std::endl;
    aiMatrix4x4 rootTransform = scene->mRootNode->mTransformation;
    glm::mat4 glmRootTransform = glm::mat4(
        rootTransform.a1, rootTransform.b1, rootTransform.c1, rootTransform.d1,
        rootTransform.a2, rootTransform.b2, rootTransform.c2, rootTransform.d2,
        rootTransform.a3, rootTransform.b3, rootTransform.c3, rootTransform.d3,
        rootTransform.a4, rootTransform.b4, rootTransform.c4, rootTransform.d4
    );
    globalInverseTransform = glm::inverse(glmRootTransform);
    //std::cout << "[Model] globalInverseTransform 계산 완료" << std::endl;


    //// 임베디드 텍스처 정보 출력
    //if (scene->mNumTextures > 0) {
    //    std::cout << "Found " << scene->mNumTextures << " embedded textures" << std::endl;
    //    for (unsigned int i = 0; i < scene->mNumTextures; i++) {
    //        std::cout << "  Embedded texture " << i << ": ";
    //        if (scene->mTextures[i]->mHeight == 0) {
    //            std::cout << "Compressed format, size: " << scene->mTextures[i]->mWidth << " bytes" << std::endl;
    //        }
    //        else {
    //            std::cout << "Raw format: " << scene->mTextures[i]->mWidth << "x" << scene->mTextures[i]->mHeight << std::endl;
    //        }
    //    }
    //}
    //else {
    //    std::cout << "No embedded textures found" << std::endl;
    //}

    // 디렉토리 경로 저장 (텍스처 로드용)
    directory = path.substr(0, path.find_last_of('/'));

    // scene 포인터 저장 (임베디드 텍스처 접근용)
    this->scene = scene;

    ProcessNode(scene->mRootNode, scene);

    // 마지막에 Animation 객체 생성
    if (scene->mNumAnimations > 0) {
        animation = new Animation(scene, this); // this는 Model*
    }
}

void Model::ProcessNode(aiNode* node, const aiScene* scene) {
    // 현재 노드의 모든 메시 처리
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(ProcessMesh(mesh, scene));
    }

    // 자식 노드들 재귀적으로 처리
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene);
    }
}

Mesh* Model::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // 정점 데이터 처리
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        // 위치
        vertex.Position = glm::vec3(
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        );

        // 법선
        if (mesh->HasNormals()) {
            vertex.Normal = glm::vec3(
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            );
        }
        else {
            vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
        }

        // 텍스처 좌표
        if (mesh->mTextureCoords[0]) {
            vertex.TexCoords = glm::vec2(
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            );
            //if (i < 5) { // 앞 5개만 출력
            //    std::cout << "Vertex " << i << " TexCoord: " << vertex.TexCoords.x << ", " << vertex.TexCoords.y << std::endl;
            //}
        }
        else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // 인덱스 데이터 처리
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    if (mesh->HasBones()) {
        //std::cout << "[ProcessMesh] 본 개수: " << mesh->mNumBones << std::endl;
        for (unsigned int i = 0; i < mesh->mNumBones; ++i) {
            aiBone* bone = mesh->mBones[i];
            std::string boneName(bone->mName.C_Str());

            int boneID = -1;
            if (boneNameToID.find(boneName) == boneNameToID.end()) {
                boneID = static_cast<int>(boneNameToID.size());
                boneNameToID[boneName] = boneID;

                glm::mat4 offsetMatrix;
                offsetMatrix = ConvertMatrixToGLMFormat(bone->mOffsetMatrix);

                boneOffsetMatrices.push_back(offsetMatrix);
            }
            else {
                boneID = boneNameToID[boneName];
            }

            for (unsigned int j = 0; j < bone->mNumWeights; ++j) {
                unsigned int vertexIndex = bone->mWeights[j].mVertexId;
                float weight = bone->mWeights[j].mWeight;
                AddBoneData(vertices[vertexIndex], boneID, weight);
            }
        }
    }





    // 머티리얼 처리
    Material material;
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

        // 머티리얼 색상 로드
        aiColor3D color(0.f, 0.f, 0.f);
        if (mat->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS)
            material.ambient = glm::vec3(color.r, color.g, color.b);
        if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
            material.diffuse = glm::vec3(color.r, color.g, color.b);
        if (mat->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
            material.specular = glm::vec3(color.r, color.g, color.b);

        float shininess;
        if (mat->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
            material.shininess = shininess;

        // 텍스처 로드
        std::vector<Texture> diffuseMaps = LoadMaterialTextures(mat, aiTextureType_DIFFUSE, "texture_diffuse", scene);
        material.diffuseTextures.insert(material.diffuseTextures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> specularMaps = LoadMaterialTextures(mat, aiTextureType_SPECULAR, "texture_specular", scene);
        material.specularTextures.insert(material.specularTextures.end(), specularMaps.begin(), specularMaps.end());

        std::vector<Texture> normalMaps = LoadMaterialTextures(mat, aiTextureType_HEIGHT, "texture_normal", scene);
        material.normalTextures.insert(material.normalTextures.end(), normalMaps.begin(), normalMaps.end());
    }

    materials.push_back(material);
    return new Mesh(vertices, indices);
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene) {
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        std::string texPath = str.C_Str();

        bool loaded = false;

        // (1) 임베디드 텍스처 직접 지정 ("*0" 형태)
        if (!texPath.empty() && texPath[0] == '*') {
            int idx = atoi(texPath.c_str() + 1);
            if (idx >= 0 && idx < (int)scene->mNumTextures) {
                Texture tex = LoadEmbeddedTexture(scene->mTextures[idx], typeName);
                tex.path = texPath;
                textures.push_back(tex);
                texturesLoaded.push_back(tex);
                loaded = true;
            }
        }

        // (2) Mixamo FBX: 경로가 파일명인데 파일은 없고, 임베디드만 존재할 때
        // LoadMaterialTextures 함수에서 텍스처 매핑 수정
        if (!loaded && scene->mNumTextures > 0) {
            // === 기존 매핑이 잘못되었을 수 있으니 다른 순서로 시도 ===
            int idx = -1;

            // 첫 번째 시도: 일반적인 순서
            if (typeName == "texture_diffuse") idx = 1;      // 컬러 텍스처는 1번
            else if (typeName == "texture_specular") idx = 0; // 그레이스케일은 0번
            else if (typeName == "texture_normal") idx = 3;
            else if (typeName == "texture_glossiness") idx = 2;

            // 만약 위가 안 되면 원래대로
            if (idx < 0 || idx >= (int)scene->mNumTextures) {
                if (typeName == "texture_diffuse") idx = 0;
                else if (typeName == "texture_specular") idx = 1;
                else if (typeName == "texture_normal") idx = 3;
                else if (typeName == "texture_glossiness") idx = 2;
            }

            if (idx >= 0 && idx < (int)scene->mNumTextures) {
                //std::cout << "[디버그] " << typeName << " → 임베디드 텍스처 " << idx << " 사용" << std::endl;
                Texture tex = LoadEmbeddedTexture(scene->mTextures[idx], typeName);
                tex.path = "*" + std::to_string(idx);
                textures.push_back(tex);
                texturesLoaded.push_back(tex);
                loaded = true;
            }
        }

        // (3) 외부 파일이 실제로 있으면 시도 (거의 필요 없음)
        if (!loaded) {
            std::string fullPath = directory + "/" + texPath;
            Texture tex(fullPath, typeName);
            if (tex.ID != 0) {
                textures.push_back(tex);
                texturesLoaded.push_back(tex);
                loaded = true;
            }
        }

        if (!loaded) {
            std::cout << "[경고] 텍스처를 찾지 못함: " << texPath << std::endl;
        }
    }
    return textures;
}
Texture Model::LoadEmbeddedTexture(const aiTexture* aiTex, const std::string& typeName) {
    Texture texture;
    texture.type = typeName;
    glGenTextures(1, &texture.ID);
    glBindTexture(GL_TEXTURE_2D, texture.ID);

    int width, height, nrChannels;
    unsigned char* data = nullptr;

    if (aiTex->mHeight == 0) {
        // 압축 이미지 데이터 (PNG, JPG 등)
        data = stbi_load_from_memory(
            reinterpret_cast<unsigned char*>(aiTex->pcData),
            aiTex->mWidth,
            &width, &height, &nrChannels, 0
        );
        //std::cout << "[임베디드] stbi_load_from_memory: data=" << (void*)data
        //    << ", w=" << width << ", h=" << height << ", c=" << nrChannels << std::endl;

        //// === 디버그 정보 추가 ===
        //printf("=== 임베디드 텍스처 디버그 (%s) ===\n", typeName.c_str());
        //printf("크기: %dx%d, 채널: %d\n", width, height, nrChannels);
        //if (data) {
        //    if (nrChannels == 1) {
        //        printf("첫 픽셀 (그레이): %d\n", data[0]);
        //        printf("샘플 픽셀들: %d, %d, %d, %d\n", data[0], data[100], data[200], data[300]);
        //    }
        //    else if (nrChannels >= 3) {
        //        printf("첫 픽셀 RGB: %d, %d, %d\n", data[0], data[1], data[2]);
        //        printf("100번째 픽셀 RGB: %d, %d, %d\n", data[300], data[301], data[302]);
        //    }
        //}
        //else {
        //    printf("데이터가 NULL입니다!\n");
        //}
        //printf("===============================\n");
    }
    else {
        // 원시 데이터
        width = aiTex->mWidth;
        height = aiTex->mHeight;
        nrChannels = 4;
        data = reinterpret_cast<unsigned char*>(aiTex->pcData);
        //std::cout << "[임베디드] RAW: data=" << (void*)data
          //  << ", w=" << width << ", h=" << height << ", c=4" << std::endl;
    }

    if (data && width > 0 && height > 0) {
        GLenum format = GL_RGB;
        if (nrChannels == 1) format = GL_RED;
        else if (nrChannels == 3) format = GL_RGB;
        else if (nrChannels == 4) format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        //std::cout << "glTexImage2D 성공: " << width << "x" << height << ", format=" << format << std::endl;

        if (aiTex->mHeight == 0) stbi_image_free(data);
    }
    else {
        std::cout << "임베디드 텍스처 로드 실패! (data==nullptr 또는 size 0)" << std::endl;
        unsigned char whitePixel[3] = { 255, 255, 255 };
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, whitePixel);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}





//#include "Model.h"
//#include <iostream>
//#include <filesystem>
//
//Model::Model(const std::string& path) {
//    LoadModel(path);
//}
//
//Model::~Model() {
//    for (auto mesh : meshes) {
//        delete mesh;
//    }
//}
//
//void Model::Draw(unsigned int shaderProgram) {
//    for (size_t i = 0; i < meshes.size(); i++) {
//        if (i < materials.size()) {
//            materials[i].Bind(shaderProgram);
//        }
//        meshes[i]->Draw();
//        if (i < materials.size()) {
//            materials[i].Unbind();
//        }
//    }
//}
//
//void Model::PrintInfo() {
//    std::cout << "Model loaded with " << meshes.size() << " meshes:" << std::endl;
//    for (size_t i = 0; i < meshes.size(); i++) {
//        std::cout << "  Mesh " << i << ": ";
//        meshes[i]->PrintInfo();
//    }
//}
//
//void Model::LoadModel(const std::string& path) {
//    Assimp::Importer importer;
//    const aiScene* scene = importer.ReadFile(path,
//        aiProcess_Triangulate |
//        aiProcess_FlipUVs |
//        aiProcess_LimitBoneWeights |
//        aiProcess_GenNormals
//    );
//
//    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
//        std::cout << "Assimp Error: " << importer.GetErrorString() << std::endl;
//        return;
//    }
//
//    // 디렉토리 경로 저장 (텍스처 로드용)
//    directory = path.substr(0, path.find_last_of('/'));
//
//    ProcessNode(scene->mRootNode, scene);
//}
//
//void Model::ProcessNode(aiNode* node, const aiScene* scene) {
//    // 현재 노드의 모든 메시 처리
//    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
//        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
//        meshes.push_back(ProcessMesh(mesh, scene));
//    }
//
//    // 자식 노드들 재귀적으로 처리
//    for (unsigned int i = 0; i < node->mNumChildren; i++) {
//        ProcessNode(node->mChildren[i], scene);
//    }
//}
//
//Mesh* Model::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
//    std::vector<Vertex> vertices;
//    std::vector<unsigned int> indices;
//
//    // 정점 데이터 처리
//    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
//        Vertex vertex;
//
//        // 위치
//        vertex.Position = glm::vec3(
//            mesh->mVertices[i].x,
//            mesh->mVertices[i].y,
//            mesh->mVertices[i].z
//        );
//
//        // 법선
//        if (mesh->HasNormals()) {
//            vertex.Normal = glm::vec3(
//                mesh->mNormals[i].x,
//                mesh->mNormals[i].y,
//                mesh->mNormals[i].z
//            );
//        }
//        else {
//            vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
//        }
//
//        // 텍스처 좌표
//        if (mesh->mTextureCoords[0]) {
//            vertex.TexCoords = glm::vec2(
//                mesh->mTextureCoords[0][i].x,
//                mesh->mTextureCoords[0][i].y
//            );
//        }
//        else {
//            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
//        }
//
//        vertices.push_back(vertex);
//    }
//
//    // 인덱스 데이터 처리
//    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
//        aiFace face = mesh->mFaces[i];
//        for (unsigned int j = 0; j < face.mNumIndices; j++) {
//            indices.push_back(face.mIndices[j]);
//        }
//    }
//
//    // 머티리얼 처리 (기본값 설정 추가)
//    Material material;
//
//    // 기본 머티리얼 값 설정
//    material.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
//    material.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
//    material.specular = glm::vec3(0.5f, 0.5f, 0.5f);
//    material.shininess = 32.0f;
//
//    if (mesh->mMaterialIndex >= 0) {
//        aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
//
//        // 머티리얼 색상 로드
//        aiColor3D color(0.f, 0.f, 0.f);
//        if (mat->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) {
//            material.ambient = glm::vec3(color.r, color.g, color.b);
//            // ambient가 너무 어두우면 기본값 사용
//            if (material.ambient.x < 0.1f && material.ambient.y < 0.1f && material.ambient.z < 0.1f) {
//                material.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
//            }
//        }
//
//        if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
//            material.diffuse = glm::vec3(color.r, color.g, color.b);
//            // diffuse가 검은색이면 기본값 사용
//            if (material.diffuse.x < 0.1f && material.diffuse.y < 0.1f && material.diffuse.z < 0.1f) {
//                material.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
//            }
//        }
//
//        if (mat->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
//            material.specular = glm::vec3(color.r, color.g, color.b);
//
//        float shininess;
//        if (mat->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
//            material.shininess = shininess;
//
//        // 텍스처 로드
//        std::vector<Texture> diffuseMaps = LoadMaterialTextures(mat, aiTextureType_DIFFUSE, "texture_diffuse");
//        material.diffuseTextures.insert(material.diffuseTextures.end(), diffuseMaps.begin(), diffuseMaps.end());
//
//        std::vector<Texture> specularMaps = LoadMaterialTextures(mat, aiTextureType_SPECULAR, "texture_specular");
//        material.specularTextures.insert(material.specularTextures.end(), specularMaps.begin(), specularMaps.end());
//
//        std::vector<Texture> normalMaps = LoadMaterialTextures(mat, aiTextureType_HEIGHT, "texture_normal");
//        material.normalTextures.insert(material.normalTextures.end(), normalMaps.begin(), normalMaps.end());
//    }
//
//    materials.push_back(material);
//    return new Mesh(vertices, indices);
//}
//
//std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
//    std::vector<Texture> textures;
//    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
//        aiString str;
//        mat->GetTexture(type, i, &str);
//
//        // 이미 로드된 텍스처인지 확인
//        bool skip = false;
//        for (unsigned int j = 0; j < texturesLoaded.size(); j++) {
//            if (std::strcmp(texturesLoaded[j].path.data(), str.C_Str()) == 0) {
//                textures.push_back(texturesLoaded[j]);
//                skip = true;
//                break;
//            }
//        }
//
//        if (!skip) {
//            std::string filename = std::string(str.C_Str());
//
//            // 파일명만 추출
//            size_t lastSlash = filename.find_last_of("/\\");
//            std::string justName = (lastSlash == std::string::npos) ? filename : filename.substr(lastSlash + 1);
//            // assets/에서 찾기
//            std::string fullPath = "assets/" + justName;
//
//            Texture texture(fullPath, typeName);
//            textures.push_back(texture);
//            texturesLoaded.push_back(texture);
//        }
//    }
//    return textures;
//}



//Model만 되었을때 v1 초기 버전

//#include "Model.h"
//#include <iostream>
//
//Model::Model(const std::string& path) {
//    LoadModel(path);
//}
//
//Model::~Model() {
//    for (auto mesh : meshes) {
//        delete mesh;
//    }
//}
//
//void Model::Draw(unsigned int shaderProgram) {
//    for (auto mesh : meshes) {
//        mesh->Draw();
//    }
//}
//
//void Model::PrintInfo() {
//    std::cout << "Model loaded with " << meshes.size() << " meshes:" << std::endl;
//    for (size_t i = 0; i < meshes.size(); i++) {
//        std::cout << "  Mesh " << i << ": ";
//        meshes[i]->PrintInfo();
//    }
//}
//
//void Model::LoadModel(const std::string& path) {
//    Assimp::Importer importer;
//    const aiScene* scene = importer.ReadFile(path,
//        aiProcess_Triangulate |
//        aiProcess_FlipUVs |
//        aiProcess_LimitBoneWeights |
//        aiProcess_GenNormals
//    );
//
//    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
//        std::cout << "Assimp Error: " << importer.GetErrorString() << std::endl;
//        return;
//    }
//
//    // 디렉토리 경로 저장 (텍스처 로드용)
//    directory = path.substr(0, path.find_last_of('/'));
//
//    ProcessNode(scene->mRootNode, scene);
//}
//
//void Model::ProcessNode(aiNode* node, const aiScene* scene) {
//    // 현재 노드의 모든 메시 처리
//    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
//        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
//        meshes.push_back(ProcessMesh(mesh, scene));
//    }
//
//    // 자식 노드들 재귀적으로 처리
//    for (unsigned int i = 0; i < node->mNumChildren; i++) {
//        ProcessNode(node->mChildren[i], scene);
//    }
//}
//
//Mesh* Model::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
//    std::vector<Vertex> vertices;
//    std::vector<unsigned int> indices;
//
//    // 정점 데이터 처리
//    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
//        Vertex vertex;
//
//        // 위치
//        vertex.Position = glm::vec3(
//            mesh->mVertices[i].x,
//            mesh->mVertices[i].y,
//            mesh->mVertices[i].z
//        );
//
//        // 법선
//        if (mesh->HasNormals()) {
//            vertex.Normal = glm::vec3(
//                mesh->mNormals[i].x,
//                mesh->mNormals[i].y,
//                mesh->mNormals[i].z
//            );
//        }
//        else {
//            vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
//        }
//
//        // 텍스처 좌표
//        if (mesh->mTextureCoords[0]) {
//            vertex.TexCoords = glm::vec2(
//                mesh->mTextureCoords[0][i].x,
//                mesh->mTextureCoords[0][i].y
//            );
//        }
//        else {
//            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
//        }
//
//        vertices.push_back(vertex);
//    }
//
//    // 인덱스 데이터 처리
//    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
//        aiFace face = mesh->mFaces[i];
//        for (unsigned int j = 0; j < face.mNumIndices; j++) {
//            indices.push_back(face.mIndices[j]);
//        }
//    }
//
//    return new Mesh(vertices, indices);
//}
//

    #pragma once
    #include <string>
    #include <GL/glew.h>

    class Texture {
    public:
        unsigned int ID;
        std::string type;
        std::string path;

        Texture();
        Texture(const std::string& filePath, const std::string& typeName);
        ~Texture();

        void Bind(unsigned int slot = 0);
        void Unbind();

        static unsigned int LoadTexture(const std::string& path);

        unsigned int GetID() const { return ID; }
    private:
        void Generate(const std::string& filePath);
    };
#include "precomp.h"
#include "common/AssetManager.h"

#include <fstream>
#include <stb_image/stb_image.h>

namespace AssetManager
{
    std::unordered_map<std::string, Texture> textures;
}

Texture AssetManager::LoadTexture(std::string const& inFilePath)
{
    Texture texture;

    if(const auto textureInMap = textures.find(inFilePath); textureInMap == textures.end())
    {
        if(DoesFileExist(inFilePath))
        {
            int width, height, channels = 0;
            std::vector<uint8_t> buffer = ReadBinaryFile(inFilePath);
            unsigned char* data = stbi_load_from_memory(buffer.data(), static_cast<int>(buffer.size()), &width, &height, &channels, 4);
            auto imageData = std::vector<uint8_t>(data, data + width * height * 4);
            texture = Texture(inFilePath, imageData, glm::ivec2(width, height));
            buffer.clear();
            imageData.clear();
            buffer.shrink_to_fit();
            imageData.shrink_to_fit();
            stbi_image_free(data);
            textures.emplace(std::make_pair(texture.GetPath(), texture));
        }
        else
        {
            printf("[AssetManager]: image %s does not exist. \n", inFilePath.c_str());
        }
    }
    else
    {
        texture = textureInMap->second;
    }

    return texture;  // NOLINT(clang-diagnostic-return-stack-address)
}

bool AssetManager::DoesFileExist(std::string const& inFilePath)
{
    std::ifstream readOnlyFile = std::ifstream();
    readOnlyFile.open(inFilePath);
    const bool isOpen = readOnlyFile.is_open();
    readOnlyFile.close();
    return isOpen;
}

std::vector<uint8_t> AssetManager::ReadBinaryFile(std::string const& inFilePath)
{
    std::vector<uint8_t> readBuffer = std::vector<uint8_t>(0);
    std::ifstream binaryFile = std::ifstream();
    binaryFile.open(inFilePath, std::ios::binary);

    if (binaryFile.is_open())
    {
        // Get file size
        binaryFile.seekg(0, std::ios::end);
        const long long fileSize = binaryFile.tellg();
        binaryFile.seekg(0, std::ios::beg);

        // Read binary file
        readBuffer = std::vector<uint8_t>(static_cast<size_t>(fileSize));
        binaryFile.read(reinterpret_cast<char*>(& *readBuffer.begin()), static_cast<std::streamsize>(fileSize));

        // Clean up
        binaryFile.close();
    }
    else
    {
        printf("[AssetManager]: Could not read file at %s \n", inFilePath.c_str());
    }

    return readBuffer;
}

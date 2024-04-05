#pragma once

struct Shader
{
	ComPtr<ID3DBlob> ShaderBlob;
};

class ShaderManager
{
public:
	ShaderManager() = default;

	void Init();

	uint32_t LoadShader(std::string inName, std::string inPath);
	uint32_t UseShader(std::string inName);
	Shader& GetShader(std::string inName);

private:
	std::unordered_map<std::string, Shader> m_shaderMap;
	std::string m_basePath;
};
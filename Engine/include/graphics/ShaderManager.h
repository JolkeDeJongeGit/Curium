#pragma once

struct Shader
{
	ComPtr<ID3DBlob> ShaderBlob;
};

class ShaderManager
{
public:
	ShaderManager() = default;

	static ShaderManager& Get()
	{
		static ShaderManager instance;
		return instance;
	}
	void Init();

	uint32_t LoadShader(const char* _name, const char* _path);
	uint32_t UseShader(const char* _name);
	Shader& GetShader(const char* _name);

private:
	std::unordered_map<const char*, Shader> m_shaderMap;
	std::string m_basePath;
};
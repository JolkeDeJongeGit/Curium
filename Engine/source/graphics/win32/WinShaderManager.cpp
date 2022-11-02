#include "precomp.h"
#include "graphics/ShaderManager.h"
#include <d3dcompiler.h>
void ShaderManager::Init()
{
}

uint32_t ShaderManager::LoadShader(const char* _name, const char* _path)
{
	Shader temp;

	auto value = std::string(_name) + ".cso";
	std::wstring wsTmp(value.begin(), value.end());
	auto value1 = wsTmp.c_str();
	D3DReadFileToBlob(value1, &temp.ShaderBlob);

	if (!temp.ShaderBlob)
	{
		printf("Failed in to load Shader %s, %s", _name, _path);
		return 0;
	}

	m_shaderMap.insert(std::pair<const char*, Shader>(_name, temp));
	return 1;
}

uint32_t ShaderManager::UseShader(const char* _name)
{
	return 1;
}

Shader& ShaderManager::GetShader(const char* _name)
{
	return m_shaderMap.at(_name);
}

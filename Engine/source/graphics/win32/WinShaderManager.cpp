#include "precomp.h"
#include "graphics/ShaderManager.h"
#include <d3dcompiler.h>
void ShaderManager::Init()
{
}

uint32_t ShaderManager::LoadShader(const char* _name, const char* _path)
{
	Shader temp;
	D3DReadFileToBlob((LPCWSTR)_path, &temp.ShaderBlob);

	if (!temp.ShaderBlob)
		printf("Failed in to load Shader %s, %s", _name, _path);

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

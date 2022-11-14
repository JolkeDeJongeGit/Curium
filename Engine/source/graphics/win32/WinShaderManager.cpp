#include "precomp.h"
#include "graphics/ShaderManager.h"
#include <d3dcompiler.h>
#include <filesystem>
#include <comdef.h>

void ShaderManager::Init()
{
	//https://gist.github.com/Jacob-Tate/7b326a086cf3f9d46e32315841101109
	wchar_t path[FILENAME_MAX] = { 0 };

	GetModuleFileNameW(nullptr, path, FILENAME_MAX);
	m_basePath = std::filesystem::path(path).parent_path().string();
}

uint32_t ShaderManager::LoadShader(const char* _name, const char* _path)
{
	Shader temp;
	auto value = m_basePath + "/" + std::string(_name) + ".cso";
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

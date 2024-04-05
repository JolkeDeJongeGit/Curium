#include "precomp.h"
#include "graphics/ShaderManager.h"
#include <d3dcompiler.h>
#include <filesystem>

void ShaderManager::Init()
{
	//https://gist.github.com/Jacob-Tate/7b326a086cf3f9d46e32315841101109
	wchar_t path[FILENAME_MAX] = { 0 };

	GetModuleFileNameW(nullptr, path, FILENAME_MAX);
	m_basePath = std::filesystem::path(path).parent_path().string();
}

uint32_t ShaderManager::LoadShader(std::string inName, std::string inPath)
{
	if (m_shaderMap.find(inName) != m_shaderMap.end())
		return 1;

	Shader temp;
	auto value = m_basePath + "/" + std::string(inName) + ".cso";
	const std::wstring wsTmp(value.begin(), value.end());
	const auto value1 = wsTmp.c_str();
	D3DReadFileToBlob(value1, &temp.ShaderBlob);

	if (!temp.ShaderBlob)
	{
		printf("Failed in to load Shader %s, %s", inName.c_str(), inPath.c_str());
		return 0;
	}

	m_shaderMap.insert(std::pair<std::string, Shader>(inName, temp));
	return 1;
}

uint32_t ShaderManager::UseShader(std::string inName)
{
	return 1;
}

Shader& ShaderManager::GetShader(std::string inName)
{
	return m_shaderMap.at(inName);
}

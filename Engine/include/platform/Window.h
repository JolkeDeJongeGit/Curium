#pragma once
class Window
{
public:
	virtual void Create(int _width, int _height) = 0;
	virtual void Update() = 0;
	virtual void Shutdown() = 0;
	virtual void SetTitle(const std::string& _title) { m_title = _title; }

	void SetWidth(const int _width) { m_width = _width; }
	void SetHeight(const int _height) { m_height = _height; ; }

	const int GetWidth() { return m_width; }
	const int GetHeight() { return m_height; }

	const std::string& GetTitle() { return m_title; }

	const bool IsActive() { return m_active; }
	void SetActive(const bool _active) { m_active = _active; }
protected:
	int m_width = 1920;
	int m_height = 1080;

	bool m_active = true;

	std::string m_title = "Curium Engine";
};
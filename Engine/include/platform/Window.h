#pragma once
class Window
{
public:
	virtual void Create(int inWidth, int inHeight);
	virtual void Update();
	virtual void Shutdown();
	virtual void SetTitle(const std::string& inTitle) { m_title = inTitle; }

	void SetWidth(const int inWidth) { m_width = inWidth; }
	void SetHeight(const int inHeight) { m_height = inHeight;  }

	[[nodiscard]] int GetWidth() const { return m_width; }
	[[nodiscard]] int GetHeight() const { return m_height; }

	const std::string& GetTitle() { return m_title; }

	[[nodiscard]] bool IsActive() const;
	void SetActive(const bool inActive) { m_active = inActive; }
protected:
	~Window() = default;
	int m_width = 1920;
	int m_height = 1080;

	bool m_active = true;

	std::string m_title = "Curium Engine";
};

inline void Window::Create(int inWidth, int inHeight)
{
}

inline void Window::Update()
{
}

inline void Window::Shutdown()
{
}

inline bool Window::IsActive() const { return m_active; }

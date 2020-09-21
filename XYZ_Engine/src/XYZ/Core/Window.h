#pragma once
#include "Event/EventManager.h"
#include <functional>

namespace XYZ {
	enum WindowFlags
	{
		NONE = 1 << 0,
		MAXIMIZED = 1 << 1,
		FULLSCREEN = 1 << 2
	};

	struct WindowProperties
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;
		int Flags;

		WindowProperties(const std::string& title = "Engine",
			unsigned int width = 1024,
			unsigned int height = 840,
			int flags = WindowFlags::NONE)
			: Title(title), Width(width), Height(height), Flags(flags)
		{
		}
	};

	class Window
	{
	public:
		virtual ~Window() = default;
		virtual void Update() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		// Window attributes
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsClosed() = 0;
		virtual void* GetNativeWindow() const = 0;

		static std::unique_ptr<Window> Create(const WindowProperties& props = WindowProperties());
	};

}
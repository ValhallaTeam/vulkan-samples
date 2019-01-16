#pragma once

#ifdef _WIN32
#include <Windows.h>
#elif defined __linux
#include <dlfcn.h>
#endif

#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <cstring>
#include <thread>
#include <cmath>
#include <functional>
#include <memory>
#include "assert.h"
#include "VulkanFunctions.h"

namespace VulkanSampleFramework
{
	// Vulkan library type
#ifdef _WIN32
#define LIBRARY_TYPE HMODULE
#elif defined __linux
#define LIBRARY_TYPE void*
#endif

	// OS-specific parameters
	struct WindowParameters
	{
#ifdef VK_USE_PLATFORM_WIN32_KHR

		HINSTANCE m_hInstance;
		HWND m_hWnd;

#elif defined VK_USE_PLATFORM_XLIB_KHR

		Display *m_Dpy;
		Window m_Window;

#elif defined VK_USE_PLATFORM_XCB_KHR

		xcb_connection_t *m_Connection;
		xcb_window_t m_Window;

#endif
	};

	// Extension availability check
	bool IsExtensionSupported(std::vector<VkExtensionProperties> const & availableExtensions, char const * const extension);

} // namespace VulkanCookbook

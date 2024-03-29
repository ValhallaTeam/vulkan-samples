#include "OS.h"
#include "VulkanSampleFramework.h"

namespace VulkanSampleFramework
{

#ifdef VK_USE_PLATFORM_WIN32_KHR
	namespace
	{
		enum UserMessage
		{
			USER_MESSAGE_RESIZE = WM_USER + 1,
			USER_MESSAGE_QUIT,
			USER_MESSAGE_MOUSE_CLICK,
			USER_MESSAGE_MOUSE_MOVE,
			USER_MESSAGE_MOUSE_WHEEL
		};
	}

	LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_LBUTTONDOWN:
			PostMessage(hWnd, USER_MESSAGE_MOUSE_CLICK, 0, 1);
			break;
		case WM_LBUTTONUP:
			PostMessage(hWnd, USER_MESSAGE_MOUSE_CLICK, 0, 0);
			break;
		case WM_RBUTTONDOWN:
			PostMessage(hWnd, USER_MESSAGE_MOUSE_CLICK, 1, 1);
			break;
		case WM_RBUTTONUP:
			PostMessage(hWnd, USER_MESSAGE_MOUSE_CLICK, 1, 0);
			break;
		case WM_MOUSEMOVE:
			PostMessage(hWnd, USER_MESSAGE_MOUSE_MOVE, LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_MOUSEWHEEL:
			PostMessage(hWnd, USER_MESSAGE_MOUSE_WHEEL, HIWORD(wParam), 0);
			break;
		case WM_SIZE:
		case WM_EXITSIZEMOVE:
			PostMessage(hWnd, USER_MESSAGE_RESIZE, wParam, lParam);
			break;
		case WM_KEYDOWN:
			if (VK_ESCAPE == wParam)
			{
				PostMessage(hWnd, USER_MESSAGE_QUIT, wParam, lParam);
			}
			break;
		case WM_CLOSE:
			PostMessage(hWnd, USER_MESSAGE_QUIT, wParam, lParam);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	}

	WindowFramework::WindowFramework(const char *windowTitle, int x, int y,	int width, int height, VulkanSampleBase &sample):
		m_WindowParams(),
		m_Sample(sample),
		m_Created(false)
	{
		m_WindowParams.m_hInstance = GetModuleHandle(nullptr);

		WNDCLASSEX window_class =
		{
			sizeof(WNDCLASSEX),					// UINT         cbSize
												/* Win 3.x */
			CS_HREDRAW | CS_VREDRAW,			// UINT         style
			WindowProcedure,					// WNDPROC      lpfnWndProc
			0,									// int          cbClsExtra
			0,									// int          cbWndExtra
			m_WindowParams.m_hInstance,			// HINSTANCE    hInstance
			nullptr,							// HICON        hIcon
			LoadCursor(nullptr, IDC_ARROW),		// HCURSOR      hCursor
			(HBRUSH)(COLOR_WINDOW + 1),			// HBRUSH       hbrBackground
			nullptr,							// LPCSTR       lpszMenuName
			"VulkanSample",						// LPCSTR       lpszClassName
												/* Win 4.0 */
			nullptr								// HICON        hIconSm
		};

		if (!RegisterClassEx(&window_class))
		{
			return;
		}

		m_WindowParams.m_hWnd = CreateWindow("VulkanSample", windowTitle, WS_OVERLAPPEDWINDOW, x, y, width, height, nullptr, nullptr, m_WindowParams.m_hInstance, nullptr);
		if (!m_WindowParams.m_hWnd)
		{
			return;
		}

		m_Created = true;
	}

	WindowFramework::~WindowFramework()
	{
		if (m_WindowParams.m_hWnd)
		{
			DestroyWindow(m_WindowParams.m_hWnd);
		}

		if (m_WindowParams.m_hInstance) {
			UnregisterClass("VulkanCookbook", m_WindowParams.m_hInstance);
		}
	}

	void WindowFramework::Render()
	{
		if (m_Created && m_Sample.Initialize(m_WindowParams))
		{

			ShowWindow(m_WindowParams.m_hWnd, SW_SHOWNORMAL);
			UpdateWindow(m_WindowParams.m_hWnd);

			MSG message;
			bool loop = true;

			while (loop)
			{
				if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) 
				{
					switch (message.message)
					{
					case USER_MESSAGE_MOUSE_CLICK:
						m_Sample.MouseClick(static_cast<size_t>(message.wParam), message.lParam > 0);
						break;
					case USER_MESSAGE_MOUSE_MOVE:
						m_Sample.MouseMove(static_cast<int>(message.wParam), static_cast<int>(message.lParam));
						break;
					case USER_MESSAGE_MOUSE_WHEEL:
						m_Sample.MouseWheel(static_cast<short>(message.wParam) * 0.002f);
						break;
					case USER_MESSAGE_RESIZE:
						if (!m_Sample.Resize())
						{
							loop = false;
						}
						break;
					case USER_MESSAGE_QUIT:
						loop = false;
						break;
					}
					TranslateMessage(&message);
					DispatchMessage(&message);
				}
				else
				{
					if (m_Sample.IsReady())
					{
						m_Sample.UpdateTime();
						m_Sample.Draw();
						m_Sample.MouseReset();
					}
				}
			}
		}

		m_Sample.Deinitialize();
	}

#endif

} // namespace VulkanCookbook

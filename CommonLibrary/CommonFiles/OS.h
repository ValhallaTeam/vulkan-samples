#pragma once
#ifdef _WIN32
#include <Windows.h>
#endif

#include "Common.h"

namespace VulkanSampleFramework
{
	class VulkanSampleBase;

	// Window managemenet class
	class WindowFramework
	{
	public:
		WindowFramework(const char *windowTitle, int x, int y, int width, int height, VulkanSampleBase &sample);
		virtual ~WindowFramework();
		virtual void Render() final;

	private:
		WindowParameters m_WindowParams;
		VulkanSampleBase &m_Sample;
		bool m_Created;
	};


}


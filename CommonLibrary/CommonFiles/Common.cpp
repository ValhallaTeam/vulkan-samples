#include "Common.h"

namespace VulkanSampleFramework
{
	bool IsExtensionSupported(std::vector<VkExtensionProperties> const & availableExtensions, char const *const  extension)
	{
		for (auto & availableExtension : availableExtensions)
		{
			if (strstr(availableExtension.extensionName, extension))
			{
				return true;
			}
		}
		return false;
	}
}



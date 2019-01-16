#include "InstanceAndDevice.h"

namespace VulkanSampleFramework
{
	bool ConnectWithVulkanLoaderLibrary(LIBRARY_TYPE &vulkanLibrary)
	{
#if defined _WIN32
		vulkanLibrary = LoadLibrary("vulkan-1.dll");
#elif defined __linux
		vulkan_library = dlopen("libvulkan.so.1", RTLD_NOW);
#endif

		if (vulkanLibrary == nullptr)
		{
			std::cout << "Could not connect with a Vulkan Runtime library." << std::endl;
			return false;
		}
		return true;
	}

	bool LoadFunctionExportedFromVulkanLoaderLibrary(LIBRARY_TYPE const & vulkanLibrary)
	{
#if defined _WIN32
#define LoadFunction GetProcAddress
#elif defined __linux
#define LoadFunction dlsym
#endif

#define EXPORTED_VULKAN_FUNCTION( name )									\
		name = (PFN_##name)LoadFunction( vulkanLibrary, #name );			\
		if( name == nullptr )												\
		{																	\
			std::cout << "Could not load exported Vulkan function named: "  \
			#name << std::endl;												\
			return false;													\
		}

#include "../CommonFiles//ListOfVulkanFunctions.inl"

		return true;
	}

	bool LoadGlobalLevelFunctions()
	{
#define GLOBAL_LEVEL_VULKAN_FUNCTION( name )								\
    name = (PFN_##name)vkGetInstanceProcAddr( nullptr, #name );				\
    if( name == nullptr )													\
	{																		\
      std::cout << "Could not load global level Vulkan function named: "	\
        #name << std::endl;													\
      return false;															\
    }

#include "../CommonFiles//ListOfVulkanFunctions.inl"

		return true;
	}

	bool CheckAvailableInstanceExtensions(std::vector<VkExtensionProperties> & availableExtensions)
	{
		uint32_t extensionsCount = 0;
		VkResult result = VK_SUCCESS;

		result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr);
		if ((result != VK_SUCCESS) || (extensionsCount == 0))
		{
			std::cout << "Could not get the number of instance extensions." << std::endl;
			return false;
		}

		availableExtensions.resize(extensionsCount);
		result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, availableExtensions.data());
		if ((result != VK_SUCCESS) || (extensionsCount == 0))
		{
			std::cout << "Could not enumerate instance extensions." << std::endl;
			return false;
		}

		return true;
	}

	bool CreateVulkanInstance(std::vector<char const *> const &desiredExtensions, char const *const applicationName, VkInstance &instance)
	{
		std::vector<VkExtensionProperties> availableExtensions;
		if (!CheckAvailableInstanceExtensions(availableExtensions))
		{
			return false;
		}

		for (auto & extension : desiredExtensions)
		{
			if (!IsExtensionSupported(availableExtensions, extension))
			{
				std::cout << "Extension named '" << extension << "' is not supported by an Instance object." << std::endl;
				return false;
			}
		}

		VkApplicationInfo applicationInfo =
		{
			VK_STRUCTURE_TYPE_APPLICATION_INFO,						// VkStructureType           sType
			nullptr,												// const void              * pNext
			applicationName,										// const char              * pApplicationName
			VK_MAKE_VERSION(1, 0, 0),								// uint32_t                  applicationVersion
			"Vulkan Sample",										// const char              * pEngineName
			VK_MAKE_VERSION(1, 0, 0),								// uint32_t                  engineVersion
			VK_MAKE_VERSION(1, 0, 0)								// uint32_t                  apiVersion
		};

		VkInstanceCreateInfo instanceCreateInfo =
		{
			VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,             // VkStructureType           sType
			nullptr,                                            // const void              * pNext
			0,                                                  // VkInstanceCreateFlags     flags
			&applicationInfo,                                  // const VkApplicationInfo * pApplicationInfo
			0,                                                  // uint32_t                  enabledLayerCount
			nullptr,                                            // const char * const      * ppEnabledLayerNames
			static_cast<uint32_t>(desiredExtensions.size()),   // uint32_t                  enabledExtensionCount
			desiredExtensions.data()                           // const char * const      * ppEnabledExtensionNames
		};

		VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
		if ((result != VK_SUCCESS) || (instance == VK_NULL_HANDLE))
		{
			std::cout << "Could not create Vulkan instance." << std::endl;
			return false;
		}

		return true;
	}

	bool LoadInstanceLevelFunctions(VkInstance instance, std::vector<char const *> const & enabledExtensions)
	{
	// Load core Vulkan API instance-level functions
	#define INSTANCE_LEVEL_VULKAN_FUNCTION( name )                                  \
		name = (PFN_##name)vkGetInstanceProcAddr( instance, #name );                \
		if( name == nullptr )														\
		{																			\
			std::cout << "Could not load instance-level Vulkan function named: "	\
			#name << std::endl;                                                     \
			return false;                                                           \
		}

		// Load instance-level functions from enabled extensions
	#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( name, extension )				\
		for( auto & enabledExtension : enabledExtensions )									\
		{																					\
			if( std::string( enabledExtension ) == std::string( extension ) )				\
			{																				\
				name = (PFN_##name)vkGetInstanceProcAddr( instance, #name );				\
				if( name == nullptr )														\
				{																			\
					std::cout << "Could not load instance-level Vulkan function named: "	\
					#name << std::endl;														\
					return false;															\
				}																			\
			}																				\
		 }

	#include "../CommonFiles/ListOfVulkanFunctions.inl"

		return true;
	}

	bool EnumerateAvailablePhysicalDevices(VkInstance instance,	std::vector<VkPhysicalDevice> & availableDevices)
	{
		uint32_t devicesCount = 0;
		VkResult result = VK_SUCCESS;

		result = vkEnumeratePhysicalDevices(instance, &devicesCount, nullptr);
		if ((result != VK_SUCCESS) || (devicesCount == 0))
		{
			std::cout << "Could not get the number of available physical devices." << std::endl;
			return false;
		}

		availableDevices.resize(devicesCount);
		result = vkEnumeratePhysicalDevices(instance, &devicesCount, availableDevices.data());
		if ((result != VK_SUCCESS) || (devicesCount == 0))
		{
			std::cout << "Could not enumerate physical devices." << std::endl;
			return false;
		}

		return true;
	}

	bool CheckAvailableDeviceExtensions(VkPhysicalDevice  physicalDevice, std::vector<VkExtensionProperties> & availableExtensions)
	{
		uint32_t extensionsCount;
		VkResult result = VK_SUCCESS;

		result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionsCount, nullptr);
		if ((result != VK_SUCCESS) || (extensionsCount == 0))
		{
			std::cout << "Could not get the number of device extensions." << std::endl;
			return false;
		}

		availableExtensions.resize(extensionsCount);
		result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionsCount, availableExtensions.data());
		if ((result != VK_SUCCESS) ||(extensionsCount == 0))
		{
			std::cout << "Could not enumerate device extensions." << std::endl;
			return false;
		}

		return true;
	}

	void GetFeaturesAndPropertiesOfPhysicalDevice(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures &deviceFeatures,
		VkPhysicalDeviceProperties & deviceProperties, VkPhysicalDeviceMemoryProperties &memoryPropertoes)
	{
		vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
		vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryPropertoes);
	}

	bool CheckAvailableQueueFamiliesAndTheirProperties(VkPhysicalDevice physicalDevice, std::vector<VkQueueFamilyProperties> &queueFamiliesProperties)
	{
		uint32_t queueFamiliesCount = 0;

		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, nullptr);
		if (queueFamiliesCount == 0)
		{
			std::cout << "Could not get the number of queue families." << std::endl;
			return false;
		}

		queueFamiliesProperties.resize(queueFamiliesCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, queueFamiliesProperties.data());
		if (queueFamiliesCount == 0)
		{
			std::cout << "Could not acquire properties of queue families." << std::endl;
			return false;
		}

		return true;
	}

	bool SelectIndexOfQueueFamilyWithDesiredCapabilities(VkPhysicalDevice physicalDevice, VkQueueFlags desiredCapabilities, 
		std::vector<VkQueueFamilyProperties> &queueFamiliesProperties, uint32_t &queueFamilyIndex)
	{
		for (uint32_t index = 0; index < static_cast<uint32_t>(queueFamiliesProperties.size()); ++index)
		{
			if ((queueFamiliesProperties[index].queueCount > 0) && ((queueFamiliesProperties[index].queueFlags & desiredCapabilities) == desiredCapabilities))
			{
				queueFamilyIndex = index;
				return true;
			}
		}
		return false;
	}

	bool CreateLogicalDevice(VkPhysicalDevice physicalDevice, std::vector<QueueInfo> queueInfos, std::vector<char const *> const &desiredExtennsions,
		VkPhysicalDeviceFeatures *desiredFeatures, VkDevice &logicalDeevice)
	{
		std::vector<VkExtensionProperties> availableExtensions;
		if (!CheckAvailableDeviceExtensions(physicalDevice, availableExtensions))
		{
			return false;
		}

		for (auto & extension : desiredExtennsions)
		{
			if (!IsExtensionSupported(availableExtensions, extension))
			{
				std::cout << "Extension named '" << extension << "' is not supported by a physical device." << std::endl;
				return false;
			}
		}

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		for (auto &info : queueInfos)
		{
			queueCreateInfos.push_back
			(
				{
					VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,			// VkStructureType                  sType
					nullptr,											// const void						*pNext
					0,													// VkDeviceQueueCreateFlags         flags
					info.m_FamilyIndex,                                 // uint32_t                         queueFamilyIndex
					static_cast<uint32_t>(info.m_Priorities.size()),    // uint32_t                         queueCount
					info.m_Priorities.data()                            // const float						*pQueuePriorities
				}
			);
		};

		VkDeviceCreateInfo deviceCreateInfo =
		{
			VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,               // VkStructureType                  sType
			nullptr,                                            // const void						*pNext
			0,                                                  // VkDeviceCreateFlags              flags
			static_cast<uint32_t>(queueCreateInfos.size()),		// uint32_t                         queueCreateInfoCount
			queueCreateInfos.data(),							// const VkDeviceQueueCreateInfo	*pQueueCreateInfos
			0,                                                  // uint32_t                         enabledLayerCount
			nullptr,                                            // const char * const				*ppEnabledLayerNames
			static_cast<uint32_t>(desiredExtennsions.size()),   // uint32_t                         enabledExtensionCount
			desiredExtennsions.data(),                          // const char * const				*ppEnabledExtensionNames
			desiredFeatures										// const VkPhysicalDeviceFeatures	*pEnabledFeatures
		};

		VkResult result = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &logicalDeevice);
		if ((result != VK_SUCCESS) || (logicalDeevice == VK_NULL_HANDLE))
		{
			std::cout << "Could not create logical device." << std::endl;
			return false;
		}

		return true;
	}

	bool LoadDeviceLevelFunctions(VkDevice logicalDevice, std::vector<char const *> const &enabledExtensions)
	{
		// Load core Vulkan API device-level functions
	#define DEVICE_LEVEL_VULKAN_FUNCTION(name)										\
		name = (PFN_##name)vkGetDeviceProcAddr(logicalDevice, #name);				\
		if( name == nullptr )														\
		{																			\
			std::cout << "Could not load device-level Vulkan function named: "      \
			#name << std::endl;                                                     \
			return false;                                                           \
		}

		// Load device-level functions from enabled extensions
	#define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension)					\
		for( auto & enabledExtension : enabledExtensions)									\
		{																					\
			if(std::string(enabledExtension) == std::string(extension))						\
			{																				\
				name = (PFN_##name)vkGetDeviceProcAddr( logicalDevice, #name );				\
				if( name == nullptr )														\
				{																			\
					std::cout << "Could not load device-level Vulkan function named: "		\
					#name << std::endl;														\
					return false;															\
				}																			\
			}																				\
		}

		#include "../CommonFiles//ListOfVulkanFunctions.inl"
		return true;
	}

	void GetDeviceQueue(VkDevice logicalDevice, uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue &queue)
	{
		vkGetDeviceQueue(logicalDevice, queueFamilyIndex, queueIndex, &queue);
	}

	void DestroyLogicalDevice(VkDevice &logicalDevice)
	{
		if (logicalDevice)
		{
			vkDestroyDevice(logicalDevice, nullptr);
			logicalDevice = VK_NULL_HANDLE;
		}
	}

	void DestroyVulkanInstance(VkInstance &instance) 
	{
		if (instance)
		{
			vkDestroyInstance(instance, nullptr);
			instance = VK_NULL_HANDLE;
		}
	}

	void ReleaseVulkanLoaderLibrary(LIBRARY_TYPE &vulkanLibrary)
	{
		if (nullptr != vulkanLibrary)
		{
#if defined _WIN32
			FreeLibrary(vulkanLibrary);
#elif defined __linux
			dlclose(vulkanLibrary);
#endif
			vulkanLibrary = nullptr;
		}
	}
}
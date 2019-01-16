#pragma once
#include "../CommonFiles/Common.h"

namespace VulkanSampleFramework
{
	struct QueueInfo
	{
		uint32_t           m_FamilyIndex;
		std::vector<float> m_Priorities;
	};

	bool ConnectWithVulkanLoaderLibrary(LIBRARY_TYPE &vulkanLibrary);
	bool LoadFunctionExportedFromVulkanLoaderLibrary(LIBRARY_TYPE const & vulkanLibrary);
	bool LoadGlobalLevelFunctions();
	bool CheckAvailableInstanceExtensions(std::vector<VkExtensionProperties> & availableExtensions);
	bool CreateVulkanInstance(std::vector<char const *> const &desiredExtensions, char const *const applicationName, VkInstance &instance);
	bool LoadInstanceLevelFunctions(VkInstance instance, std::vector<char const *> const &enabledExtensions);
	bool EnumerateAvailablePhysicalDevices(VkInstance instance, std::vector<VkPhysicalDevice> &availableDevices);
	bool CheckAvailableDeviceExtensions(VkPhysicalDevice  physicalDevice, std::vector<VkExtensionProperties> &availableExtensions);
	void GetFeaturesAndPropertiesOfPhysicalDevice(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures &deviceFeatures,
		VkPhysicalDeviceProperties &deviceProperties, VkPhysicalDeviceMemoryProperties &memoryPropertoes);
	bool CheckAvailableQueueFamiliesAndTheirProperties(VkPhysicalDevice physicalDevice, std::vector<VkQueueFamilyProperties> &queueFamiliesProperties);
	bool SelectIndexOfQueueFamilyWithDesiredCapabilities(VkPhysicalDevice physicalDevice, VkQueueFlags desiredCapabilities,
		std::vector<VkQueueFamilyProperties> &queueFamiliesProperties, uint32_t &queueFamilyIndex);
	bool CreateLogicalDevice(VkPhysicalDevice physicalDevice, std::vector<QueueInfo> queueInfos, std::vector<char const *> const &desiredExtennsions,
		VkPhysicalDeviceFeatures *desiredFeatures, VkDevice &logicalDeevice);
	bool LoadDeviceLevelFunctions(VkDevice logicalDevice, std::vector<char const *> const &enabledExtensions);
	void GetDeviceQueue(VkDevice logicalDevice, uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue &queue);
	void DestroyLogicalDevice(VkDevice &logicalDevice);
	void DestroyVulkanInstance(VkInstance &instance);
	void ReleaseVulkanLoaderLibrary(LIBRARY_TYPE &vulkanLibrary);
}

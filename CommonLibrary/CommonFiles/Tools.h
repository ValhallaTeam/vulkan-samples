#pragma once
#include "Common.h"

namespace VulkanSampleFramework
{
	struct Mesh
	{
		std::vector<float> m_Data;

		struct Part
		{
			uint32_t  m_VertexOffset;
			uint32_t  m_VertexCount;
		};

		std::vector<Part> m_Parts;
	};

	using Vector3 = std::array<float, 3>;
	using Matrix4x4 = std::array<float, 16>;

	bool GetBinaryFileContents(std::string const &fileName, std::vector<unsigned char> &contents);
	bool SaveBinaryFileContents(std::string const &fileName, std::vector<unsigned char> &contents);
	bool Load3DModelFromObjFile(char const *filename, bool loadNormals, bool loadTexcoords, bool generateTangentSpaceVectors, bool unify, Mesh &mesh, uint32_t *vertexStride = nullptr);
	bool LoadTextureDataFromFile(char const *filename, int numRequestedComponents, std::vector<unsigned char> &imageData, int *imageWidth, int * imageHeight, int * imageNumComponents,
		int *imageDataSize);
	Matrix4x4 PrepareRotationMatrix(float angle, Vector3 const &axis, float normalizeAxis = false);
	Matrix4x4 PreparePerspectiveProjectionMatrix(float aspectRatio, float fieldOfView, float nearPlane, float farPlane);
	
	float Deg2Rad(float value);

	float Dot(Vector3 const & left,	Vector3 const &right);

	Vector3 Cross(Vector3 const & left,	Vector3 const &right);

	Vector3 Normalize(Vector3 const & vector);

	Vector3 operator+ (Vector3 const & left, Vector3 const &right);

	Vector3 operator- (Vector3 const & left,  Vector3 const &right);

	Vector3 operator+ (float const &left, Vector3 const &right);

	Vector3 operator- (float const &left, Vector3 const &right);

	Vector3 operator+ (Vector3 const &left, float const &right);

	Vector3 operator- (Vector3 const &left, float const &right);

	Vector3 operator* (float left, Vector3 const &right);

	Vector3 operator* (Vector3 const &left,	float right);

	Vector3 operator* (Vector3 const &left,	Matrix4x4 const & right);

	Vector3 operator- (Vector3 const & vector);

	bool operator== (Vector3 const &left, Vector3 const & right);

	Matrix4x4 operator* (Matrix4x4 const &left,	Matrix4x4 const & right);

}

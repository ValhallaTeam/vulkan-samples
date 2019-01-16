#include "Tools.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../External/tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../External/stb_image.h"

namespace VulkanSampleFramework
{
	namespace 
	{
		// Based on:
		// Lengyel, Eric. "Computing Tangent Space Basis Vectors for an Arbitrary Mesh". Terathon Software 3D Graphics Library, 2001.
		// http://www.terathon.com/code/tangent.html

		void CalculateTangentAndBitangent(float const *normalData, Vector3 const &faceTangent, Vector3 const &faceBitangent, float *tangentData, float *bitangentData)
		{
			// Gram-Schmidt orthogonalize
			Vector3 const normal = { normalData[0], normalData[1], normalData[2] };
			Vector3 const tangent = Normalize(faceTangent - normal * Dot(normal, faceTangent));

			// Calculate handedness
			float handedness = (Dot(Cross(normal, tangent), faceTangent) < 0.0f) ? -1.0f : 1.0f;

			Vector3 const bitangent = handedness * Cross(normal, tangent);

			tangentData[0] = tangent[0];
			tangentData[1] = tangent[1];
			tangentData[2] = tangent[2];

			bitangentData[0] = bitangent[0];
			bitangentData[1] = bitangent[1];
			bitangentData[2] = bitangent[2];
		}

		void GenerateTangentSpaceVectors(Mesh & mesh)
		{
			size_t const normalOffset = 3;
			size_t const texcoordOffset = 6;
			size_t const tangentOffset = 8;
			size_t const bitangentOffset = 11;
			size_t const stride = bitangentOffset + 3;

			for (auto & part : mesh.m_Parts)
			{
				for (size_t i = 0; i < mesh.m_Data.size(); i += stride * 3)
				{
					size_t i1 = i;
					size_t i2 = i1 + stride;
					size_t i3 = i2 + stride;
					Vector3 const v1 = { mesh.m_Data[i1], mesh.m_Data[i1 + 1], mesh.m_Data[i1 + 2] };
					Vector3 const v2 = { mesh.m_Data[i2], mesh.m_Data[i2 + 1], mesh.m_Data[i2 + 2] };
					Vector3 const v3 = { mesh.m_Data[i3], mesh.m_Data[i3 + 1], mesh.m_Data[i3 + 2] };

					std::array<float, 2> const w1 = { mesh.m_Data[i1 + texcoordOffset], mesh.m_Data[i1 + texcoordOffset + 1] };
					std::array<float, 2> const w2 = { mesh.m_Data[i2 + texcoordOffset], mesh.m_Data[i2 + texcoordOffset + 1] };
					std::array<float, 2> const w3 = { mesh.m_Data[i3 + texcoordOffset], mesh.m_Data[i3 + texcoordOffset + 1] };

					float x1 = v2[0] - v1[0];
					float x2 = v3[0] - v1[0];
					float y1 = v2[1] - v1[1];
					float y2 = v3[1] - v1[1];
					float z1 = v2[2] - v1[2];
					float z2 = v3[2] - v1[2];

					float s1 = w2[0] - w1[0];
					float s2 = w3[0] - w1[0];
					float t1 = w2[1] - w1[1];
					float t2 = w3[1] - w1[1];

					float r = 1.0f / (s1 * t2 - s2 * t1);
					Vector3 faceTangent = { (t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r };
					Vector3 faceBitangent = { (s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r };

					CalculateTangentAndBitangent(&mesh.m_Data[i1 + normalOffset], faceTangent, faceBitangent, &mesh.m_Data[i1 + tangentOffset], &mesh.m_Data[i1 + bitangentOffset]);
					CalculateTangentAndBitangent(&mesh.m_Data[i2 + normalOffset], faceTangent, faceBitangent, &mesh.m_Data[i2 + tangentOffset], &mesh.m_Data[i2 + bitangentOffset]);
					CalculateTangentAndBitangent(&mesh.m_Data[i3 + normalOffset], faceTangent, faceBitangent, &mesh.m_Data[i3 + tangentOffset], &mesh.m_Data[i3 + bitangentOffset]);
				}
			}
		}
	}

	bool GetBinaryFileContents(std::string const &fileName,	std::vector<unsigned char> & contents)
	{
		contents.clear();

		std::ifstream file(fileName, std::ios::binary);
		if (file.fail())
		{
			std::cout << "Could not open '" << fileName << "' file." << std::endl;
			return false;
		}

		std::streampos begin;
		std::streampos end;
		begin = file.tellg();
		file.seekg(0, std::ios::end);
		end = file.tellg();

		if ((end - begin) == 0)
		{
			std::cout << "The '" << fileName << "' file is empty." << std::endl;
			return false;
		}
		contents.resize(static_cast<size_t>(end - begin));
		file.seekg(0, std::ios::beg);
		file.read(reinterpret_cast<char*>(contents.data()), end - begin);
		file.close();

		return true;
	}

	bool SaveBinaryFileContents(std::string const &fileName, std::vector<unsigned char> &contents)
	{
		std::ofstream file(fileName, std::ios::binary | std::ofstream::out);
		if (file.fail())
		{
			std::cout << "Could not open '" << fileName << "' file." << std::endl;
			return false;
		}

		file.write(reinterpret_cast<char*>(contents.data()), contents.size());
		file.close();
		return true;
	}

	bool Load3DModelFromObjFile(char const *filename, bool loadNormals, bool loadTexcoords, bool generateTangentSpaceVectors, bool unify, Mesh &mesh, uint32_t *vertexStride/* = nullptr*/)
	{
		// Load model
		tinyobj::attrib_t attribs;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string error;

		bool result = tinyobj::LoadObj(&attribs, &shapes, &materials, &error, filename);
		if (!result)
		{
			std::cout << "Could not open the '" << filename << "' file.";
			if (0 < error.size())
			{
				std::cout << " " << error;
			}
			std::cout << std::endl;
			return false;
		}

		// Normal vectors and texture coordinates are required to generate tangent and bitangent vectors
		if (!loadNormals || !loadTexcoords)
		{
			generateTangentSpaceVectors = false;
		}

		// Load model data and unify (normalize) its size and position
		float minX = attribs.vertices[0];
		float maxX = attribs.vertices[0];
		float minY = attribs.vertices[1];
		float maxY = attribs.vertices[1];
		float minZ = attribs.vertices[2];
		float maxZ = attribs.vertices[2];

		mesh = {};
		uint32_t offset = 0;

		for (auto & shape : shapes)
		{
			uint32_t partOffset = offset;

			for (auto & index : shape.mesh.indices)
			{
				mesh.m_Data.emplace_back(attribs.vertices[3 * index.vertex_index + 0]);
				mesh.m_Data.emplace_back(attribs.vertices[3 * index.vertex_index + 1]);
				mesh.m_Data.emplace_back(attribs.vertices[3 * index.vertex_index + 2]);
				++offset;

				if (loadNormals)
				{
					if (attribs.normals.size() == 0)
					{
						std::cout << "Could not load normal vectors data in the '" << filename << "' file.";
						return false;
					}
					else
					{
						mesh.m_Data.emplace_back(attribs.normals[3 * index.normal_index + 0]);
						mesh.m_Data.emplace_back(attribs.normals[3 * index.normal_index + 1]);
						mesh.m_Data.emplace_back(attribs.normals[3 * index.normal_index + 2]);
					}
				}

				if (loadTexcoords)
				{
					if (attribs.texcoords.size() == 0) 
					{
						std::cout << "Could not load texture coordinates data in the '" << filename << "' file.";
						return false;
					}
					else
					{
						mesh.m_Data.emplace_back(attribs.texcoords[2 * index.texcoord_index + 0]);
						mesh.m_Data.emplace_back(attribs.texcoords[2 * index.texcoord_index + 1]);
					}
				}

				if (generateTangentSpaceVectors)
				{
					// Insert temporary tangent space vectors data
					for (int i = 0; i < 6; ++i)
					{
						mesh.m_Data.emplace_back(0.0f);
					}
				}

				if (unify)
				{
					if (attribs.vertices[3 * index.vertex_index + 0] < minX)
					{
						minX = attribs.vertices[3 * index.vertex_index + 0];
					}

					if (attribs.vertices[3 * index.vertex_index + 0] > maxX) 
					{
						maxX = attribs.vertices[3 * index.vertex_index + 0];
					}

					if (attribs.vertices[3 * index.vertex_index + 1] < minY) 
					{
						minY = attribs.vertices[3 * index.vertex_index + 1];
					}

					if (attribs.vertices[3 * index.vertex_index + 1] > maxY)
					{
						maxY = attribs.vertices[3 * index.vertex_index + 1];
					}

					if (attribs.vertices[3 * index.vertex_index + 2] < minZ) 
					{
						minZ = attribs.vertices[3 * index.vertex_index + 2];
					}

					if (attribs.vertices[3 * index.vertex_index + 2] > maxZ)
					{
						maxZ = attribs.vertices[3 * index.vertex_index + 2];
					}
				}
			}

			uint32_t partVertexCount = offset - partOffset;
			if (0 < partVertexCount)
			{
				mesh.m_Parts.push_back({ partOffset, partVertexCount });
			}
		}

		uint32_t stride = 3 + (loadNormals ? 3 : 0) + (loadTexcoords ? 2 : 0) + (generateTangentSpaceVectors ? 6 : 0);
		if (vertexStride)
		{
			*vertexStride = stride * sizeof(float);
		}

		if (generateTangentSpaceVectors)
		{
			GenerateTangentSpaceVectors(mesh);
		}

		if (unify)
		{
			float offsetX = 0.5f * (minX + maxX);
			float offsetY = 0.5f * (minY + maxY);
			float offsetZ = 0.5f * (minZ + maxZ);
			float scaleX = abs(minX - offsetX) > abs(maxX - offsetX) ? abs(minX - offsetX) : abs(maxX - offsetX);
			float scaleY = abs(minY - offsetY) > abs(maxY - offsetY) ? abs(minY - offsetY) : abs(maxY - offsetY);
			float scaleZ = abs(minZ - offsetZ) > abs(maxZ - offsetZ) ? abs(minZ - offsetZ) : abs(maxZ - offsetZ);
			float scale = scaleX > scaleY ? scaleX : scaleY;
			scale = scaleZ > scale ? 1.0f / scaleZ : 1.0f / scale;

			for (size_t i = 0; i < mesh.m_Data.size() - 2; i += stride)
			{
				mesh.m_Data[i + 0] = scale * (mesh.m_Data[i + 0] - offsetX);
				mesh.m_Data[i + 1] = scale * (mesh.m_Data[i + 1] - offsetY);
				mesh.m_Data[i + 2] = scale * (mesh.m_Data[i + 2] - offsetZ);
			}
		}

		return true;
	}

	bool LoadTextureDataFromFile(char const *filename, int numRequestedComponents, std::vector<unsigned char> &imageData, int *imageWidth, int * imageHeight, int * imageNumComponents,
		int *imageDataSize)
	{
		int width = 0;
		int height = 0;
		int numComponents = 0;
		std::unique_ptr<unsigned char, void(*)(void*)> stbiData(stbi_load(filename, &width, &height, &numComponents, numRequestedComponents), stbi_image_free);

		if ((!stbiData) || (0 >= width) || (0 >= height) || (0 >= numComponents))
		{
			std::cout << "Could not read image!" << std::endl;
			return false;
		}

		int dataSize = width * height * (0 < numRequestedComponents ? numRequestedComponents : numComponents);
		if (imageDataSize)
		{
			*imageDataSize = dataSize;
		}

		if (imageWidth)
		{
			*imageWidth = width;
		}

		if (imageHeight) 
		{
			*imageHeight = height;
		}

		if (imageNumComponents)
		{
			*imageNumComponents = numComponents;
		}

		imageData.resize(dataSize);
		std::memcpy(imageData.data(), stbiData.get(), dataSize);
		return true;
	}

	Matrix4x4 PrepareRotationMatrix(float angle, Vector3 const &axis, float normalizeAxis/* = false*/)
	{
		float x;
		float y;
		float z;

		if (normalizeAxis)
		{
			Vector3 normalized = Normalize(axis);
			x = normalized[0];
			y = normalized[1];
			z = normalized[2];
		}
		else
		{
			x = axis[0];
			y = axis[1];
			z = axis[2];
		}

		const float c = cos(Deg2Rad(angle));
		const float _1_c = 1.0f - c;
		const float s = sin(Deg2Rad(angle));

		Matrix4x4 rotationMatrix =
		{
			x * x * _1_c + c,
			y * x * _1_c - z * s,
			z * x * _1_c + y * s,
			0.0f,

			x * y * _1_c + z * s,
			y * y * _1_c + c,
			z * y * _1_c - x * s,
			0.0f,

			x * z * _1_c - y * s,
			y * z * _1_c + x * s,
			z * z * _1_c + c,
			0.0f,

			0.0f,
			0.0f,
			0.0f,
			1.0f
		};
		return rotationMatrix;
	}

	Matrix4x4 PreparePerspectiveProjectionMatrix(float aspectRatio, float fieldOfView, float nearPlane, float farPlane)
	{
		float f = 1.0f / tan(Deg2Rad(0.5f * fieldOfView));

		Matrix4x4 perspectiveProjectionMatrix =
		{
			f / aspectRatio,
			0.0f,
			0.0f,
			0.0f,

			0.0f,
			-f,
			0.0f,
			0.0f,

			0.0f,
			0.0f,
			farPlane / (nearPlane - farPlane),
			-1.0f,

			0.0f,
			0.0f,
			(nearPlane * farPlane) / (nearPlane - farPlane),
			0.0f
		};
		return perspectiveProjectionMatrix;
	}

	float Deg2Rad(float value) 
	{
		return value * 0.01745329251994329576923690768489f;
	}

	float Dot(Vector3 const &left, Vector3 const &right)
	{
		return left[0] * right[0] + left[1] * right[1] + left[2] * right[2];
	}

	Vector3 Cross(Vector3 const & left,	Vector3 const & right)
	{
		return
		{
			left[1] * right[2] - left[2] * right[1],
			left[2] * right[0] - left[0] * right[2],
			left[0] * right[1] - left[1] * right[0]
		};
	}

	Vector3 Normalize(Vector3 const & vector)
	{
		float length = std::sqrt(vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2]);
		return
		{
			vector[0] / length,
			vector[1] / length,
			vector[2] / length
		};
	}

	Vector3 operator+ (Vector3 const &left,	Vector3 const &right)
	{
		return
		{
			left[0] + right[0],
			left[1] + right[1],
			left[2] + right[2]
		};
	}

	Vector3 operator- (Vector3 const &left,	Vector3 const &right)
	{
		return
		{
			left[0] - right[0],
			left[1] - right[1],
			left[2] - right[2]
		};
	}

	Vector3 operator+ (float const &left, Vector3 const &right)
	{
		return
		{
			left + right[0],
			left + right[1],
			left + right[2]
		};
	}

	Vector3 operator- (float const &left, Vector3 const &right)
	{
		return
		{
			left - right[0],
			left - right[1],
			left - right[2]
		};
	}

	Vector3 operator+ (Vector3 const &left,	float const &right)
	{
		return
		{
			left[0] + right,
			left[1] + right,
			left[2] + right
		};
	}

	Vector3 operator- (Vector3 const &left, float const &right)
	{
		return
		{
			left[0] - right,
			left[1] - right,
			left[2] - right
		};
	}

	Vector3 operator* (float left, Vector3 const & right)
	{
		return
		{
			left * right[0],
			left * right[1],
			left * right[2]
		};
	}

	Vector3 operator* (Vector3 const &left, float right)
	{
		return
		{
			left[0] * right,
			left[1] * right,
			left[2] * right
		};
	}

	Vector3 operator* (Vector3 const &left,	Matrix4x4 const &right)
	{
		return
		{
			left[0] * right[0] + left[1] * right[1] + left[2] * right[2],
			left[0] * right[4] + left[1] * right[5] + left[2] * right[6],
			left[0] * right[8] + left[1] * right[9] + left[2] * right[10]
		};
	}

	Vector3 operator- (Vector3 const & vector)
	{
		return
		{
			-vector[0],
			-vector[1],
			-vector[2]
		};
	}

	bool operator== (Vector3 const &left, Vector3 const &right)
	{
		if ((std::abs(left[0] - right[0]) > 0.00001f) ||
			(std::abs(left[1] - right[1]) > 0.00001f) ||
			(std::abs(left[2] - right[2]) > 0.00001f))
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	Matrix4x4 operator* (Matrix4x4 const &left, Matrix4x4 const &right)
	{
		return
		{
			left[0] * right[0] + left[4] * right[1] + left[8] * right[2] + left[12] * right[3],
			left[1] * right[0] + left[5] * right[1] + left[9] * right[2] + left[13] * right[3],
			left[2] * right[0] + left[6] * right[1] + left[10] * right[2] + left[14] * right[3],
			left[3] * right[0] + left[7] * right[1] + left[11] * right[2] + left[15] * right[3],

			left[0] * right[4] + left[4] * right[5] + left[8] * right[6] + left[12] * right[7],
			left[1] * right[4] + left[5] * right[5] + left[9] * right[6] + left[13] * right[7],
			left[2] * right[4] + left[6] * right[5] + left[10] * right[6] + left[14] * right[7],
			left[3] * right[4] + left[7] * right[5] + left[11] * right[6] + left[15] * right[7],

			left[0] * right[8] + left[4] * right[9] + left[8] * right[10] + left[12] * right[11],
			left[1] * right[8] + left[5] * right[9] + left[9] * right[10] + left[13] * right[11],
			left[2] * right[8] + left[6] * right[9] + left[10] * right[10] + left[14] * right[11],
			left[3] * right[8] + left[7] * right[9] + left[11] * right[10] + left[15] * right[11],

			left[0] * right[12] + left[4] * right[13] + left[8] * right[14] + left[12] * right[15],
			left[1] * right[12] + left[5] * right[13] + left[9] * right[14] + left[13] * right[15],
			left[2] * right[12] + left[6] * right[13] + left[10] * right[14] + left[14] * right[15],
			left[3] * right[12] + left[7] * right[13] + left[11] * right[14] + left[15] * right[15]
		};
	}
} 

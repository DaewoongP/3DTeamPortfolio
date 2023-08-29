#pragma once
#include <DirectXMath.h>
using namespace DirectX;

namespace Engine
{
	typedef struct tagNode
	{
		wchar_t				szName[256];
		XMFLOAT4X4			TransformationMatrix; // row major

		unsigned int		iNodeIndex;
		int					iParent; // Root Index = -1
		unsigned int		iNumChildren;
		unsigned int*		iChildrens; // array
	}NODE;

	typedef struct tagWeight
	{
		unsigned int		iVertexId;
		float				fWeight;
	}WEIGHT;

	typedef struct tagBone
	{
		wchar_t				szName[256];
		XMFLOAT4X4			OffsetMatrix; // row major

		// Weight
		unsigned int		iNumWeights;
		WEIGHT*				Weights; // array
	}BONE;

	typedef struct tagFace
	{
		unsigned int		iNumIndices;
		unsigned int*		iIndices; // array
	}FACE;

	typedef struct tagMesh
	{
		wchar_t				szName[256];
		unsigned int		iMaterialIndex;
		unsigned int		iNumVertices;
		unsigned int		iNumFaces;
		FACE*				Faces; // array
		XMFLOAT3*			vPositions; // array
		XMFLOAT3*			vNormals; // array
		XMFLOAT2*			vTexCoords; // array
		XMFLOAT3*			vTangents; // array

		// Mesh_Bone
		unsigned int		iNumBones;
		BONE*				Bones; // array
	}MESH;

	typedef enum TextureType
	{
		NONE,
		DIFFUSE,
		SPECULAR,
		AMBIENT,
		EMISSIVE,
		HEIGHT,
		NORMALS,
		SHININESS,
		OPACITY,
		DISPLACEMENT,
		LIGHTMAP,
		REFLECTION,
		BASE_COLOR,
		NORMAL_CAMERA,
		EMISSION_COLOR,
		METALNESS,
		DIFFUSE_ROUGHNESS,
		AMBIENT_OCCLUSION,
		SHEEN,
		CLEARCOAT,
		TRANSMISSION,
		UNKNOWN,
	}TEXTYPE;
#define TextureType_MAX UNKNOWN

	typedef struct tagTexture
	{
		TEXTYPE				TexType;
		wchar_t				szTexPath[256];
	}MATERIALTEX;

	typedef struct tagMaterial
	{
		MATERIALTEX			MaterialTexture[TextureType_MAX];
	}MATERIAL;

	typedef struct tagVectorKey
	{
		float				fTime;
		XMFLOAT3			vValue;
	}VECTORKEY;

	typedef struct tagQuaternionKey
	{
		float				fTime;
		XMFLOAT4			vValue;
	}QUATERNIONKEY;

	typedef struct tagChannel
	{
		// Same as Node Name
		wchar_t				szName[256];

		// Scale
		unsigned int		iNumScalingKeys;
		VECTORKEY*			ScalingKeys; // array

		// Rotation
		unsigned int		iNumRotationKeys;
		QUATERNIONKEY*		RotationKeys; // array

		// Position
		unsigned int		iNumPositionKeys;
		VECTORKEY*			PositionKeys;
	}CHANNEL;

	typedef struct tagAnimation
	{
		wchar_t				szName[256];
		float				fDuration;
		float				fTickPerSecond;

		// Channel
		unsigned int		iNumChannels;
		CHANNEL*			Channels; // array
	}ANIMATION;

	typedef struct tagModel
	{
		// Node
		unsigned int		iNumNodes;
		// Mesh
		unsigned int		iNumMeshes;
		// Material
		unsigned int		iNumMaterials;
		// Animation
		unsigned int		iNumAnimations;
	}MODEL;
}
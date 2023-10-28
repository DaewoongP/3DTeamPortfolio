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

	typedef struct tagColMesh
	{
		unsigned int		iNumVertices;
		unsigned int		iNumFaces;
		FACE* Faces; // array
		XMFLOAT3* vPositions; // array
	}COLMESH;

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

	/* For GCM Convert*/
	typedef struct tagModel_GCM
	{
		// Node
		unsigned int		iNumNodes;
		// Mesh
		unsigned int		iNumMeshes;
		// Material
		unsigned int		iNumMaterials;
		// RootBoneIndex
		unsigned int		iRootBoneIndex;
		// PartCount
		unsigned int		iAnimationPartCount;
		// Array
		unsigned int*		iNumAnimations;
		// Array
		//(각 파츠가 영향을 주는 뼈의 포인터)를 가리기키는 포인터
		vector<unsigned int>* iAffectBones;
	}MODEL_GCM;
	
	typedef struct tagFrame_GCM : public KEYFRAME
	{
		wchar_t			szName[256];
	}KEYFRAME_GCM;

	typedef struct tagSpeedFrame_GCM : KEYFRAME_GCM
	{
		float			fSpeed{};
	}SPEEDFRAME_GCM;

	typedef struct tagNotifyFrame_GCM :KEYFRAME_GCM
	{
		function<void()> Action;
	}NOTIFYFRAME_GCM;

	typedef struct tagSoundFrame_GCM : KEYFRAME_GCM
	{
		wchar_t wszSoundTag[MAX_PATH] = {};
		_float	fVolum = { 0.7f };
	}SOUNDFRAME_GCM;

	typedef struct tagParticleFrame_GCM : KEYFRAME_GCM
	{
		wchar_t wszParticleTag[MAX_PATH] = {};
		_uint	   iBoneIndex = { 0 };
		_float4x4  OffsetMatrix = {};
	}PARTICLEFRAME_GCM;

	//노티파이엔 이름이 없어요.
	typedef struct tagNotify_GCM
	{
		unsigned int			iNumKeyFrames;
		//프레임에만 이름이 있어요
		vector<KEYFRAME_GCM*>			tKeyFrame;
	}NOTIFY_GCM;

	typedef struct tagMatrixFrame_GCM
	{
		XMFLOAT3		vScale;
		XMFLOAT4		vRotation;
		XMFLOAT3		vTranslation;

		float			fTime;
	}MATRIXFRAME_GCM;

	typedef struct tagChannel_GCM
	{
		wchar_t				szName[256];
		
		unsigned int		iNumMartixKeys;
		MATRIXFRAME_GCM*		iMatrixFrame;
	}CHANNEL_GCM;

	typedef struct tagAnimation_GCM
	{
		wchar_t				szName[256];
		float				fDuration;
		float				fTickPerSecond;

		_bool				isLoop;
		_bool				isRootAnim;
		_bool				isLerp;

		XMFLOAT3			vOffsetPosition = {};

		// Channel
		unsigned int		iNumChannels;
		CHANNEL_GCM*		Channels; // array

		//Notify
		NOTIFY_GCM*			Notify; // one

	}ANIMATION_GCM;

	
}
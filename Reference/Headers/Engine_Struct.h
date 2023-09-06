#pragma once

namespace Engine
{
	typedef struct tagGraphicDesc
	{
		enum WINMODE { WM_FULL, WM_WIN, WM_END };
		HWND			hWnd;
		unsigned int	iViewportSizeX, iViewportSizeY;
		WINMODE			eWinMode;
	}GRAPHICDESC;

	typedef struct tagMeshMaterial
	{
		class CTexture* pMtrlTexture[20];
	}MESHMATERIAL;

	//행렬 변환용 키프레임 입니다.
	typedef struct tagMatrixFrame
	{
		XMFLOAT3		vScale;
		XMFLOAT4		vRotation;
		XMFLOAT3		vTranslation;

		float			fTime;
	}MATRIXFRAME;

	typedef struct tagFrame
	{
		enum KEYFRAMETYPE {KF_SPEED,KF_NOTIFY,KF_SOUND,KF_END};
		KEYFRAMETYPE    eKeyFrameType;
		float			fTime;
		bool			isEnable = { true };
	}KEYFRAME;

	typedef struct tagSpeedFrame : KEYFRAME
	{
		float			fSpeed;
	}SPEEDFRAME;

	typedef struct tagNotifyFrame :KEYFRAME
	{
		function<void()> Action;
	}NOTIFYFRAME;

	typedef struct tagSoundFrame : KEYFRAME
	{
		wchar_t wszSoundTag[MAX_PATH] = {};
	}SOUNDFRAME;

	typedef struct tagAnimationDesc 
	{
		explicit tagAnimationDesc() {};
		explicit tagAnimationDesc(const tagAnimationDesc& rhs)
		{
			iPreviousAnimIndex = rhs.iPreviousAnimIndex;
			iCurrentAnimIndex = rhs.iCurrentAnimIndex;
			iNumAnimations = rhs.iNumAnimations;
			isResetAnimTrigger = rhs.isResetAnimTrigger;
			isAnimChangeLerp = rhs.isAnimChangeLerp;
			fAnimChangeTimer = rhs.fAnimChangeTimer;
			iRootBoneIndex = rhs.iRootBoneIndex;
			AffectBoneVec = rhs.AffectBoneVec;
		}

		/* Basic Action*/
		_uint							iPreviousAnimIndex = { 0 };
		_uint							iCurrentAnimIndex = { 0 };
		_uint							iNumAnimations = { 0 };
		vector<class CAnimation*>		Animations;
		_bool							isResetAnimTrigger = { false };

		/* For Lerp*/
		_bool							isAnimChangeLerp = { false };
		_float							fAnimChangeTimer = { 0.f };

		/* For Setting Bone*/
		_uint							iRootBoneIndex = { 0 };
		vector<_uint>					AffectBoneVec;
	}ANIMATIONDESC;

	/*typedef struct tagColliderFrame
	{
		Collider* pCollider = { nullptr	};
		float			fTime;
	}COLLIDERFRAME;
	typedef struct tagParticleFrame
	{
		float			fTime;
		float			fTime;
	}PARTICLEFRAME;*/

	typedef struct tagNotify
	{
		// 해당 애니메이션 속도값
		float			fSpeed;

		// 카메라값
		XMFLOAT4		vEye;
		XMFLOAT4		vAt;

		// 사운드값
		//const _tchar	szSouneName[MAX_PATH];
		// 사운드enum...

		// 이펙트, 파티클...

		float			fTime;
	}NOTIFY;

	//장현우
	//서브카메라 구조체
	typedef struct ENGINE_DLL tagCutSceneCameraDesc
	{
		_bool isLerp;
		_float fDuration;
		_float4 vAt;
		_float4 vEye;
	}CUTSCENECAMERADESC;

	typedef struct ENGINE_DLL tagOffSetCameraDesc
	{
		_bool isLerp;
		_float fDuration;
		_float4x4 OffsetMatrix;
		_float4x4* pTargetMatrix;
	}OFFSETCAMERADESC;

	//보류
	typedef struct ENGINE_DLL tagActionCameraDesc
	{
		_float fDuration;
		_float4 vAt;
		_float4 vEye;
	}ACTIONCAMERADESC;


	typedef struct tagCollisionDesc
	{
		enum COLTYPE
		{
			COLTYPE_PLAYER,
			COLTYPE_MONSTER,
			COLTYPE_END
		};

		COLTYPE						ColType = { COLTYPE_END };

		const class CCollider*		pMyCollider = { nullptr };
		const class CCollider*		pOtherCollider = { nullptr };
		const class CGameObject*	pOtherOwner = { nullptr };
		const class CTransform*		pOtherTransform = { nullptr };
	}COLLISIONDESC;

	typedef struct tagVertex_Position
	{
		XMFLOAT3		vPosition;
	}VTXPOS;

	typedef struct ENGINE_DLL tagVertex_Position_Declaration
	{
		static const unsigned int				iNumElements = { 1 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXPOS_DECL;

	typedef struct tagVertex_Position_TexCoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexCoord;
	}VTXPOSTEX;

	typedef struct ENGINE_DLL tagVertex_Position_TexCoord_Declaration
	{
		static const unsigned int				iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXPOSTEX_DECL;

	typedef struct tagVertex_Position_Normal_TexCoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexCoord;
	}VTXPOSNORTEX;

	typedef struct ENGINE_DLL tagVertex_Position_Normal_TexCoord_Declaration
	{
		static const unsigned int				iNumElements = { 3 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXPOSNORTEX_DECL;

	typedef struct tagVertex_Position_TexCube
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexCoord;
	}VTXPOSCUBE;

	typedef struct ENGINE_DLL tagVertex_Position_TexCube_Declaration
	{
		static const unsigned int				iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXPOSCUBE_DECL;

	typedef struct tagVertex_Mesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexCoord;
		XMFLOAT3		vTangent;
	}VTXMESH;

	typedef struct ENGINE_DLL tagVertex_Mesh_Declaration
	{
		static const unsigned int				iNumElements = { 4 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXMESH_DECL;

	typedef struct tagVertex_AnimMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexCoord;
		XMFLOAT3		vTangent;
		XMUINT4			vBlendIndices;
		XMFLOAT4		vBlendWeights;
	}VTXANIMMESH;

	typedef struct ENGINE_DLL tagVertex_AnimMesh_Declaration
	{
		static const unsigned int				iNumElements = { 6 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXANIMMESH_DECL;

	typedef struct tagVertex_Instance
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vTranslation;
	}VTXINSTANCE;

	// VTXPOSTEX
	typedef struct ENGINE_DLL tagVertex_Rect_Instance_Declaration
	{
		static const unsigned int				iNumElements = { 6 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXRECTINSTANCE_DECL;

	typedef struct ENGINE_DLL tagVertex_Rect_Color_Instance_Declaration
	{
		static const unsigned int				iNumElements = { 7 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXRECTCOLORINSTANCE_DECL;

	typedef struct tagVertex_Position_Size
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vPSize;
	}VTXPOINT;

	typedef struct ENGINE_DLL tagVertex_Point_Instance_Declaration
	{
		static const unsigned int				iNumElements = { 6 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXPOINTINSTANCE_DECL;

	typedef struct tagVertex_Color_Instance
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vTranslation;
		XMFLOAT4		vColor;
	}VTXCOLINSTANCE;

	typedef struct ENGINE_DLL tagVertex_Point_Color_Instance_Declaration
	{
		static const unsigned int				iNumElements = { 7 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXPOINTCOLORINSTANCE_DECL;

}

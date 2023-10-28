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
		enum KEYFRAMETYPE {KF_SPEED,KF_NOTIFY,KF_SOUND,KF_PARTICLE,KF_END};

		tagFrame() {}
		tagFrame(const tagFrame& rhs)
			: eKeyFrameType(rhs.eKeyFrameType)
			, fTime(rhs.fTime)
			, isEnable(rhs.isEnable) {}

		KEYFRAMETYPE    eKeyFrameType = KF_NOTIFY;
		float			fTime = { 0.f };
		bool			isEnable = { true };
	}KEYFRAME;

	typedef struct tagSpeedFrame : KEYFRAME
	{
		tagSpeedFrame() {}
		tagSpeedFrame(const tagSpeedFrame& rhs)
			: tagFrame(rhs)
			, fSpeed(rhs.fSpeed) {}
		float			fSpeed = 0.f;
	}SPEEDFRAME;

	typedef struct tagNotifyFrame :KEYFRAME
	{
		tagNotifyFrame() {}
		tagNotifyFrame(const tagNotifyFrame& rhs)
			: tagFrame(rhs)
		{
			Action = rhs.Action;
		}
		function<void()> Action = { nullptr };
	}NOTIFYFRAME;

	typedef struct tagSoundFrame : KEYFRAME
	{
		tagSoundFrame() {}
		tagSoundFrame(const tagSoundFrame& rhs)
			: tagFrame(rhs)
		{
			lstrcpy(wszSoundTag, rhs.wszSoundTag);
		}
		wchar_t wszSoundTag[MAX_PATH] = {};
		_float	fVolum = { 0.7f };
	}SOUNDFRAME;

	typedef struct tagParticleFrame : KEYFRAME
	{
		tagParticleFrame() {}
		tagParticleFrame(const tagParticleFrame& rhs)
			: tagFrame(rhs)
		{
			lstrcpy(wszParticleTag, rhs.wszParticleTag);
			iBoneIndex = rhs.iBoneIndex;
			OffsetMatrix = rhs.OffsetMatrix;
			//BindBoneMatrix = rhs.BindBoneMatrix;
		}
		wchar_t wszParticleTag[MAX_PATH] = {};
		// 파티클이 재생될 위치를 지정하기 위함임.
		// 붙을 뼈의 인덱스를 저장함.
		_uint	   iBoneIndex = {0};
		// 얘는 툴에서 불러올때 나올거임.
		const _float4x4* BindBoneMatrix = { nullptr };
		// 얘는 툴에서 지정해주자.
		_float4x4  OffsetMatrix = {};
	}PARTICLEFRAME;

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

		/* For Check End*/
		_bool							isFinishAnimation = { false };
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

	// // ****************** 사용 금지 ************************* //
	typedef struct tagCollisionDesc
	{
		// 삭제 예정
		enum COLTYPE
		{
			COLTYPE_PLAYER,
			COLTYPE_MONSTER,
			COLTYPE_END
		};

		COLTYPE						ColType = { COLTYPE_END };
		const class CCollider* pMyCollider = { nullptr };
		const class CCollider* pOtherCollider = { nullptr };
		// 삭제 예정




		const _tchar*				pOtherObjectTag = { nullptr };
		const class CGameObject*	pOtherOwner = { nullptr };
		const class CTransform*		pOtherTransform = { nullptr };
	}COLLISIONDESC;

	// 피직스 콜라이더 이벤트 구조체.
	typedef struct tagCollisionEventDesc
	{
		const _tchar*				pOtherObjectTag = { nullptr };
		_tchar						pOtherCollisionTag[MAX_PATH] = TEXT("");
		_tchar						pThisCollisionTag[MAX_PATH] = TEXT("");
		const class CGameObject*	pOtherOwner = { nullptr };
		const class CTransform*		pOtherTransform = { nullptr };
		void*						pArg = { nullptr };
	}COLLEVENTDESC;

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

	typedef struct tagVertex_Position_Normal
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
	}VTXPOSNOR;

	typedef struct ENGINE_DLL tagVertex_Position_Normal_Declaration
	{
		static const unsigned int				iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXPOSNOR_DECL;

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

	typedef struct ENGINE_DLL tagVertex_Mesh_Instance_Declaration
	{
		static const unsigned int				iNumElements = { 8 };
		static const D3D11_INPUT_ELEMENT_DESC			Elements[8];
	}VTXMESHINSTANCE_DECL;

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

	//
	typedef struct tagVertex_Color_Index_Instance
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vTranslation;
		XMFLOAT4		vColor;
		UINT			iCurrentIndex;
	}VTXCOLIDXINSTANCE;

	typedef struct ENGINE_DLL tagVertex_Rect_Color_Index_Instance_Declaration
	{
		static const unsigned int				iNumElements = { 8 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXRECTCOLIDXINSTANCE_DECL;

	typedef struct tagVertex_Particle_Instance
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vTranslation;
		XMFLOAT4		vColor;
		XMFLOAT4		vVelocity;
		UINT			iCurrentIndex;
	}VTXPARTICLEINSTANCE;

	typedef struct ENGINE_DLL tagVertex_Velocity_Instance_Declartion
	{
		static const unsigned int				iNumElements = { 9 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXPARTICLE_DECL;
}

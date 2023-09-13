#pragma once

/* =============================================== */
// 
//	�� : �ڴ��
//	�� :
//
/* =============================================== */

#include "PipeLine.h"
#include "Time_Manager.h"
#include "Input_Device.h"
#include "Light_Manager.h"
#include "PhysX_Manager.h"
#include "Sound_Manager.h"
#include "Component_Manager.h"
#include "Collision_Manager.h"
#include "Ease.h"
BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)

private:
	explicit CGameInstance();
	virtual ~CGameInstance() = default;

public:
	// ���� ������Ʈ �ʱ�ȭ �Լ�
	HRESULT Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const GRAPHICDESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	// ���� ������Ʈ �̸� ���� �Ҵ� �Լ�
	HRESULT	Reserve_Engine(_uint iNumLevels);
	// ���� Tick ȣ�� �Լ�
	void Tick_Engine(_float fTimeDelta);
	// ���� ���ҽ� �ʱ�ȭ �Լ�
	void Clear_LevelResources(_uint iLevelIndex);

public: /* For. Graphic_Device */
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();

	HRESULT Bind_BackBuffer();

public: /* For.Timer_Manager */
	/*	Ÿ�̸� �߰� 
		*����ü �̼��� �� ����Ʈ �� ����*  */
	HRESULT Add_Timer(const wstring & wstrTimerTag, _bool m_isRepeat = false, const _float fDuration = 0.f);
	/* Ÿ�̸� ���� */
	HRESULT Remove_Timer(const wstring & wstrTimerTag);
	/* Ÿ�̸��� TimeAcc�� 0.f�� �ʱ�ȭ */
	HRESULT Reset_Timer(const wstring & wstrTimerTag);
	/*	�������� �ð� üũ �Լ�.
		�ش� �±��� �ð��� Duration���� ���� ��� false�� ��ȯ.
		ũ�ų� ������ true ��ȯ */
	_bool Check_Timer(const wstring & wstrTimerTag);
	/* ���� ���� �����ð��� ��ȯ*/
	const _float& Get_World_TimeAcc() const;
	/* ���� ���� ƽ�� ��ȯ */
	_float Get_World_Tick() const;
	/* ������ �߰��� Ÿ�̸��� �����ð��� ��ȯ */
	_float Get_TimeAcc(const wstring & wstrTimerTag) const;
	/* ���� �����ð� �ʱ�ȭ (����� ��� ���������� �̸� ����ϼ���) */
	void Reset_World_TimeAcc();
	HRESULT	Add_QueryTimer(const _tchar * pTimerTag);
	void	Tick_QueryTimer(const _tchar * pTimerTag);
	_float	Get_QueryTimeDelta(const _tchar * pTimerTag);

public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iLevelIndex, class CLevel * pNewLevel);
	_uint Get_CurrentLevelIndex() const;
	class CLevel* Get_CurrentLevel();
	HRESULT Render_Level();

public: /* For.Component_Manager*/
	class CComponent* Find_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag);
	HRESULT Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, class CComponent* pPrototype, _bool isFailedSkip = false);
	HRESULT Add_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, const _tchar * pLayerTag, const _tchar * pComponentTag, void* pArg = nullptr);
	class CComponent* Clone_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, void* pArg = nullptr);
	class CComponent* Find_Component_In_Layer(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag);
	class CLayer* Find_Layer(_uint iLevelIndex, const _tchar * pLayerTag);
	HRESULT	Clear_Layer(_uint iLevelIndex, const _tchar * pLayerTag);
	HRESULT	Delete_Component(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag);

public: /* For.Input_Device*/
	_bool		Get_DIKeyState(_ubyte ubyKeyID, CInput_Device::KEYSTATE eState = CInput_Device::KEY_PRESSING);
	_bool		Get_DIMouseState(CInput_Device::MOUSEKEYSTATE eMouseID, CInput_Device::KEYSTATE eState = CInput_Device::KEY_PRESSING);
	_long		Get_DIMouseMove(CInput_Device::MOUSEMOVESTATE eMouseMoveID);

public: /* For.PipeLine*/
	// ī�޶󿡼� ��, ���� ��� ����
	void Set_Transform(CPipeLine::D3DTRANSFORMSTATE eTransformState, _float4x4 TransformStateMatrix);
	// ī�޶󿡼� Far �� ����.
	void Set_CameraFar(_float fCamFar);
	const _float4x4* Get_TransformMatrix(CPipeLine::D3DTRANSFORMSTATE eTransformState);
	const _float4x4* Get_TransformMatrix_Inverse(CPipeLine::D3DTRANSFORMSTATE eTransformState);
	const _float4* Get_CamPosition();
	const _float3* Get_CamUp();
	const _float3* Get_CamLook();
	const _float* Get_CamFar();

public: /* For. Collision_Manager */
	HRESULT Add_Collider(COLLISIONDESC::COLTYPE eCollisionType, class CCollider* pCollider);
	void ClearColliders();

public: /* For. Font_Manager */
	HRESULT Add_Fonts(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFontTag, const _tchar * pFontFilePath);
	HRESULT Render_Font(const _tchar * pFontTag, const _tchar * pText, const _float2 & Position, _float4 vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float fRotation = 0.f, const _float2 & vOrigin = _float2(0.f, 0.f), _float fScale = 1.f);

public: /* For. Frustum */
	_bool isIn_WorldFrustum(_float4 vWorldPos, _float fRange = 0.f);

public: /* For.Light_Manager */
	const CLight::LIGHTDESC* Get_Light(_uint iIndex);
	const _float4x4* Get_LightView();
	const _float4x4* Get_LightProj();


	void Set_Light(_uint iIndex, CLight::LIGHTDESC LightDesc);
	class CLight* Add_Lights(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CLight::LIGHTDESC & LightDesc);
	HRESULT Delete_Lights(_uint iIndex,const _char* Name);
	HRESULT Clear_Lights();

public: /* For.Sound_Manager */
	HRESULT Add_Sounds(const _tchar * szSoundFilePath);
	HRESULT Play_Sound(const _tchar * szSoundTag, CSound_Manager::SOUNDCHANNEL eChannel, _float fVolume, _bool bForcePlay = false);
	// �������� ������ ���� ������ �±� �Է�
	HRESULT Play_Sound(const _tchar * pSoundTag, _uint iNumSounds, CSound_Manager::SOUNDCHANNEL eChannel, _float fVolume, _bool bForcePlay = false);
	HRESULT Play_BGM(const _tchar * szSoundTag, _float fVolume);
	HRESULT Stop_Sound(CSound_Manager::SOUNDCHANNEL eChannel);
	HRESULT Pause_Sound(CSound_Manager::SOUNDCHANNEL eChannel);
	HRESULT Restart_Sound(CSound_Manager::SOUNDCHANNEL eChannel);
	HRESULT Stop_AllSound();
	HRESULT Set_ChannelVolume(CSound_Manager::SOUNDCHANNEL eChannel, _float fVolume);

public: /* For. Calculator */
	// ���� ���콺�� ���̸� ��ȯ�޴� �Լ��Դϴ�(��ŷ����� ���� ����� �ʿ�). (����)
	HRESULT Get_MouseRay(ID3D11DeviceContext * pContext, HWND hWnd, _float4x4 PickingWorldMatrix_Inverse, _Inout_ _float4 * vRayPos, _Inout_ _float4 * vRayDir);
	// ���� ���콺�� ���̸� ��ȯ�޴� �Լ��Դϴ� (����)
	HRESULT Get_WorldMouseRay(ID3D11DeviceContext * pContext, HWND hWnd, _Inout_ _float4 * vRayPos, _Inout_ _float4 * vRayDir);
	// ���콺�� Ŭ���̾�Ʈ ���ο� �ִ��� üũ�ϴ� �Լ��Դϴ�.
	_bool IsMouseInClient(ID3D11DeviceContext * pContext, HWND hWnd);
	// 1�� ���� : ����ġ�� ���Ϳ� ���� (��ü ������� �۾ƾ���)
	// 2�� ���� : �������� ���� ������ ����
	// ��ȯ : ������� �������� �ϳ��� ��ȯ
	// ���ͻ������ ���� �ٸ���� ���Ƿ� ���Ϳ� ���� �ۼ�Ʈ�� �й��ؼ� �־���.
	// ������ ���ƾ��ϹǷ� ȿ������ ���̷��� ����� �����ָ� ����.
	_uint RandomChoose(vector<_float> Weights, _uint iChooseSize);
	// ������ ���̸� �Է��ϸ� �� ������ ���ο��� ���⺤�͸� �����ϰ� �̾��ִ� �Լ��Դϴ�.
	_float4 Get_RandomVectorInSphere(_float fRadius);
	// FilePath ���� ��� ������ ��ȸ�ϸ鼭 Ext Ȯ���ڿ� �´� ���ϵ��� OutVector�� �־��ݴϴ�.
	HRESULT ReadFileInDirectory(_Inout_ vector<wstring>& OutVector, const _tchar* pFilePath, const _tchar* pExt);
	_float3 PolarToCartesian(_float _fLength, _float _fTheta, _float _fPhi);
	_float4x4 RightUpLook_In_Vectors(_float3 vDestPos, _float3 vSourPos);

public: /* For.PhysX_Manager */
	PxPhysics* Get_Physics() const;
	PxScene* Get_PhysxScene() const;
	PxControllerManager* Get_ControllerManager() const;
	cloth::Factory* Get_ClothFactory() const;
	// 1. vOrigin : ���� �������� 2. vDir : ���� 3. fMaxDist : �ִ�Ÿ� 4. pHitPosition : (out)���̰� �浹�� ��ġ 5. pDist : (out)�浹�� �Ÿ� 
	// 6. iMaxHits : ���̸� ���� �� �ִ� �ִ� ���� 7. RaycastFlag : dynamic / static / all �߿� ���̿� �浹�� ��ü Ÿ�� (static�� ����Ʈ�ʵ� ���� �������ΰ� �����ؾ��մϴ�.)
	// ��ȯ : �浹 ���� �� true
	_bool RayCast(_float3 vOrigin, _float3 vDir, _float fMaxDist = PX_MAX_F32, _Inout_ _float3* pHitPosition = nullptr, _Inout_ _float* pDist = nullptr, _uint iMaxHits = 1, CPhysX_Manager::RayCastQueryFlag RaycastFlag = CPhysX_Manager::RAY_ALL);
	// 1. pContext : Dx11 DeviceContext 2. hWnd : Ŭ���̾�Ʈ �ڵ� 3. fMaxDist : �ִ�Ÿ� 4. pHitPosition : (out)���̰� �浹�� ��ġ 5. pDist : (out)�浹�� �Ÿ� 
	// 6. iMaxHits : ���̸� ���� �� �ִ� �ִ� ���� 7. RaycastFlag : dynamic / static / all �߿� ���̿� �浹�� ��ü Ÿ�� (static�� ����Ʈ�ʵ� ���� �������ΰ� �����ؾ��մϴ�.)
	// ��ȯ : �浹 ���� �� true
	_bool Mouse_RayCast(HWND hWnd, ID3D11DeviceContext* pContext, _float fMaxDist = PX_MAX_F32, _Inout_ _float3* pHitPosition = nullptr, _Inout_ _float* pDist = nullptr, _uint iMaxHits = 1, CPhysX_Manager::RayCastQueryFlag RaycastFlag = CPhysX_Manager::RAY_ALL);

public:	/* For.Camera_Manager */
	//�ƾ� ī�޶� �����͸� ��´�.
	HRESULT Read_CutSceneCamera(const _tchar* _CutSceneTag, const _tchar* _CutScenePath);
	//�ƾ� ����� ���� ť �߰�
	HRESULT Add_CutScene(const _tchar* _CutSceneTag);
	//�ƾ� ����� ���� ť �߰�
	HRESULT Add_CutScene(CUTSCENECAMERADESC& _CutSceneCameraDesc);
	//������ ī�޶� �����͸� ��´�.
	HRESULT Read_OffSetCamera(const _tchar* _OffSetTag, const _tchar* _OffSetPath);
	//������ ����� ���� ť �߰�
	HRESULT Add_OffSetCamera(const _tchar* _OffSetTag);
	//ī�޶� �߰�
	HRESULT Add_Camera(const _tchar* _CameraTag, class CCamera* _pCamera);
	//ī�޶� ����
	HRESULT Set_Camera(const _tchar* _CameraTag);
	//ī�޶� ã��
	class CCamera* Find_Camera(const _tchar* _CameraTag);
	//�ƾ� ����
	void Stop_CutScene();

public: /* For.RenderTaget_Manager*/
	class CRenderTarget* Find_RenderTarget(const _tchar* pTargetTag);

public: /* For. String_Manager */
	// �����迭�� ���ڿ��� �Ҵ����ִ� �Լ�
	// ������ ����Ǳ� ������ ����ִ� ���ڿ��Դϴ�.
	_char* Make_Char(const _char* pMakeChar);
	// �����迭�� ���ڿ��� �Ҵ����ִ� �Լ�
	// ������ ����Ǳ� ������ ����ִ� ���ڿ��Դϴ�.
	_tchar* Make_WChar(const _tchar* pMakeWChar);
	HRESULT Delete_Char(_char* pChar);
	HRESULT Delete_WChar(_tchar* pWChar);

private:
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CInput_Device*			m_pInput_Device = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CComponent_Manager*		m_pComponent_Manager = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CCollision_Manager*		m_pCollision_Manager = { nullptr };
	class CFont_Manager*			m_pFont_Manager = { nullptr };
	class CFrustum*					m_pFrustum = { nullptr };
	class CRenderTarget_Manager*	m_pRenderTarget_Manager = { nullptr };
	class CLight_Manager*			m_pLight_Manager = { nullptr };
	class CSound_Manager*			m_pSound_Manager = { nullptr };
	class CCalculator*				m_pCalculator = { nullptr };
	class CPhysX_Manager*			m_pPhysX_Manager = { nullptr };
	class CCamera_Manager*			m_pCamera_Manager = { nullptr };
	class CString_Manager*			m_pString_Manager = { nullptr };
	
public:
	
	HRESULT Add_Prototype_Textures(_uint iLevel, ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pPrototypeName, const _tchar* pTargetExtension, const _tchar* pDirectoryPath, _bool isFailedSkip);
	HRESULT Add_Prototype_Models(_uint iLevel, ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eType, const _tchar* pPrototypeName, const _tchar* pTargetExtension, const _tchar* pDirectoryPath, _bool isFailedSkip, _float4x4 PivotMatrix = _float4x4());
	static void Release_Engine();
	virtual void Free() override;
};

END
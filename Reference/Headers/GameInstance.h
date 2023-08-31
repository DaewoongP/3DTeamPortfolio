#pragma once
#include "PipeLine.h"
#include "Input_Device.h"
#include "Light_Manager.h"
#include "Sound_Manager.h"
#include "Component_Manager.h"
#include "Collision_Manager.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)

private:
	explicit CGameInstance();
	virtual ~CGameInstance() = default;

public:
	// 엔진 프로젝트 초기화 함수
	HRESULT Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const GRAPHICDESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	// 엔진 프로젝트 미리 공간 할당 함수
	HRESULT	Reserve_Engine(_uint iNumLevels);
	// 엔진 Tick 호출 함수
	void Tick_Engine(_float fTimeDelta);
	// 레벨 리소스 초기화 함수
	void Clear_LevelResources(_uint iLevelIndex);

public: /* For. Graphic_Device */
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();

	HRESULT Bind_BackBuffer();

public: /* For.Timer_Manager */
	HRESULT		Add_Timer(const _tchar * pTimerTag);
	void		Tick_Timer(const _tchar * pTimerTag);
	_float		Get_TimeDelta(const _tchar * pTimerTag);

public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iLevelIndex, class CLevel * pNewLevel);
	_uint Get_CurrentLevelIndex() const;
	class CLevel* Get_CurrentLevel();
	HRESULT Render_Level();

public: /* For.Object_Manager */
	HRESULT Add_Prototype_GameObject(const _tchar * pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject(_uint iLevelIndex, const _tchar * pPrototypeTag, const _tchar * pLayerTag, const _tchar * pGameObjectTag, void* pArg = nullptr);
	class CLayer* Find_Layer(_uint iLevelIndex, const _tchar * pLayerTag);
	class CGameObject* Find_GameObject_In_Layer(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pGameObjectTag);

public: /* For.Component_Manager*/
	HRESULT Add_Prototype_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, void* pArg = nullptr);
	HRESULT	Delete_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag);

public: /* For.Input_Device*/
	_bool		Get_DIKeyState(_ubyte ubyKeyID, CInput_Device::KEYSTATE eState = CInput_Device::KEY_PRESSING);
	_bool		Get_DIMouseState(CInput_Device::MOUSEKEYSTATE eMouseID, CInput_Device::KEYSTATE eState = CInput_Device::KEY_PRESSING);
	_long		Get_DIMouseMove(CInput_Device::MOUSEMOVESTATE eMouseMoveID);

public: /* For.PipeLine*/
	// 카메라에서 뷰, 투영 행렬 세팅
	void Set_Transform(CPipeLine::D3DTRANSFORMSTATE eTransformState, _float4x4 TransformStateMatrix);
	// 카메라에서 Far 값 세팅.
	void Set_CameraFar(_float fCamFar);
	const _float4x4* Get_TransformMatrix(CPipeLine::D3DTRANSFORMSTATE eTransformState);
	const _float4x4* Get_TransformMatrix_Inverse(CPipeLine::D3DTRANSFORMSTATE eTransformState);
	const _float4* Get_CamPosition();
	const _float* Get_CamFar();

public: /* For. Collision_Manager */
	HRESULT Add_Collider(COLLISIONDESC::COLTYPE eCollisionType, class CCollider* pCollider);
	void ClearColliders();

public: /* For. Font_Manager */
	HRESULT Add_Fonts(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFontTag, const _tchar * pFontFilePath);
	HRESULT Render_Font(const _tchar * pFontTag, const _tchar * pText, const _float2 & Position, _float4 vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float fRotation = 0.f, const _float2 & vOrigin = _float2(0.f, 0.f), _float fScale = 1.f);

public: /* For. Frustum */
	_bool isIn_WorldFrustum(_float4 vWorldPos, _float fRange = 0.f);
	vector<_float4> Get_CulledPositions() const;

public: /* For.Light_Manager */
	const CLight::LIGHTDESC* Get_Light(_uint iIndex);
	void Set_Light(_uint iIndex, CLight::LIGHTDESC LightDesc);
	class CLight* Add_Lights(const CLight::LIGHTDESC & LightDesc);
	HRESULT Clear_Lights();

public: /* For.Sound_Manager */
	HRESULT Add_Sounds(const _tchar * szSoundFilePath);
	HRESULT Play_Sound(const _tchar * szSoundTag, CSound_Manager::SOUNDCHANNEL eChannel, _float fVolume, _bool bForcePlay = false);
	// 랜덤으로 실행할 사운드 개수와 태그 입력
	HRESULT Play_Sound(const _tchar * pSoundTag, _uint iNumSounds, CSound_Manager::SOUNDCHANNEL eChannel, _float fVolume, _bool bForcePlay = false);
	HRESULT Play_BGM(const _tchar * szSoundTag, _float fVolume);
	HRESULT Stop_Sound(CSound_Manager::SOUNDCHANNEL eChannel);
	HRESULT Pause_Sound(CSound_Manager::SOUNDCHANNEL eChannel);
	HRESULT Restart_Sound(CSound_Manager::SOUNDCHANNEL eChannel);
	HRESULT Stop_AllSound();
	HRESULT Set_ChannelVolume(CSound_Manager::SOUNDCHANNEL eChannel, _float fVolume);

public: /* For. Calculator */
	// 현재 마우스의 레이를 반환받는 함수입니다(피킹대상의 월드 역행렬 필요). (로컬)
	HRESULT Get_MouseRay(ID3D11DeviceContext * pContext, HWND hWnd, _float4x4 PickingWorldMatrix_Inverse, _Inout_ _float4 * vRayPos, _Inout_ _float4 * vRayDir);
	// 현재 마우스의 레이를 반환받는 함수입니다 (월드)
	HRESULT Get_WorldMouseRay(ID3D11DeviceContext * pContext, HWND hWnd, _Inout_ _float4 * vRayPos, _Inout_ _float4 * vRayDir);
	// 마우스가 클라이언트 내부에 있는지 체크하는 함수입니다.
	_bool IsMouseInClient(ID3D11DeviceContext * pContext, HWND hWnd);
	// 1번 인자 : 가중치를 벡터에 대입 (전체 사이즈보다 작아야함)
	// 2번 인자 : 랜덤으로 뽑을 사이즈 대입
	// 반환 : 사이즈보다 작은값중 하나를 반환
	// 벡터사이즈와 값이 다를경우 임의로 벡터에 남은 퍼센트를 분배해서 넣어줌.
	// 루프를 돌아야하므로 효율성을 높이려면 사이즈를 맞춰주면 좋음.
	_uint RandomChoose(vector<_float> Weights, _uint iChooseSize);
	// 반지름 길이를 입력하면 그 반지름 내부에서 방향벡터를 랜덤하게 뽑아주는 함수입니다.
	_float4 Get_RandomVectorInSphere(_float fRadius);
	// FilePath 내의 모든 파일을 순회하면서 Ext 확장자에 맞는 파일들을 OutVector에 넣어줍니다.
	HRESULT ReadFileInDirectory(_Inout_ vector<wstring>& OutVector, const _tchar* pFilePath, const _tchar* pExt);

private:
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CInput_Device*			m_pInput_Device = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
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

public:
	static void Release_Engine();
	virtual void Free() override;
};

END
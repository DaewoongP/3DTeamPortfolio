#pragma once

/* =============================================== */
// 
//	정 : 박대웅
//	부 :
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
	/*	타이머 추가 
		*구조체 미설정 시 디폴트 값 설정*  */
	HRESULT Add_Timer(const wstring & wstrTimerTag, _bool m_isRepeat = false, const _float fDuration = 0.f);
	/* 타이머 삭제 */
	HRESULT Remove_Timer(const wstring & wstrTimerTag);
	/* 타이머의 TimeAcc를 0.f로 초기화 */
	HRESULT Reset_Timer(const wstring & wstrTimerTag);
	/*	실질적인 시간 체크 함수.
		해당 태그의 시간이 Duration보다 작은 경우 false를 반환.
		크거나 같으면 true 반환 */
	_bool Check_Timer(const wstring & wstrTimerTag);
	/* 현재 월드 누적시간을 반환*/
	const _float& Get_World_TimeAcc() const;
	/* 현재 월드 틱을 반환 */
	_float Get_World_Tick() const;
	/* 기존에 추가한 타이머의 누적시간을 반환 */
	_float Get_TimeAcc(const wstring & wstrTimerTag) const;
	/* 월드 누적시간 초기화 (사용할 경우 팀원들한테 미리 얘기하세요) */
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
	// 카메라에서 뷰, 투영 행렬 세팅
	void Set_Transform(CPipeLine::D3DTRANSFORMSTATE eTransformState, _float4x4 TransformStateMatrix);
	// 카메라에서 Far 값 세팅.
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
	_float3 PolarToCartesian(_float _fLength, _float _fTheta, _float _fPhi);
	_float4x4 RightUpLook_In_Vectors(_float3 vDestPos, _float3 vSourPos);

public: /* For.PhysX_Manager */
	PxPhysics* Get_Physics() const;
	PxScene* Get_PhysxScene() const;
	PxControllerManager* Get_ControllerManager() const;
	cloth::Factory* Get_ClothFactory() const;
	// 1. vOrigin : 레이 시작지점 2. vDir : 방향 3. fMaxDist : 최대거리 4. pHitPosition : (out)레이가 충돌한 위치 5. pDist : (out)충돌한 거리 
	// 6. iMaxHits : 레이를 맞을 수 있는 최대 개수 7. RaycastFlag : dynamic / static / all 중에 레이와 충돌할 객체 타입 (static에 하이트맵도 현재 포함중인거 생각해야합니다.)
	// 반환 : 충돌 했을 시 true
	_bool RayCast(_float3 vOrigin, _float3 vDir, _float fMaxDist = PX_MAX_F32, _Inout_ _float3* pHitPosition = nullptr, _Inout_ _float* pDist = nullptr, _uint iMaxHits = 1, CPhysX_Manager::RayCastQueryFlag RaycastFlag = CPhysX_Manager::RAY_ALL);
	// 1. pContext : Dx11 DeviceContext 2. hWnd : 클라이언트 핸들 3. fMaxDist : 최대거리 4. pHitPosition : (out)레이가 충돌한 위치 5. pDist : (out)충돌한 거리 
	// 6. iMaxHits : 레이를 맞을 수 있는 최대 개수 7. RaycastFlag : dynamic / static / all 중에 레이와 충돌할 객체 타입 (static에 하이트맵도 현재 포함중인거 생각해야합니다.)
	// 반환 : 충돌 했을 시 true
	_bool Mouse_RayCast(HWND hWnd, ID3D11DeviceContext* pContext, _float fMaxDist = PX_MAX_F32, _Inout_ _float3* pHitPosition = nullptr, _Inout_ _float* pDist = nullptr, _uint iMaxHits = 1, CPhysX_Manager::RayCastQueryFlag RaycastFlag = CPhysX_Manager::RAY_ALL);

public:	/* For.Camera_Manager */
	//컷씬 카메라 데이터를 담는다.
	HRESULT Read_CutSceneCamera(const _tchar* _CutSceneTag, const _tchar* _CutScenePath);
	//컷씬 재생을 위한 큐 추가
	HRESULT Add_CutScene(const _tchar* _CutSceneTag);
	//컷씬 재생을 위한 큐 추가
	HRESULT Add_CutScene(CUTSCENECAMERADESC& _CutSceneCameraDesc);
	//오프셋 카메라 데이터를 담는다.
	HRESULT Read_OffSetCamera(const _tchar* _OffSetTag, const _tchar* _OffSetPath);
	//오프셋 재생을 위한 큐 추가
	HRESULT Add_OffSetCamera(const _tchar* _OffSetTag);
	//카메라 추가
	HRESULT Add_Camera(const _tchar* _CameraTag, class CCamera* _pCamera);
	//카메라 변경
	HRESULT Set_Camera(const _tchar* _CameraTag);
	//카메라 찾기
	class CCamera* Find_Camera(const _tchar* _CameraTag);
	//컷씬 중지
	void Stop_CutScene();

public: /* For.RenderTaget_Manager*/
	class CRenderTarget* Find_RenderTarget(const _tchar* pTargetTag);

public: /* For. String_Manager */
	// 동적배열로 문자열을 할당해주는 함수
	// 게임이 종료되기 전까지 살아있는 문자열입니다.
	_char* Make_Char(const _char* pMakeChar);
	// 동적배열로 문자열을 할당해주는 함수
	// 게임이 종료되기 전까지 살아있는 문자열입니다.
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
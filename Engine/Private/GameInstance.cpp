#include "..\Public\GameInstance.h"
#include "Frustum.h"
#include "Calculator.h"
#include "Font_Manager.h"
#include "Level_Manager.h"
#include "Timer_Manager.h"
#include "Graphic_Device.h"
#include "Camera_Manager.h"
#include "String_Manager.h"
#include "RenderTarget_Manager.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphic_Device{ CGraphic_Device::GetInstance() }
	, m_pLevel_Manager{ CLevel_Manager::GetInstance() }
	, m_pTimer_Manager{ CTimer_Manager::GetInstance() }
	, m_pComponent_Manager{ CComponent_Manager::GetInstance() }
	, m_pInput_Device{ CInput_Device::GetInstance() }
	, m_pPipeLine{ CPipeLine::GetInstance() }
	, m_pCollision_Manager{ CCollision_Manager::GetInstance() }
	, m_pFont_Manager{ CFont_Manager::GetInstance() }
	, m_pFrustum{ CFrustum::GetInstance() }
	, m_pRenderTarget_Manager{ CRenderTarget_Manager::GetInstance() }
	, m_pLight_Manager{ CLight_Manager::GetInstance() }
	, m_pSound_Manager{ CSound_Manager::GetInstance() }
	, m_pCalculator{ CCalculator::GetInstance() }
	, m_pPhysX_Manager{ CPhysX_Manager::GetInstance() }
	, m_pCamera_Manager{ CCamera_Manager::GetInstance() }
	, m_pString_Manager{ CString_Manager::GetInstance() }
{
	Safe_AddRef(m_pFrustum);
	Safe_AddRef(m_pFont_Manager);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pCollision_Manager);
	Safe_AddRef(m_pRenderTarget_Manager);
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pSound_Manager);
	Safe_AddRef(m_pCalculator);
	Safe_AddRef(m_pPhysX_Manager);
	Safe_AddRef(m_pCamera_Manager);
	Safe_AddRef(m_pString_Manager);
}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const GRAPHICDESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
	NULL_CHECK_RETURN_MSG(m_pGraphic_Device, E_FAIL, TEXT("Graphic Device NULL"));

	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eWinMode, GraphicDesc.iViewportSizeX, GraphicDesc.iViewportSizeY, ppDevice, ppContext)))
		return E_FAIL;

	if (FAILED(Reserve_Engine(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pInput_Device->Ready_Input_Device(hInst, GraphicDesc.hWnd)))
		return E_FAIL;

	if (FAILED(m_pFrustum->Initialize()))
		return E_FAIL;

	if (FAILED(m_pSound_Manager->Initialize()))
		return E_FAIL;

	if (FAILED(m_pPhysX_Manager->Initialize(*ppDevice, *ppContext)))
		return E_FAIL;

	if (FAILED(m_pCamera_Manager->Initialize_CameraManager()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameInstance::Reserve_Engine(_uint iNumLevels)
{
	if (FAILED(m_pComponent_Manager->Reserve_Containers(iNumLevels)))
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Tick_Engine(_float fTimeDelta)
{
	NULL_CHECK_RETURN_MSG(m_pLevel_Manager, , TEXT("Level_Manager NULL"));
	NULL_CHECK_RETURN_MSG(m_pPipeLine, , TEXT("PipeLine NULL"));
	NULL_CHECK_RETURN_MSG(m_pInput_Device, , TEXT("Input_Device NULL"));
	NULL_CHECK_RETURN_MSG(m_pCollision_Manager, , TEXT("Collsion_Manager NULL"));

	m_pTimer_Manager->Tick(fTimeDelta);

	m_pInput_Device->Tick();

	m_pComponent_Manager->Tick(fTimeDelta);

	m_pCamera_Manager->Tick(fTimeDelta);

	m_pPipeLine->Tick();

	m_pFrustum->Tick();
	
	m_pPhysX_Manager->Tick(fTimeDelta);

	m_pComponent_Manager->Late_Tick(fTimeDelta);

	m_pCollision_Manager->Tick();

	m_pLevel_Manager->Tick(fTimeDelta);
}

void CGameInstance::Clear_LevelResources(_uint iLevelIndex)
{
	NULL_CHECK_RETURN_MSG(m_pComponent_Manager, , TEXT("Component_Manager NULL"));

	m_pComponent_Manager->Clear_LevelResources(iLevelIndex);
}

void CGameInstance::Clear_Resources()
{
	NULL_CHECK_RETURN_MSG(m_pCamera_Manager, , TEXT("Camera_Manager NULL"));
	NULL_CHECK_RETURN_MSG(m_pTimer_Manager, , TEXT("Timer_Manager NULL"));

	m_pCamera_Manager->Clear();
	m_pTimer_Manager->Clear();
}

_float2 CGameInstance::Get_ViewPortSize(ID3D11DeviceContext* pContext)
{
	_uint iNumViewport = 1;
	D3D11_VIEWPORT Viewport;
	pContext->RSGetViewports(&iNumViewport, &Viewport);
	return _float2(Viewport.Width, Viewport.Height);
}

HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	NULL_CHECK_RETURN_MSG(m_pGraphic_Device, E_FAIL, TEXT("Graphic_Device NULL"));

	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	NULL_CHECK_RETURN_MSG(m_pGraphic_Device, E_FAIL, TEXT("Graphic_Device NULL"));
	
	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	NULL_CHECK_RETURN_MSG(m_pGraphic_Device, E_FAIL, TEXT("Graphic_Device NULL"));

	return m_pGraphic_Device->Present();
}

HRESULT CGameInstance::Bind_BackBuffer()
{
	NULL_CHECK_RETURN_MSG(m_pGraphic_Device, E_FAIL, TEXT("Graphic_Device NULL"));

	return m_pGraphic_Device->Bind_BackBuffer();
}

HRESULT CGameInstance::Add_Timer(const wstring& wstrTimerTag, _bool m_isRepeat, const _float fDuration)
{
	NULL_CHECK_RETURN_MSG(m_pTimer_Manager, E_FAIL, TEXT("Time_Manager NULL"));

	return m_pTimer_Manager->Add_Timer(wstrTimerTag, m_isRepeat, fDuration);
}

HRESULT CGameInstance::Remove_Timer(const wstring& wstrTimerTag)
{
	NULL_CHECK_RETURN_MSG(m_pTimer_Manager, E_FAIL, TEXT("Time_Manager NULL"));

	return m_pTimer_Manager->Remove_Timer(wstrTimerTag);
}

HRESULT CGameInstance::Reset_Timer(const wstring& wstrTimerTag)
{
	NULL_CHECK_RETURN_MSG(m_pTimer_Manager, E_FAIL, TEXT("Time_Manager NULL"));

	return m_pTimer_Manager->Reset_Timer(wstrTimerTag);
}

_bool CGameInstance::Check_Timer(const wstring& wstrTimerTag)
{
	NULL_CHECK_RETURN_MSG(m_pTimer_Manager, false, TEXT("Time_Manager NULL"));

	return m_pTimer_Manager->Check_Timer(wstrTimerTag);
}

const _float& CGameInstance::Get_World_TimeAcc() const
{
	NULL_CHECK_MSG(m_pTimer_Manager, TEXT("Time_Manager NULL"));

	return m_pTimer_Manager->Get_World_TimeAcc();
}

_float CGameInstance::Get_World_Tick() const
{
	NULL_CHECK_RETURN_MSG(m_pTimer_Manager, 0.f, TEXT("Time_Manager NULL"));

	return m_pTimer_Manager->Get_World_Tick();
}

_float CGameInstance::Get_TimeAcc(const wstring& wstrTimerTag) const
{
	NULL_CHECK_RETURN_MSG(m_pTimer_Manager, false, TEXT("Time_Manager NULL"));

	return m_pTimer_Manager->Get_TimeAcc(wstrTimerTag);
}

void CGameInstance::Reset_World_TimeAcc()
{
	NULL_CHECK_MSG(m_pTimer_Manager, TEXT("Time_Manager NULL"));

	return m_pTimer_Manager->Reset_World_TimeAcc();
}

HRESULT CGameInstance::Add_QueryTimer(const _tchar* pTimerTag)
{
	NULL_CHECK_RETURN_MSG(m_pTimer_Manager, E_FAIL, TEXT("Timer_Manager NULL"));

	return m_pTimer_Manager->Add_QueryTimer(pTimerTag);
}

void CGameInstance::Tick_QueryTimer(const _tchar* pTimerTag)
{
	NULL_CHECK_RETURN_MSG(m_pTimer_Manager, , TEXT("Timer_Manager NULL"));

	m_pTimer_Manager->Tick_QueryTimer(pTimerTag);
}

_float CGameInstance::Get_QueryTimeDelta(const _tchar* pTimerTag)
{
	NULL_CHECK_RETURN_MSG(m_pTimer_Manager, 0.f, TEXT("Timer_Manager NULL"));

	return m_pTimer_Manager->Get_QueryTimeDelta(pTimerTag);
}

HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel* pNewLevel)
{
	NULL_CHECK_RETURN_MSG(m_pLevel_Manager, E_FAIL, TEXT("Level_Manager NULL"));

	return m_pLevel_Manager->Open_Level(iLevelIndex, pNewLevel);
}

CLevel* CGameInstance::Get_CurrentLevel()
{
	NULL_CHECK_RETURN_MSG(m_pLevel_Manager, nullptr, TEXT("Level_Manager NULL"));

	return m_pLevel_Manager->Get_CurrentLevel();
}

HRESULT CGameInstance::Render_Level()
{
	NULL_CHECK_RETURN_MSG(m_pLevel_Manager, E_FAIL, TEXT("Level_Manager NULL"));

	return m_pLevel_Manager->Render();
}

HRESULT CGameInstance::Add_Scene(const _tchar* pSceneTag, const _tchar* pLayerTag)
{
	NULL_CHECK_RETURN_MSG(m_pLevel_Manager, E_FAIL, TEXT("Level_Manager NULL"));

	return m_pLevel_Manager->Add_Scene(pSceneTag, pLayerTag);
}

CComponent* CGameInstance::Find_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag)
{
	NULL_CHECK_RETURN_MSG(m_pComponent_Manager, nullptr, TEXT("Component_Manager NULL"));

	return m_pComponent_Manager->Find_Prototype(iLevelIndex, pPrototypeTag);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype, _bool isFailedSkip)
{
	NULL_CHECK_RETURN_MSG(m_pComponent_Manager, E_FAIL, TEXT("Component_Manager NULL"));

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, pPrototypeTag, pPrototype, isFailedSkip);
}

HRESULT CGameInstance::Add_Component(_uint iPrototypeLevelIndex, _uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pLayerTag, const _tchar* pComponentTag, void* pArg)
{
	NULL_CHECK_RETURN_MSG(m_pComponent_Manager, E_FAIL, TEXT("Component_Manager NULL"));

	return m_pComponent_Manager->Add_Component(iPrototypeLevelIndex, iLevelIndex, pPrototypeTag, pLayerTag, pComponentTag, pArg);
}

CComponent* CGameInstance::Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg)
{
	NULL_CHECK_RETURN_MSG(m_pComponent_Manager, nullptr, TEXT("Component_Manager NULL"));

	return m_pComponent_Manager->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
}

CComponent* CGameInstance::Find_Component_In_Layer(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag)
{
	NULL_CHECK_RETURN_MSG(m_pComponent_Manager, nullptr, TEXT("Component_Manager NULL"));

	return m_pComponent_Manager->Find_Component_In_Layer(iLevelIndex, pLayerTag, pComponentTag);
}

unordered_map<const _tchar*, class CComponent*>* CGameInstance::Find_Components_In_Layer(_uint iLevelIndex, const _tchar* pLayerTag)
{
	NULL_CHECK_RETURN_MSG(m_pComponent_Manager, nullptr, TEXT("Component_Manager NULL"));

	return m_pComponent_Manager->Find_Components_In_Layer(iLevelIndex, pLayerTag);
}

CLayer* CGameInstance::Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag)
{
	NULL_CHECK_RETURN_MSG(m_pComponent_Manager, nullptr, TEXT("Component_Manager NULL"));

	return m_pComponent_Manager->Find_Layer(iLevelIndex, pLayerTag);
}

HRESULT CGameInstance::Clear_Layer(_uint iLevelIndex, const _tchar* pLayerTag)
{
	NULL_CHECK_RETURN_MSG(m_pComponent_Manager, E_FAIL, TEXT("Component_Manager NULL"));

	return m_pComponent_Manager->Clear_Layer(iLevelIndex, pLayerTag);
}

HRESULT CGameInstance::Delete_Component(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag)
{
	NULL_CHECK_RETURN_MSG(m_pComponent_Manager, E_FAIL, TEXT("Component_Manager NULL"));

	return m_pComponent_Manager->Delete_Component(iLevelIndex, pLayerTag, pComponentTag);
}

void CGameInstance::Set_CurrentScene(const _tchar* pSceneTag, _bool isPhysXSimulation)
{
	NULL_CHECK_RETURN_MSG(m_pComponent_Manager, , TEXT("Component_Manager NULL"));

	return m_pComponent_Manager->Set_CurrentScene(pSceneTag, isPhysXSimulation);
}

const _tchar* CGameInstance::Get_CurrentSceneTag() const
{
	NULL_CHECK_RETURN_MSG(m_pComponent_Manager, nullptr, TEXT("Component_Manager NULL"));

	return m_pComponent_Manager->Get_CurrentSceneTag();
}

_bool	CGameInstance::Get_DIKeyState(_ubyte ubyKeyID, CInput_Device::KEYSTATE eState)
{
	NULL_CHECK_RETURN_MSG(m_pInput_Device, 0, TEXT("Input_Device NULL"));

	return m_pInput_Device->Get_DIKeyState(ubyKeyID, eState);
}

_bool	CGameInstance::Get_DIMouseState(CInput_Device::MOUSEKEYSTATE eMouseID, CInput_Device::KEYSTATE eState)
{
	NULL_CHECK_RETURN_MSG(m_pInput_Device, 0, TEXT("Input_Device NULL"));

	return m_pInput_Device->Get_DIMouseState(eMouseID, eState);
}

_long	CGameInstance::Get_DIMouseMove(CInput_Device::MOUSEMOVESTATE eMouseMoveID)
{
	NULL_CHECK_RETURN_MSG(m_pInput_Device, 0, TEXT("Input_Device NULL"));

	return m_pInput_Device->Get_DIMouseMove(eMouseMoveID);
}

void CGameInstance::Set_Transform(CPipeLine::D3DTRANSFORMSTATE eTransformState, _float4x4 TransformStateMatrix)
{
	NULL_CHECK_RETURN_MSG(m_pPipeLine, , TEXT("PipeLine NULL"));

	return m_pPipeLine->Set_Transform(eTransformState, TransformStateMatrix);
}

void CGameInstance::Set_CameraFar(_float fCamFar)
{
	NULL_CHECK_RETURN_MSG(m_pPipeLine, , TEXT("PipeLine NULL"));

	return m_pPipeLine->Set_CameraFar(fCamFar);
}

const _float4x4* CGameInstance::Get_TransformMatrix(CPipeLine::D3DTRANSFORMSTATE eTransformState)
{
	NULL_CHECK_RETURN_MSG(m_pPipeLine, nullptr, TEXT("PipeLine NULL"));

	return m_pPipeLine->Get_TransformMatrix(eTransformState);
}

const _float4x4* CGameInstance::Get_LightTransformMatrix(CPipeLine::D3DTRANSFORMSTATE eTransformState)
{
	NULL_CHECK_RETURN_MSG(m_pPipeLine, nullptr, TEXT("PipeLine NULL"));

	return m_pPipeLine->Get_LightTransformMatrix(eTransformState);
}

const _float4x4* CGameInstance::Get_TransformMatrix_Inverse(CPipeLine::D3DTRANSFORMSTATE eTransformState)
{
	NULL_CHECK_RETURN_MSG(m_pPipeLine, nullptr, TEXT("PipeLine NULL"));

	return m_pPipeLine->Get_TransformMatrix_Inverse(eTransformState);
}

const _float4* CGameInstance::Get_CamPosition()
{
	NULL_CHECK_RETURN_MSG(m_pPipeLine, nullptr, TEXT("PipeLine NULL"));

	return m_pPipeLine->Get_CamPosition();
}

const _float3* CGameInstance::Get_CamUp()
{
	NULL_CHECK_RETURN_MSG(m_pPipeLine, nullptr, TEXT("PipeLine NULL"));

	return m_pPipeLine->Get_CamUp();
}

const _float3* CGameInstance::Get_CamLook()
{
	NULL_CHECK_RETURN_MSG(m_pPipeLine, nullptr, TEXT("PipeLine NULL"));

	return m_pPipeLine->Get_CamLook();
}

const _float3* CGameInstance::Get_CamRight()
{
	NULL_CHECK_RETURN_MSG(m_pPipeLine, nullptr, TEXT("PipeLine NULL"));

	return m_pPipeLine->Get_CamRight();
}

const _float* CGameInstance::Get_CamFar()
{
	NULL_CHECK_RETURN_MSG(m_pPipeLine, nullptr, TEXT("PipeLine NULL"));

	return m_pPipeLine->Get_CamFar();
}

HRESULT CGameInstance::Add_Collider(COLLISIONDESC::COLTYPE eCollisionType, CCollider* pCollider)
{
	NULL_CHECK_RETURN_MSG(m_pCollision_Manager, E_FAIL, TEXT("Collision NULL"));

	return m_pCollision_Manager->Add_Collider(eCollisionType, pCollider);
}

void CGameInstance::ClearColliders()
{
	NULL_CHECK_RETURN_MSG(m_pCollision_Manager, , TEXT("Collision NULL"));

	return m_pCollision_Manager->ClearColliders();
}

HRESULT CGameInstance::Add_Fonts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontTag, const _tchar* pFontFilePath)
{
	NULL_CHECK_RETURN_MSG(m_pFont_Manager, E_FAIL, TEXT("Font NULL"));

	return m_pFont_Manager->Add_Fonts(pDevice, pContext, pFontTag, pFontFilePath);
}

HRESULT CGameInstance::Render_Font(const _tchar* pFontTag, const _tchar* pText, const _float2& Position, _float4 vColor, _float fRotation, const _float2& vOrigin, _float fScale)
{
	NULL_CHECK_RETURN_MSG(m_pFont_Manager, E_FAIL, TEXT("Font NULL"));

	return m_pFont_Manager->Render_Font(pFontTag, pText, Position, vColor, fRotation, vOrigin, fScale);
}

_bool CGameInstance::isIn_WorldFrustum(_float4 vWorldPos, _float fRange)
{
	NULL_CHECK_RETURN_MSG(m_pFrustum, false, TEXT("Frustum NULL"));

	return m_pFrustum->isIn_WorldFrustum(vWorldPos, fRange);
}

const CLight::LIGHTDESC* CGameInstance::Get_Light(_uint iIndex)
{
	NULL_CHECK_RETURN_MSG(m_pLight_Manager, nullptr, TEXT("Light NULL"));

	return m_pLight_Manager->Get_Light(iIndex);
}

const _float4x4* CGameInstance::Get_LightView()
{
	//NULL_CHECK_RETURN_MSG(m_pLight_Manager, nullptr, TEXT("Light NULL"));

	if(nullptr==m_pLight_Manager)
	{
		MSG_BOX("LightNULL");
		return nullptr;
	}

	return m_pLight_Manager->Get_LightView();
}

const _float4x4* CGameInstance::Get_LightProj()
{
	//NULL_CHECK_RETURN_MSG(m_pLight_Manager, nullptr, TEXT("Light NULL"));
	if (nullptr == m_pLight_Manager)
	{
		MSG_BOX("LightNULL");
		return nullptr;
	}
	return m_pLight_Manager->Get_LightProj();
}

void CGameInstance::Set_Light(_uint iIndex, CLight::LIGHTDESC LightDesc)
{
	NULL_CHECK_RETURN_MSG(m_pLight_Manager, , TEXT("Light NULL"));

	return m_pLight_Manager->Set_Light(iIndex, LightDesc);
}

CLight* CGameInstance::Add_Lights(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CLight::LIGHTDESC& LightDesc)
{
	NULL_CHECK_RETURN_MSG(m_pLight_Manager, nullptr, TEXT("Light NULL"));

	return m_pLight_Manager->Add_Lights(pDevice,pContext,LightDesc);
}

HRESULT CGameInstance::Delete_Lights(_uint iIndex,const _char* Name)
{
	NULL_CHECK_RETURN_MSG(m_pLight_Manager, E_FAIL, TEXT("Light NULL"));

	return m_pLight_Manager->Delete_Lights(iIndex,Name);
}

HRESULT CGameInstance::Clear_Lights()
{
	NULL_CHECK_RETURN_MSG(m_pLight_Manager, E_FAIL, TEXT("Light NULL"));

	return m_pLight_Manager->Clear_Lights();
}

HRESULT CGameInstance::Add_Sounds(const _tchar* szSoundFilePath)
{
	NULL_CHECK_RETURN_MSG(m_pSound_Manager, E_FAIL, TEXT("Sound NULL"));

	return m_pSound_Manager->Add_Sounds(szSoundFilePath);
}

HRESULT CGameInstance::Play_Sound(const _tchar* szSoundTag, CSound_Manager::SOUNDCHANNEL eChannel, _float fVolume, _bool bForcePlay)
{
	NULL_CHECK_RETURN_MSG(m_pSound_Manager, E_FAIL, TEXT("Sound NULL"));

	return m_pSound_Manager->Play_Sound(szSoundTag, eChannel, fVolume, bForcePlay);
}

HRESULT CGameInstance::Play_Sound(const _tchar* pSoundTag, _uint iNumSounds, CSound_Manager::SOUNDCHANNEL eChannel, _float fVolume, _bool bForcePlay)
{
	NULL_CHECK_RETURN_MSG(m_pSound_Manager, E_FAIL, TEXT("Sound NULL"));

	return m_pSound_Manager->Play_Sound(pSoundTag, iNumSounds, eChannel, fVolume, bForcePlay);
}

HRESULT CGameInstance::Play_BGM(const _tchar* szSoundTag, _float fVolume)
{
	NULL_CHECK_RETURN_MSG(m_pSound_Manager, E_FAIL, TEXT("Sound NULL"));

	return m_pSound_Manager->Play_BGM(szSoundTag, fVolume);
}

HRESULT CGameInstance::Stop_Sound(CSound_Manager::SOUNDCHANNEL eChannel)
{
	NULL_CHECK_RETURN_MSG(m_pSound_Manager, E_FAIL, TEXT("Sound NULL"));

	return m_pSound_Manager->Stop_Sound(eChannel);
}

HRESULT CGameInstance::Pause_Sound(CSound_Manager::SOUNDCHANNEL eChannel)
{
	NULL_CHECK_RETURN_MSG(m_pSound_Manager, E_FAIL, TEXT("Sound NULL"));

	return m_pSound_Manager->Pause_Sound(eChannel);
}

HRESULT CGameInstance::Restart_Sound(CSound_Manager::SOUNDCHANNEL eChannel)
{
	NULL_CHECK_RETURN_MSG(m_pSound_Manager, E_FAIL, TEXT("Sound NULL"));

	return m_pSound_Manager->Restart_Sound(eChannel);
}

HRESULT CGameInstance::Stop_AllSound()
{
	NULL_CHECK_RETURN_MSG(m_pSound_Manager, E_FAIL, TEXT("Sound NULL"));

	return m_pSound_Manager->Stop_AllSound();
}

HRESULT CGameInstance::Set_ChannelVolume(CSound_Manager::SOUNDCHANNEL eChannel, _float fVolume)
{
	NULL_CHECK_RETURN_MSG(m_pSound_Manager, E_FAIL, TEXT("Sound NULL"));

	return m_pSound_Manager->Set_ChannelVolume(eChannel, fVolume);
}

HRESULT CGameInstance::Get_MouseRay(ID3D11DeviceContext* pContext, HWND hWnd, _float4x4 PickingWorldMatrix_Inverse, _Inout_ _float4* vRayPos, _Inout_ _float4* vRayDir)
{
	NULL_CHECK_RETURN_MSG(m_pCalculator, E_FAIL, TEXT("Calculator NULL"));

	return m_pCalculator->Get_MouseRay(pContext, hWnd, PickingWorldMatrix_Inverse, vRayPos, vRayDir);
}

HRESULT CGameInstance::Get_WorldMouseRay(ID3D11DeviceContext* pContext, HWND hWnd, _Inout_ _float4* vRayPos, _Inout_ _float4* vRayDir)
{
	NULL_CHECK_RETURN_MSG(m_pCalculator, E_FAIL, TEXT("Calculator NULL"));

	return m_pCalculator->Get_WorldMouseRay(pContext, hWnd, vRayPos, vRayDir);
}

_bool CGameInstance::IsMouseInClient(ID3D11DeviceContext* pContext, HWND hWnd)
{
	NULL_CHECK_RETURN_MSG(m_pCalculator, false, TEXT("Calculator NULL"));

	return m_pCalculator->IsMouseInClient(pContext, hWnd);
}

_uint CGameInstance::RandomChoose(vector<_float> Weights, _uint iChooseSize)
{
	NULL_CHECK_RETURN_MSG(m_pCalculator, 0, TEXT("Calculator NULL"));

	return m_pCalculator->RandomChoose(Weights, iChooseSize);
}

_float4 CGameInstance::Get_RandomVectorInSphere(_float fRadius)
{
	NULL_CHECK_RETURN_MSG(m_pCalculator, XMVectorZero(), TEXT("Calculator NULL"));

	return m_pCalculator->Get_RandomVectorInSphere(fRadius);
}

HRESULT CGameInstance::ReadFileInDirectory(_Inout_ vector<wstring>& OutVector, const _tchar* pFilePath, const _tchar* pExt)
{
	NULL_CHECK_RETURN_MSG(m_pCalculator, E_FAIL, TEXT("Calculator NULL"));

	return m_pCalculator->ReadFileInDirectory(OutVector, pFilePath, pExt);
}

_float3 CGameInstance::PolarToCartesian(_float _fLength, _float _fTheta, _float _fPhi)
{
	NULL_CHECK_RETURN_MSG(m_pCalculator, _float3(), TEXT("Calculator NULL"));

	return m_pCalculator->PolarToCartesian(_fLength, _fTheta, _fPhi);
}

_float2 CGameInstance::PolarToCartesian(_float _fLength, _float _fTheta)
{
	NULL_CHECK_RETURN_MSG(m_pCalculator, _float2(), TEXT("Calculator NULL"));

	return m_pCalculator->PolarToCartesian(_fLength, _fTheta);
}

_float3 CGameInstance::GetVectorSlerp(_float3 v1, _float3 v2, _float3 vUp, _float k, _float f)
{
	NULL_CHECK_RETURN_MSG(m_pCalculator, _float3(), TEXT("Calculator NULL"));

	return m_pCalculator->GetVectorSlerp(v1, v2, vUp, k, f);
}

_float4x4 CGameInstance::RightUpLook_In_Vectors(_float3 vSourPos, _float3 vDestPos)
{
	NULL_CHECK_RETURN_MSG(m_pCalculator, _float4x4(), TEXT("Calculator NULL"));

	return m_pCalculator->RightUpLook_In_Vectors(vSourPos, vDestPos);
}

PxPhysics* CGameInstance::Get_Physics() const
{
	NULL_CHECK_RETURN_MSG(m_pPhysX_Manager, nullptr, TEXT("PhysX_Manager NULL"));

	return m_pPhysX_Manager->Get_Physics();
}

PxScene* CGameInstance::Get_PhysxScene() const
{
	NULL_CHECK_RETURN_MSG(m_pPhysX_Manager, nullptr, TEXT("PhysX_Manager NULL"));

	return m_pPhysX_Manager->Get_PhysxScene();
}

PxControllerManager* CGameInstance::Get_ControllerManager() const
{
	NULL_CHECK_RETURN_MSG(m_pPhysX_Manager, nullptr, TEXT("PhysX_Manager NULL"));

	return m_pPhysX_Manager->Get_ControllerManager();
}

cloth::Factory* CGameInstance::Get_ClothFactory() const
{
	NULL_CHECK_RETURN_MSG(m_pPhysX_Manager, nullptr, TEXT("PhysX_Manager NULL"));

	return m_pPhysX_Manager->Get_ClothFactory();
}

void CGameInstance::Set_Simulation(_bool isSimulation)
{
	NULL_CHECK_RETURN_MSG(m_pPhysX_Manager, , TEXT("PhysX_Manager NULL"));

	m_pPhysX_Manager->Set_Simulation(isSimulation);
}

_bool CGameInstance::RayCast(_float3 vOrigin, _float3 vDir, _float fMaxDist, _Inout_ _float3* pHitPosition, _Inout_ _float* pDist, _uint iMaxHits, CPhysX_Manager::RayCastQueryFlag RaycastFlag)
{
	NULL_CHECK_RETURN_MSG(m_pPhysX_Manager, false, TEXT("PhysX_Manager NULL"));

	return m_pPhysX_Manager->RayCast(vOrigin, vDir, fMaxDist, pHitPosition, pDist, iMaxHits, RaycastFlag);
}

_bool CGameInstance::Mouse_RayCast(HWND hWnd, ID3D11DeviceContext* pContext, _float fMaxDist, _Inout_ _float3* pHitPosition, _Inout_ _float* pDist, _uint iMaxHits, CPhysX_Manager::RayCastQueryFlag RaycastFlag)
{
	NULL_CHECK_RETURN_MSG(m_pPhysX_Manager, false, TEXT("PhysX_Manager NULL"));

	return m_pPhysX_Manager->Mouse_RayCast(hWnd, pContext, fMaxDist, pHitPosition, pDist, iMaxHits, RaycastFlag);
}

void CGameInstance::Update_PhysxScene()
{
	NULL_CHECK_RETURN_MSG(m_pPhysX_Manager, , TEXT("PhysX_Manager NULL"));

	m_pPhysX_Manager->Tick(1 / 60.f);
}

HRESULT CGameInstance::Read_CutSceneCamera(const _tchar* _CutSceneTag, const _tchar* _CutScenePath)
{
	NULL_CHECK_RETURN_MSG(m_pCamera_Manager, E_FAIL, TEXT("Camera NULL"));

	return m_pCamera_Manager->Read_CutSceneCamera(_CutSceneTag, _CutScenePath);
}

HRESULT CGameInstance::Add_CutScene(const _tchar* _CutSceneTag)
{
	NULL_CHECK_RETURN_MSG(m_pCamera_Manager, E_FAIL, TEXT("Camera NULL"));

	return m_pCamera_Manager->Add_CutScene(_CutSceneTag);
}

HRESULT CGameInstance::Add_CutScene(CUTSCENECAMERADESC& _CutSceneCameraDesc)
{
	NULL_CHECK_RETURN_MSG(m_pCamera_Manager, E_FAIL, TEXT("Camera NULL"));

	return m_pCamera_Manager->Add_CutScene(_CutSceneCameraDesc);
}

HRESULT CGameInstance::Read_OffSetCamera(const _tchar* _OffSetTag, const _tchar* _OffSetPath)
{
	NULL_CHECK_RETURN_MSG(m_pCamera_Manager, E_FAIL, TEXT("Camera NULL"));

	return m_pCamera_Manager->Read_OffSetCamera(_OffSetTag, _OffSetPath);
}

HRESULT CGameInstance::Add_OffSetCamera(const _tchar* _OffSetTag)
{
	NULL_CHECK_RETURN_MSG(m_pCamera_Manager, E_FAIL, TEXT("Camera NULL"));

	return m_pCamera_Manager->Add_OffSetCamera(_OffSetTag);
}

HRESULT CGameInstance::Add_Camera(const _tchar* _CameraTag, CCamera* _pCamera)
{
	NULL_CHECK_RETURN_MSG(m_pCamera_Manager, E_FAIL, TEXT("Camera NULL"));

	return m_pCamera_Manager->Add_Camera(_CameraTag, _pCamera);
}

HRESULT CGameInstance::Set_Camera(const _tchar* _CameraTag)
{
	NULL_CHECK_RETURN_MSG(m_pCamera_Manager, E_FAIL, TEXT("Camera NULL"));

	return m_pCamera_Manager->Set_Camera(_CameraTag);
}

CCamera* CGameInstance::Find_Camera(const _tchar* _CameraTag)
{
	NULL_CHECK_RETURN_MSG(m_pCamera_Manager, nullptr, TEXT("Camera NULL"));

	return m_pCamera_Manager->Find_Camera(_CameraTag);
}

void CGameInstance::Stop_CutScene()
{
	NULL_CHECK_RETURN_MSG(m_pCamera_Manager, , TEXT("Camera NULL"));

	return m_pCamera_Manager->Stop_CutScene();
}

CRenderTarget* CGameInstance::Find_RenderTarget(const _tchar* pTargetTag)
{
	NULL_CHECK_RETURN_MSG(m_pRenderTarget_Manager, nullptr, TEXT("RenderTarget_Manager NULL"));

	return m_pRenderTarget_Manager->Find_RenderTarget(pTargetTag);
}

HRESULT CGameInstance::Add_Prototype_Textures(_uint iLevel, ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pPrototypeName, const _tchar* pTargetExtension, const _tchar* pDirectoryPath, _bool isFailedSkip)
{
	fs::directory_iterator iter(pDirectoryPath);

	while (iter != fs::end(iter))
	{
		const fs::directory_entry& FileEntry = *iter;

		// 대상이 폴더일 경우 재귀로 들어감.
		if (true == FileEntry.is_directory())
		{
			if (FAILED(Add_Prototype_Textures(iLevel, pDevice, pContext, pPrototypeName, pTargetExtension, FileEntry.path().wstring().c_str(), isFailedSkip)))
			{
				return E_FAIL;
			}
		}

		// 확장자가 있는지 검사.
		if (true == FileEntry.path().has_extension())
		{
			// 확장자를 비교.
			if (FileEntry.path().extension() == pTargetExtension)
			{
				// 태그 만드는 로직
				wstring wstrPrototypeTag = pPrototypeName; // Prototype_Component_Texture
				if (wstrPrototypeTag.back() != TEXT('_'))
					wstrPrototypeTag += TEXT('_'); // Prototype_Component_Texture_
				wstrPrototypeTag += FileEntry.path().stem().wstring(); // Prototype_Component_Texture_FileName
				
				// 경로 만드는 로직.
				wstring wstrFilePathName = pDirectoryPath; // "../../Resources/Default/Textures
				if (wstrFilePathName.back() != TEXT('/'))
					wstrFilePathName += TEXT('/'); // "../../Resources/Default/Textures/
				wstrFilePathName += FileEntry.path().filename().wstring(); // "../../Resources/Default/Textures/FileName.ext

				// 어떤 텍스처가 문제있는지 일일이 찾기 힘드므로
				// FAILED_CHECK_RETURN로 문제 있는 텍스처에서 바로 멈추게끔 처리.
				
				FAILED_CHECK_RETURN(Add_Prototype(iLevel, wstrPrototypeTag.c_str(),
					CTexture::Create(pDevice, pContext, wstrFilePathName.c_str()), isFailedSkip), E_FAIL);
				// 만약에 경로를 못찾았다는 오류가 뜨면 원본 파일이 손상되어있을 가능성이 있음. 직접 열어봐야함.
			}
		}
		
		iter++;
	}

	return S_OK;
}

HRESULT CGameInstance::Add_Prototype_Models(_uint iLevel, ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eType, const _tchar* pPrototypeName, const _tchar* pTargetExtension, const _tchar* pDirectoryPath, _bool isFailedSkip, _float4x4 PivotMatrix)
{
	fs::directory_iterator iter(pDirectoryPath);

	while (iter != fs::end(iter))
	{
		const fs::directory_entry& FileEntry = *iter;

		// 대상이 폴더일 경우 재귀로 들어감.
		if (true == FileEntry.is_directory())
		{
			if (FAILED(Add_Prototype_Models(iLevel, pDevice, pContext, eType, pPrototypeName, pTargetExtension, FileEntry.path().wstring().c_str(), isFailedSkip, PivotMatrix)))
			{
				return E_FAIL;
			}
		}

		// 확장자가 있는지 검사.
		if (true == FileEntry.path().has_extension())
		{
			// 확장자를 비교.
			if (FileEntry.path().extension() == pTargetExtension)
			{
				// 태그 만드는 로직
				wstring wstrPrototypeTag = pPrototypeName; // Prototype_Component_Model
				if (wstrPrototypeTag.back() != TEXT('_'))
					wstrPrototypeTag += TEXT('_'); // Prototype_Component_Model_
				wstrPrototypeTag += FileEntry.path().stem().wstring(); // Prototype_Component_Model_FileName

				// 경로 만드는 로직.
				wstring wstrFilePathName = pDirectoryPath; // "../../Resources/Default/Models
				if (wstrFilePathName.back() != TEXT('/'))
					wstrFilePathName += TEXT('/'); // "../../Resources/Default/Models/
				wstrFilePathName += FileEntry.path().filename().wstring(); // "../../Resources/Default/Models/FileName.ext

				// 어떤 파일에 문제있는지 일일이 찾기 힘드므로
				// FAILED_CHECK_RETURN로 문제 있는 파일에서 바로 멈추게끔 처리.
				FAILED_CHECK_RETURN(Add_Prototype(iLevel, wstrPrototypeTag.c_str(),
					CModel::Create(pDevice, pContext, eType, wstrFilePathName.c_str(), PivotMatrix), isFailedSkip), E_FAIL);
			}
		}

		++iter;
	}

	return S_OK;
}

_char* CGameInstance::Make_Char(const _char* pMakeChar)
{
	NULL_CHECK_RETURN_MSG(m_pString_Manager, nullptr, TEXT("String_Manager NULL"));

	return m_pString_Manager->Make_Char(pMakeChar);
}

_tchar* CGameInstance::Make_WChar(const _tchar* pMakeWChar)
{
	NULL_CHECK_RETURN_MSG(m_pString_Manager, nullptr, TEXT("String_Manager NULL"));

	return m_pString_Manager->Make_WChar(pMakeWChar);
}

HRESULT CGameInstance::Delete_Char(_char* pChar)
{
	NULL_CHECK_RETURN_MSG(m_pString_Manager, E_FAIL, TEXT("String_Manager NULL"));

	return m_pString_Manager->Delete_Char(pChar);
}

HRESULT CGameInstance::Delete_WChar(_tchar* pWChar)
{
	NULL_CHECK_RETURN_MSG(m_pString_Manager, E_FAIL, TEXT("String_Manager NULL"));

	return m_pString_Manager->Delete_WChar(pWChar);
}

void CGameInstance::Release_Engine()
{
	CGameInstance::GetInstance()->DestroyInstance();

	CCamera_Manager::GetInstance()->DestroyInstance();

	CPipeLine::GetInstance()->DestroyInstance();

	CComponent_Manager::GetInstance()->DestroyInstance();

	CLevel_Manager::GetInstance()->DestroyInstance();

	CPhysX_Manager::GetInstance()->DestroyInstance();

	CTimer_Manager::GetInstance()->DestroyInstance();

	CFont_Manager::GetInstance()->DestroyInstance();
	
	CFrustum::GetInstance()->DestroyInstance();

	CCollision_Manager::GetInstance()->DestroyInstance();

	CRenderTarget_Manager::GetInstance()->DestroyInstance();

	CSound_Manager::GetInstance()->DestroyInstance();

	CCalculator::GetInstance()->DestroyInstance();

	CLight_Manager::GetInstance()->DestroyInstance();

	CInput_Device::GetInstance()->DestroyInstance();

	CString_Manager::GetInstance()->DestroyInstance();

	CGraphic_Device::GetInstance()->DestroyInstance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pString_Manager);
	Safe_Release(m_pPhysX_Manager);
	Safe_Release(m_pCalculator);
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pRenderTarget_Manager);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pCollision_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pCamera_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pGraphic_Device);
}

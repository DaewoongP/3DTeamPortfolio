#include "..\Public\GameInstance.h"
#include "Frustum.h"
#include "Font_Manager.h"
#include "Level_Manager.h"
#include "Timer_Manager.h"
#include "Graphic_Device.h"
#include "Object_Manager.h"
#include "RenderTarget_Manager.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphic_Device{ CGraphic_Device::GetInstance() }
	, m_pLevel_Manager{ CLevel_Manager::GetInstance() }
	, m_pObject_Manager{ CObject_Manager::GetInstance() }
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
{
	Safe_AddRef(m_pFrustum);
	Safe_AddRef(m_pFont_Manager);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pCollision_Manager);
	Safe_AddRef(m_pRenderTarget_Manager);
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pSound_Manager);
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

	return S_OK;
}

HRESULT CGameInstance::Reserve_Engine(_uint iNumLevels)
{
	if (FAILED(m_pObject_Manager->Reserve_Containers(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pComponent_Manager->Reserve_Containers(iNumLevels)))
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Tick_Engine(_float fTimeDelta)
{
	NULL_CHECK_RETURN_MSG(m_pLevel_Manager, , TEXT("Level_Manager NULL"));
	NULL_CHECK_RETURN_MSG(m_pObject_Manager, , TEXT("Object_Manager NULL"));
	NULL_CHECK_RETURN_MSG(m_pPipeLine, , TEXT("PipeLine NULL"));
	NULL_CHECK_RETURN_MSG(m_pInput_Device, , TEXT("Input_Device NULL"));

	m_pInput_Device->Tick();
	m_pObject_Manager->Tick(fTimeDelta);
	m_pPipeLine->Tick();
	m_pFrustum->Tick();
	m_pObject_Manager->Late_Tick(fTimeDelta);
	m_pCollision_Manager->Tick();
	m_pLevel_Manager->Tick(fTimeDelta);
}

void CGameInstance::Clear_LevelResources(_uint iLevelIndex)
{
	NULL_CHECK_RETURN_MSG(m_pObject_Manager, , TEXT("Object_Manager NULL"));
	NULL_CHECK_RETURN_MSG(m_pComponent_Manager, , TEXT("Component_Manager NULL"));

	m_pObject_Manager->Clear_LevelResources(iLevelIndex);
	m_pComponent_Manager->Clear_LevelResources(iLevelIndex);
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

HRESULT CGameInstance::Add_Timer(const _tchar* pTimerTag)
{
	NULL_CHECK_RETURN_MSG(m_pTimer_Manager, E_FAIL, TEXT("Timer_Manager NULL"));

	return m_pTimer_Manager->Add_Timer(pTimerTag);
}

void CGameInstance::Tick_Timer(const _tchar* pTimerTag)
{
	NULL_CHECK_RETURN_MSG(m_pTimer_Manager, , TEXT("Timer_Manager NULL"));

	m_pTimer_Manager->Tick_Timer(pTimerTag);
}

_float CGameInstance::Get_TimeDelta(const _tchar* pTimerTag)
{
	NULL_CHECK_RETURN_MSG(m_pTimer_Manager, 0.f, TEXT("Timer_Manager NULL"));

	return m_pTimer_Manager->Get_TimeDelta(pTimerTag);
}

HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel* pNewLevel)
{
	NULL_CHECK_RETURN_MSG(m_pLevel_Manager, E_FAIL, TEXT("Level_Manager NULL"));

	return m_pLevel_Manager->Open_Level(iLevelIndex, pNewLevel);
}

_uint CGameInstance::Get_CurrentLevelIndex() const
{
	NULL_CHECK_RETURN_MSG(m_pLevel_Manager, 0, TEXT("Level_Manager NULL"));

	return m_pLevel_Manager->Get_CurrentLevelIndex();
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

HRESULT CGameInstance::Add_Prototype(const _tchar* pPrototypeTag, CGameObject* pPrototype)
{
	NULL_CHECK_RETURN_MSG(m_pObject_Manager, E_FAIL, TEXT("Object_Manager NULL"));
	
	return m_pObject_Manager->Add_Prototype(pPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Add_GameObject(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pLayerTag, const _tchar* pGameObjectTag, void* pArg)
{
	NULL_CHECK_RETURN_MSG(m_pObject_Manager, E_FAIL, TEXT("Object_Manager NULL"));

	return m_pObject_Manager->Add_GameObject(iLevelIndex, pPrototypeTag, pLayerTag, pGameObjectTag, pArg);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, CComponent* pPrototype)
{
	NULL_CHECK_RETURN_MSG(m_pComponent_Manager, E_FAIL, TEXT("Component_Manager NULL"));

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, pPrototypeTag, pPrototype);
}

CComponent* CGameInstance::Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg)
{
	NULL_CHECK_RETURN_MSG(m_pComponent_Manager, nullptr, TEXT("Component_Manager NULL"));

	return m_pComponent_Manager->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
}

HRESULT CGameInstance::Delete_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag)
{
	NULL_CHECK_RETURN_MSG(m_pComponent_Manager, E_FAIL, TEXT("Component_Manager NULL"));

	return m_pComponent_Manager->Delete_Prototype(iLevelIndex, pPrototypeTag);
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

vector<_float4> CGameInstance::Get_CulledPositions() const
{
	return m_pFrustum->Get_CulledPositions();
}

const CLight::LIGHTDESC* CGameInstance::Get_Light(_uint iIndex)
{
	NULL_CHECK_RETURN_MSG(m_pLight_Manager, nullptr, TEXT("Light NULL"));

	return m_pLight_Manager->Get_Light(iIndex);
}

void CGameInstance::Set_Light(_uint iIndex, CLight::LIGHTDESC LightDesc)
{
	NULL_CHECK_RETURN_MSG(m_pLight_Manager, , TEXT("Light NULL"));

	return m_pLight_Manager->Set_Light(iIndex, LightDesc);
}

CLight* CGameInstance::Add_Lights(const CLight::LIGHTDESC& LightDesc)
{
	NULL_CHECK_RETURN_MSG(m_pLight_Manager, nullptr, TEXT("Light NULL"));

	return m_pLight_Manager->Add_Lights(LightDesc);
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

void CGameInstance::Release_Engine()
{
	CGameInstance::GetInstance()->DestroyInstance();

	CPipeLine::GetInstance()->DestroyInstance();

	CObject_Manager::GetInstance()->DestroyInstance();

	CComponent_Manager::GetInstance()->DestroyInstance();

	CLevel_Manager::GetInstance()->DestroyInstance();

	CTimer_Manager::GetInstance()->DestroyInstance();

	CFont_Manager::GetInstance()->DestroyInstance();
	
	CFrustum::GetInstance()->DestroyInstance();

	CCollision_Manager::GetInstance()->DestroyInstance();

	CRenderTarget_Manager::GetInstance()->DestroyInstance();

	CSound_Manager::GetInstance()->DestroyInstance();

	CLight_Manager::GetInstance()->DestroyInstance();

	CInput_Device::GetInstance()->DestroyInstance();

	CGraphic_Device::GetInstance()->DestroyInstance();
}

void CGameInstance::Free()
{
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
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pGraphic_Device);
}

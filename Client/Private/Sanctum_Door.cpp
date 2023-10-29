#include "..\Public\Sanctum_Door.h"
#include "GameInstance.h"

#include "LightStand.h"

CSanctum_Door::CSanctum_Door(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapObject(pDevice, pContext)
{
}

CSanctum_Door::CSanctum_Door(const CSanctum_Door& rhs)
	: CMapObject(rhs)
{
}

HRESULT CSanctum_Door::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSanctum_Door::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("CSanctum_Door Argument is NULL");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSanctum_Door::Initialize_Level(_uint iCurrentLevelIndex)
{
	/* Com_Model */
	if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, m_ObjectDesc.wszTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CSanctum_Door Add_Component : (Com_Model)");
		__debugbreak();
		return E_FAIL;
	}

	// ���� ������ �ľ�
	wstring wsTargetName(TEXT("Prototype_Component_Model_"));
	wstring wsModelName(m_ObjectDesc.wszTag);

	_uint iLength = wsTargetName.size();
	wsModelName = wsModelName.substr(iLength);

	wstring wsLeft(TEXT("SM_SanctumDun_Door_Repository__L"));
	wstring wsRight(TEXT("SM_SanctumDun_Door_Repository__R"));

	if (0 == lstrcmp(wsModelName.c_str(), wsLeft.c_str()))
	{
		m_eDoorType = LEFTDOOR;
	}
	else if (0 == lstrcmp(wsModelName.c_str(), wsRight.c_str()))
	{
		m_eDoorType = RIGHTDOOR;
	}

	return S_OK;
}

void CSanctum_Door::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Door_Action(fTimeDelta);
}

void CSanctum_Door::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);
	}
}

void CSanctum_Door::Door_Action(_float fTimeDelta)
{
	if (false == m_isDoorAction)
		return;

	// �Ҹ�
	if (true == m_isSound)
	{
		BEGININSTANCE;
		pGameInstance->Play_Sound(TEXT("SanctumDoorOpen.wav"), 0.75f);
		ENDINSTANCE;

		m_isSound = false;
	}

	if(RIGHTDOOR == m_eDoorType)
		m_pTransform->Turn(_float3(0.f, 1.f, 0.f), XMConvertToRadians(m_fDoorTurn), fTimeDelta * 0.5f);
	else
		m_pTransform->Turn(_float3(0.f, 1.f, 0.f), XMConvertToRadians(-m_fDoorTurn), fTimeDelta * 0.5f);

	// DOORTURN ��ŭ �������� ȸ���� ����
	m_fDoorSpinValue += m_fDoorTurn * fTimeDelta * 0.5f;

	// ���� ���� ���¶�� ������ ���� ���¶�� �״�� ���� �̺�Ʈ���� ���
	if (m_fDoorTurn <= m_fDoorSpinValue)
	{
		m_isDoorAction = false;
		m_fDoorSpinValue = 0.f;

		BEGININSTANCE;
		pGameInstance->Play_Sound(TEXT("SanctumDoorEnd.wav"), 1.f);
		ENDINSTANCE;
	}
}

CSanctum_Door* CSanctum_Door::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSanctum_Door* pInstance = New CSanctum_Door(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CSanctum_Door");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSanctum_Door::Clone(void* pArg)
{
	CSanctum_Door* pInstance = New CSanctum_Door(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CSanctum_Door");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSanctum_Door::Free()
{
	__super::Free();
}

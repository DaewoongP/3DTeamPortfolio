#include "..\Public\Tool.h"

CTool::CTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CItem(pDevice, pContext)
{
}

CTool::CTool(const CTool& rhs)
	: CItem(rhs)
{
}

HRESULT CTool::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	// ������ ����
	m_ItemCreateDesc.iCost = 100;											// ����
	m_ItemCreateDesc.wstrKoreanName = TEXT("�������� ����");					// �ѱ۸�
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Potions/UI_T_WoundCleaning.png"); // UI���

	return S_OK;
}

HRESULT CTool::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CTool::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CTool::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CTool::Use(_float3 vPlayPos)
{
}

void CTool::Free(void)
{
	__super::Free();
}

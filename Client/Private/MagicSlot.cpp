#include "..\Public\MagicSlot.h"

CMagicSlot::CMagicSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CMagicSlot::CMagicSlot(const CMagicSlot& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMagicSlot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMagicSlot::Initialize(void* pArg)
{
	return S_OK;
}

void CMagicSlot::Tick(_float fTimeDelta)
{
}

void CMagicSlot::Late_Tick(_float fTimeDelta)
{
}

HRESULT CMagicSlot::Add_Magic(_uint iSlotNum, SPELL eMagicTag)
{
	if (4 == m_MagicSlots.size())
		return E_FAIL;

	// ������ ������ ���� �������ְ�
	// �װ� �����ϴ� Ŭ���������� �ʿ�
	// �Ŵ��� Ŭ������ �����ص� ����������
	// ������Ʈ���·� �����Ͽ� �ٸ� AI���� ������ ���� �� �ְԲ� ó��.
	// ���� ���⼭ ������Ʈ�� ����ִٰ� �������ִ¹������ ó��



	return S_OK;
}

HRESULT CMagicSlot::Add_Components()
{
	CMagic::MAGICDESC magicInitDesc;
	magicInitDesc.eBuffType = CMagic::BUFF_SHILED;
	magicInitDesc.eMagicGroup = CMagic::MG_ESSENTIAL;
	magicInitDesc.eMagicType = CMagic::MT_ALL;
	magicInitDesc.eMagicTag = PROTEGO;
	magicInitDesc.fCoolTime = 1.f;
	magicInitDesc.fDamage = 0.f;
	magicInitDesc.fCastDistance = 1000;
	magicInitDesc.fBallDistance = 30;
	magicInitDesc.fLifeTime = 5.f;
	
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Magic"),
		TEXT("Com_Magic_Protego"), reinterpret_cast<CComponent**>(&m_Magics[magicInitDesc.eMagicTag]), &magicInitDesc)))
	{
		MSG_BOX("Failed CMagicSlot Add_Component : (Com_Magic_Protego)");
		return E_FAIL;
	}

	return S_OK;
}

CMagicSlot* CMagicSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMagicSlot* pInstance = New CMagicSlot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMagicSlot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMagicSlot::Clone(void* pArg)
{
	CMagicSlot* pInstance = New CMagicSlot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMagic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMagicSlot::Free()
{
	__super::Free();
}

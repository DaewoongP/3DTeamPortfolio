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

HRESULT CMagicSlot::Create_Magic(CMagic::MAGICDESC _MagicDesc, std::function<void()> func)
{
	if (4 == m_MagicSlots.size())
		return E_FAIL;

	// 마법은 어차피 전부 정해져있고
	// 그걸 관리하는 클래스정도만 필요
	// 매니저 클래스로 제작해도 괜찮겠지만
	// 컴포넌트형태로 제작하여 다른 AI에도 가져다 붙일 수 있게끔 처리.
	// 전부 여기서 컴포넌트로 들고있다가 스왑해주는방식으로 처리



	return S_OK;
}

HRESULT CMagicSlot::Add_Components()
{
	return E_NOTIMPL;
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

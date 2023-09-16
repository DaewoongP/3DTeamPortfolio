#include "Protego.h"
#include "GameInstance.h"
#include "Protego_Effect.h"
CProtego::CProtego(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

CProtego::CProtego(const CProtego& rhs)
	: CMagicBall(rhs)
{
}

HRESULT CProtego::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CProtego::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX("Failed Player Initialize");

		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		MSG_BOX("Failed Player Add_Components");

		return E_FAIL;
	}
	return S_OK;
}

void CProtego::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CProtego::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CProtego::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionEnter(CollisionEventDesc);
	cout << "Player Enter" << endl;
}

void CProtego::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionStay(CollisionEventDesc);
	cout << "stay" << endl;
}

void CProtego::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionExit(CollisionEventDesc);
	cout << "Exit" << endl;
}

HRESULT CProtego::Add_Components()
{
	try
	{
		if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Protego_Effect")
			, TEXT("Com_Protego_Effect"), reinterpret_cast<CComponent**>(&m_pProtegoEffect), &m_MagicBallDesc)))
			throw "Com_Protego_Effect";
	}
	catch (const _char* pErrorMessage)
	{
		string strErrorMessage = "Failed to Cloned in CProtego : ";
		strErrorMessage += pErrorMessage;
		MSG_BOX(strErrorMessage.data());
	}


	return S_OK;
}

HRESULT CProtego::Add_Effect()
{
	return S_OK;
}

CProtego* CProtego::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CProtego* pInstance = new CProtego(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CProtego");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CProtego::Clone(void* pArg)
{
	CProtego* pInstance = new CProtego(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CProtego");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CProtego::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pProtegoEffect);
	}
}

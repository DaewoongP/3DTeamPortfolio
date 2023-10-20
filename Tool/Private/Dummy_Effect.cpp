#include "Dummy_Effect.h"
#include "DummyTrail.h"
#include "DummyParticle.h"
#include "DummyMeshEffect.h"
#include "DummyFlipBook.h"
#include "GameInstance.h"
#include "Modules.h"
#include <effects.h>

CDummy_Effect::CDummy_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CDummy_Effect::CDummy_Effect(const CDummy_Effect& rhs)
	: CGameObject(rhs)
{
}

HRESULT CDummy_Effect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	BEGININSTANCE;

	if (nullptr == pGameInstance->Find_Prototype(0, TEXT("Prototype_GameObject_MeshEffect_Outer_Ball")))
	{
		if (FAILED(pGameInstance->Add_Prototype(0, TEXT("Prototype_GameObject_MeshEffect_Outer_Ball")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Projectile_White/Outer_Ball.ME"), 0))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(0, TEXT("Prototype_GameObject_MeshEffect_Inner_Ball")))
	{
		if (FAILED(pGameInstance->Add_Prototype(0, TEXT("Prototype_GameObject_MeshEffect_Inner_Ball")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Projectile_White/Inner_Ball.ME"), 0))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CDummy_Effect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_Position(_float3(0.f, 0.f, 0.f));

	//PT_NOTIFY ptNotify;
	//ptNotify.isPlayOnce = true;
	//ptNotify.StartCondition = [&](const PARTICLE_IT& iter)->_bool {
	//	return false == iter->isAlive;
	//};
	//ptNotify.Event = [&]() {
	//	cout << "데스이벤트 발생" << endl;
	//};
	//m_pParticleSystem->Add_Notify(ptNotify);

	return S_OK;
}

void CDummy_Effect::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	BEGININSTANCE;

	if (pGameInstance->Get_DIKeyState(DIK_UPARROW, CInput_Device::KEY_DOWN))
	{
		m_pMeshEffect_Outer_Ball->Play(m_pTransform->Get_Position());
		m_pMeshEffect_Inner_Ball->Play(m_pTransform->Get_Position());
	}

	ENDINSTANCE;
}

void CDummy_Effect::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CDummy_Effect::Tick_Imgui(_float fTimeDelta)
{
	//m_pMeshEffect_Outer_Ball->Tick_Imgui(fTimeDelta);
	//m_pMeshEffect_Inner_Ball->Tick_Imgui(fTimeDelta);
}

HRESULT CDummy_Effect::Add_Components()
{
	try
	{
		/* Com_Trail */
		/*if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_GameObject_TestTrail"),
			TEXT("Com_Trail"), reinterpret_cast<CComponent**>(&m_pTrail))))
			throw "Com_Trail";*/

		if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_GameObject_MeshEffect_Outer_Ball"),
			TEXT("Com_test1"), reinterpret_cast<CComponent**>(&m_pMeshEffect_Outer_Ball))))
			throw "Com_test1";

		if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_GameObject_MeshEffect_Inner_Ball"),
			TEXT("Com_test2"), reinterpret_cast<CComponent**>(&m_pMeshEffect_Inner_Ball))))
			throw "Com_test2";

	}
	catch (const _char* pErrorMessage)
	{
		string strErrorMessage = "Failed to Add Component in CDummy_Effect : ";
		strErrorMessage += pErrorMessage;
		MSG_BOX(strErrorMessage.data());
	}

	return S_OK;
}

CDummy_Effect* CDummy_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDummy_Effect* pInstance = New CDummy_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CDummy_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDummy_Effect::Clone(void* pArg)
{
	CDummy_Effect* pInstance = New CDummy_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CDummy_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDummy_Effect::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pMeshEffect_Inner_Ball);
		Safe_Release(m_pMeshEffect_Outer_Ball);
	}
}

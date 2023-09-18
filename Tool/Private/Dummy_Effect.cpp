#include "Dummy_Effect.h"
#include "DummyTrail.h"
#include "DummyParticle.h"
#include "DummyMeshEffect.h"
#include "DummyFlipBook.h"
#include "GameInstance.h"
#include "Modules.h"
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

	if (nullptr == pGameInstance->Find_Prototype(0, TEXT("Prototype_GameObject_TestTrail")))
	{
		pGameInstance->Add_Prototype(0, TEXT("Prototype_GameObject_TestTrail")
			, CDummyTrail::Create(m_pDevice, m_pContext, TEXT("")));
	}

	if (nullptr == pGameInstance->Find_Prototype(0, TEXT("Prototype_GameObject_TestParticle")))
	{
		pGameInstance->Add_Prototype(0, TEXT("Prototype_GameObject_TestParticle")
			, CDummyParticle::Create(m_pDevice, m_pContext, TEXT("")));
	}

	if (nullptr == pGameInstance->Find_Prototype(0, TEXT("Prototype_GameObject_TestMeshEffect")))
	{
		pGameInstance->Add_Prototype(0, TEXT("Prototype_GameObject_TestMeshEffect")
			, CDummyMeshEffect::Create(m_pDevice, m_pContext, TEXT("")));
	}

	if (nullptr == pGameInstance->Find_Prototype(0, TEXT("Prototype_GameObject_TestTexture_Flipbook")))
	{
		/* Prototype_Component_Texture_Ground */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_GameObject_TestTexture_Flipbook"),
			CDummyFlipBook::Create(m_pDevice, m_pContext, LEVEL_TOOL, nullptr))))
			return E_FAIL;
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
	
	m_pTrailTransform = m_pTrail->Get_Transform();
	m_pParticleSystemTransform = m_pParticleSystem->Get_Transform();
	m_pMeshEffectTransform = m_pMeshEffect->Get_Transform();
	Safe_AddRef(m_pTrailTransform);
	Safe_AddRef(m_pParticleSystemTransform);
	Safe_AddRef(m_pMeshEffectTransform);
	return S_OK;
}

void CDummy_Effect::Tick(_float fTimeDelta)
{
	m_pTrail->Tick(fTimeDelta);
	m_pParticleSystem->Tick(fTimeDelta);
	m_pMeshEffect->Tick(fTimeDelta);
	m_pTextureFlipbook->Tick(fTimeDelta);
	BEGININSTANCE;

	if (pGameInstance->Get_DIKeyState(DIK_UPARROW))
	{
		m_pTransform->Go_Straight(fTimeDelta);
	}
	else if (pGameInstance->Get_DIKeyState(DIK_DOWNARROW))
	{
		m_pTransform->Go_Backward(fTimeDelta);
	}
	else if (pGameInstance->Get_DIKeyState(DIK_LEFTARROW))
	{
		m_pTransform->Go_Left(fTimeDelta);
	}
	else if (pGameInstance->Get_DIKeyState(DIK_RIGHTARROW))
	{
		m_pTransform->Go_Right(fTimeDelta);
	}
	else if (pGameInstance->Get_DIKeyState(DIK_SPACE))
	{
		m_pParticleSystem->Play();
	}


	SHAPE_MODULE& ShapeModule = m_pParticleSystem->Get_ShapeModuleRef();
	float fLength = _float3(m_vPrevPos - m_pTransform->Get_Position()).Length();
	if(fLength >= 0.001f)
		ShapeModule.ShapeMatrix.MatrixLookAt(m_pTransform->Get_Position(), m_vPrevPos, _float3(0.f, 1.f, 0.f));
	m_vPrevPos = m_pTransform->Get_Position();
	ENDINSTANCE;
}

void CDummy_Effect::Late_Tick(_float fTimeDelta)
{
	//m_pTrail->Late_Tick(fTimeDelta);
	m_pParticleSystem->Late_Tick(fTimeDelta);
	//m_pMeshEffect->Late_Tick(fTimeDelta);
	m_pTextureFlipbook->Late_Tick(fTimeDelta);
	m_vPrevPos = m_pTransform->Get_Position();
}

void CDummy_Effect::Tick_Imgui(_float fTimeDelta)
{
	m_pParticleSystem->Tick_Imgui(fTimeDelta);
}

HRESULT CDummy_Effect::Add_Components()
{
	try
	{
		/* Com_Trail */
		if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_GameObject_TestTrail"),
			TEXT("Com_Trail"), reinterpret_cast<CComponent**>(&m_pTrail))))
			throw "Com_Trail";

		/* Com_Pariticle */
		if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_GameObject_TestParticle"),
			TEXT("Com_Pariticle"), reinterpret_cast<CComponent**>(&m_pParticleSystem))))
			throw "Com_Pariticle";

		/* Com_Pariticle */
		if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_GameObject_TestMeshEffect"),
			TEXT("Com_MeshEffect"), reinterpret_cast<CComponent**>(&m_pMeshEffect))))
			throw "Com_MeshEffect";

		/* Com_Texture_Flipbook */
		if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_GameObject_TestTexture_Flipbook"),
			TEXT("Com_Flipbook"), reinterpret_cast<CComponent**>(&m_pTextureFlipbook))))
			throw "Com_Flipbook";
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
		Safe_Release(m_pTrail);
		Safe_Release(m_pParticleSystem);
		Safe_Release(m_pMeshEffect);
		Safe_Release(m_pTextureFlipbook);

		Safe_Release(m_pTrailTransform);
		Safe_Release(m_pParticleSystemTransform);
		Safe_Release(m_pMeshEffectTransform);
	}
}

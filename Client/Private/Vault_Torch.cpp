#include "..\Public\Vault_Torch.h"
#include "Light.h"
#include "GameInstance.h"
#include "MagicBall.h"

CVault_Torch::CVault_Torch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapObject(pDevice, pContext)
{
}

CVault_Torch::CVault_Torch(const CVault_Torch& rhs)
	: CMapObject(rhs)
{
}

void CVault_Torch::Switch_OnOff(_bool isValue)
{
	m_isSwitch = isValue;
	BEGININSTANCE;
	if (m_isSwitch)
	{
		m_pEffect->Play(m_pTransform->Get_Position() + _float3(0, 1.2f, 0));
		if (m_pLight == nullptr)
		{
			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);
			CLight::LIGHTDESC LightDesc;
			LightDesc.eType = CLight::TYPE_POINT;
			LightDesc.fRange = 10.f;
			LightDesc.vDiffuse = _float4(1.f, 1.f, 0.f, 1.f);
			LightDesc.vAmbient = LightDesc.vDiffuse;
			LightDesc.vSpecular = LightDesc.vDiffuse;
			LightDesc.vPos = m_pTransform->Get_Position().TransCoord();
			pGameInstance->Add_Light(LightDesc, &m_pLight);
			Safe_Release(pGameInstance);
		}
	}
	else 
	{
		m_pEffect->Stop();
		pGameInstance->Add_InstanceLight(m_pTransform->Get_Position(), 10.f, 1.f, _float4(1,1,1,1));
		if (m_pLight != nullptr)
		{
			pGameInstance->Return_Light(m_pLight);
			m_pLight = nullptr;
		}
	}
	ENDINSTANCE;
}

HRESULT CVault_Torch::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVault_Torch::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("CVault_Torch Argument is NULL");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(CComposite::Add_Component(LEVEL_VAULT, TEXT("Prototype_GameObject_Vault_Torch_P"),
		TEXT("Com_Torch"), reinterpret_cast<CComponent**>(&m_pEffect))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Vault_Torch_P)");
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CVault_Torch::Initialize_Level(_uint iCurrentLevelIndex)
{
	/* Com_Model */
	if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, m_ObjectDesc.wszTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CMapObject Add_Component : (Com_Model)");
		__debugbreak();
		return E_FAIL;
	}

	// ≈‰ƒ° ¿Œµ¶Ω∫ √ﬂ√‚
	wstring wsTorch(m_pTag);
	size_t findIndex = wsTorch.find(TEXT("Torch_")) + 6;

	wstring wsModelIndex = wsTorch.substr(findIndex);
	m_iTorchIndex = stoi(wsModelIndex);

	// æ»√∂πŒ Torch
	if (24 <= m_iTorchIndex)
	{
		CRigidBody::RIGIDBODYDESC RigidBodyDesc;
		RigidBodyDesc.isStatic = true;
		RigidBodyDesc.isTrigger = false;
		RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
		RigidBodyDesc.vOffsetPosition = _float3(0.f, 0.0f, 0.f);
		RigidBodyDesc.fStaticFriction = 0.f;
		RigidBodyDesc.fDynamicFriction = 0.f;
		RigidBodyDesc.fRestitution = 0.f;
		PxSphereGeometry SphereGeometry = PxSphereGeometry(2.f);
		RigidBodyDesc.pGeometry = &SphereGeometry;
		RigidBodyDesc.eConstraintFlag = CRigidBody::AllRot;
		RigidBodyDesc.vDebugColor = _float4(1.f, 0.f, 0.f, 1.f);
		RigidBodyDesc.isGravity = false;
		RigidBodyDesc.pOwnerObject = this;
		RigidBodyDesc.eThisCollsion = COL_ENEMY;
		RigidBodyDesc.eCollisionFlag = COL_MAGIC;
		strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Torch");

		/* Com_RigidBody */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
			TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
		{
			MSG_BOX("Failed CMagicBall Add_Component : (Com_RigidBody)");
			__debugbreak();
			return E_FAIL;
		}

		m_pEffect->Stop();
		m_pEffect->Get_MainModuleRef().fSimulationSpeed = Random_Generator(0.5f, 1.0f);
		Switch_OnOff(true);
	}
	
	// Ω…¡§»Ø Torch
	else
	{
		m_pEffect->Stop();
	}

	return S_OK;
}

void CVault_Torch::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Torch_On_By_Trigger(fTimeDelta);
}

void CVault_Torch::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);	
#ifdef _DEBUG
		if(nullptr != m_pRigidBody)
			m_pRenderer->Add_DebugGroup(m_pRigidBody);
#endif // _DEBUG
	}
}

void CVault_Torch::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	wstring wstrObjectTag = CollisionEventDesc.pOtherObjectTag;
	/* Collision Magic */
	if (wstring::npos != wstrObjectTag.find(TEXT("MagicBall")))
	{
		CMagicBall::COLLSIONREQUESTDESC* pCollisionMagicBallDesc = static_cast<CMagicBall::COLLSIONREQUESTDESC*>(CollisionEventDesc.pArg);
		BUFF_TYPE eBuff = pCollisionMagicBallDesc->eBuffType;
		if (eBuff == BUFF_CONFRINGO)
		{
			Switch_OnOff(true);
		}
	}
}

void CVault_Torch::Torch_On_By_Trigger(_float fTimeDelta)
{
	if (false == m_isTorchOn)
		return;

	m_fLightTime += fTimeDelta;

	if (false == m_isEffectOn)
	{
		m_pEffect->Play(m_pTransform->Get_Position() + _float3(0, 1.2f, 0));
		m_isEffectOn = true;
	}	

	if (nullptr == m_pLightbyTrigger && 0.2f <= m_fLightTime)
	{
		m_pEffect->Get_MainModuleRef().fSimulationSpeed = Random_Generator(0.5f, 1.0f);

		BEGININSTANCE;

		CLight::LIGHTDESC LightDesc;
		LightDesc.eType = CLight::TYPE_POINT;
		LightDesc.fRange = 10.f;
		LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
		LightDesc.vAmbient = LightDesc.vDiffuse;
		LightDesc.vSpecular = LightDesc.vDiffuse;
		LightDesc.vPos = m_pTransform->Get_Position().TransCoord();
		pGameInstance->Add_Light(LightDesc, &m_pLightbyTrigger);
		
		ENDINSTANCE;
	}
}

CVault_Torch* CVault_Torch::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVault_Torch* pInstance = New CVault_Torch(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CVault_Torch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CVault_Torch::Clone(void* pArg)
{
	CVault_Torch* pInstance = New CVault_Torch(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CVault_Torch");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVault_Torch::Free()
{
	__super::Free();
	if (m_isCloned)
	{
		Safe_Release(m_pRigidBody);
		Safe_Release(m_pEffect);
		Safe_Release(m_pLight);
	}

	Safe_Release(m_pLightbyTrigger);
}

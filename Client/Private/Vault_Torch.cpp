#include "..\Public\Vault_Torch.h"
#include "GameInstance.h"

CVault_Torch::CVault_Torch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapObject(pDevice, pContext)
{
}

CVault_Torch::CVault_Torch(const CVault_Torch& rhs)
	: CMapObject(rhs)
{
}

HRESULT CVault_Torch::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	BEGININSTANCE;

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_VAULT, TEXT("Prototype_GameObject_Vault_Torch_P")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_VAULT, TEXT("Prototype_GameObject_Vault_Torch_P")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Vault_Torch"), LEVEL_VAULT))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	ENDINSTANCE;

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

	m_pEffect->Play(m_pTransform->Get_Position() + _float3(0,1.2f,0));
	m_pEffect->Get_MainModuleRef().fSimulationSpeed = Random_Generator(0.5f, 1.0f);

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

	// 토치 인덱스 추출
	wstring wsTorch(m_pTag);
	size_t findIndex = wsTorch.find(TEXT("Torch_")) + 6;

	wstring wsModelIndex = wsTorch.substr(findIndex);
	m_iTorchIndex = stoi(wsModelIndex);

	// 자체 발광 추가
	CLight::LIGHTDESC		LightDescHork;
	ZeroMemory(&LightDescHork, sizeof LightDescHork);

	LightDescHork.eType = CLight::TYPE_POINT;
	LightDescHork.vPos = m_pTransform->Get_Position().TransCoord();
	LightDescHork.fRange = 10.f;

	LightDescHork.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDescHork.vAmbient = WHITEDEFAULT;
	LightDescHork.vSpecular = LightDescHork.vDiffuse;

	BEGININSTANCE;
	pGameInstance->Add_Light(LightDescHork, nullptr);
	ENDINSTANCE;

	return S_OK;
}

void CVault_Torch::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	BEGININSTANCE;

	if (pGameInstance->Get_DIKeyState(DIK_Y, CInput_Device::KEY_DOWN))
	{

	}

	ENDINSTANCE;
}

void CVault_Torch::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);		
	}
}

void CVault_Torch::Torch_Bright()
{
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

	Safe_Release(m_pEffect);
}

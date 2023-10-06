#include "..\Public\Gatherer.h"
#include "GameInstance.h"

#include "Player.h"

CGatherer::CGatherer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CGatherer::CGatherer(const CGatherer& rhs)
	: CGameObject(rhs)
{
}

HRESULT CGatherer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGatherer::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("CGatherer Argument is NULL");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_ObjectDesc = *reinterpret_cast<MAPOBJECTDESC*>(pArg);
	m_pTransform->Set_WorldMatrix(m_ObjectDesc.WorldMatrix);

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGatherer::Initialize_Level(_uint iCurrentLevelIndex)
{
	/* Com_Model */
	if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, m_ObjectDesc.wszTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CGatherer Add_Component : (Com_Model)");
		__debugbreak();
		return E_FAIL;
	}

	// 채집물 종류 파악
	wstring wsAshwinderEggs(TEXT("Anim_AshwinderEggs"));
	wstring wsHorklump(TEXT("Anim_Horklump"));
	wstring wsLeapingToadStools(TEXT("Anim_LeapingToadStools"));
	wstring wsLeech(TEXT("Anim_Leech"));

	if (0 == lstrcmp(m_ObjectDesc.wszTag, wsAshwinderEggs.c_str()))
	{
		m_GatheringType = CGatherer::ASHWINDEREGG;
	}
	else if (0 == lstrcmp(m_ObjectDesc.wszTag, wsHorklump.c_str()))
	{
		m_GatheringType = CGatherer::HORKLUMP;
	}
	else if (0 == lstrcmp(m_ObjectDesc.wszTag, wsLeapingToadStools.c_str()))
	{
		m_GatheringType = CGatherer::LEAPINGTOADSTOOLS;
	}
	else if (0 == lstrcmp(m_ObjectDesc.wszTag, wsLeech.c_str()))
	{
		m_GatheringType = CGatherer::LEECH;
	}

	// 채집물 애니메이션이 공통적으로 0번이 뽑히는 모션임
	// 1번이 IDLE이라고 생각할 수 있음
	m_pModel->Set_CurrentAnimIndex(1);

	// 플레이어 찾기
	BEGININSTANCE;
	m_pPlayer = static_cast<CPlayer*>(pGameInstance->Find_Component_In_Layer(iCurrentLevelIndex, TEXT("Layer_Player"), TEXT("GameObject_Player")));
	ENDINSTANCE;

	return S_OK;
}

void CGatherer::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	// 플레이어와 거리 비교
	_float3 vPlayerPos = m_pPlayer->Get_PlayerPos();
	_float3 vChestPos = m_pTransform->Get_Position();

	m_fDist_From_Player = sqrtf((vPlayerPos.x - vChestPos.x) * (vPlayerPos.x - vChestPos.x) +
		(vPlayerPos.y - vChestPos.y) * (vPlayerPos.y - vChestPos.y) +
		(vPlayerPos.z - vChestPos.z) * (vPlayerPos.z - vChestPos.z));

	// 일정 거리안으로 들어왔을 때
	if (2.f >= m_fDist_From_Player && nullptr != m_pModel)
	{
		// 여기서 버튼 UI가 나타나면 될듯

		BEGININSTANCE;  // 버튼을 누르면 동작
		if (pGameInstance->Get_DIKeyState(DIK_E, CInput_Device::KEY_DOWN))
		{
			// 채집당하는 애니메이션으로 변경
			m_pModel->Set_CurrentAnimIndex(0);

			// 여기서 인벤토리 처리해주면 될듯
		}

		ENDINSTANCE;
	}

	if (nullptr != m_pModel)
		m_pModel->Play_Animation(fTimeDelta, CModel::UPPERBODY, m_pTransform);
}

void CGatherer::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	BEGININSTANCE;

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);
	}

	ENDINSTANCE;
}

HRESULT CGatherer::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint iMeshCount = 0; iMeshCount < iNumMeshes; ++iMeshCount)
	{
		m_pModel->Bind_BoneMatrices(m_pShader, "g_BoneMatrices", iMeshCount);

		m_pModel->Bind_Material(m_pShader, "g_DiffuseTexture", iMeshCount, DIFFUSE);
		m_pModel->Bind_Material(m_pShader, "g_NormalTexture", iMeshCount, NORMALS);

		m_pShader->Begin("AnimMesh");

		if (FAILED(m_pModel->Render(iMeshCount)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CGatherer::Render_Depth()
{
	if (FAILED(SetUp_ShadowShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint iMeshCount = 0; iMeshCount < iNumMeshes; ++iMeshCount)
	{
		m_pModel->Bind_BoneMatrices(m_pShadowShader, "g_BoneMatrices", iMeshCount);

		if (FAILED(m_pShadowShader->Begin("Shadow")))
			return E_FAIL;

		if (FAILED(m_pModel->Render(iMeshCount)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CGatherer::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CGatherer Add_Component : (Com_Renderer)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CGatherer Add_Component : (Com_Shader)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_ShadowShader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ShadowAnimMesh"),
		TEXT("Com_ShadowShader"), reinterpret_cast<CComponent**>(&m_pShadowShader))))
	{
		MSG_BOX("Failed CGatherer Add_Component : (Com_ShadowShader)");
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGatherer::SetUp_ShaderResources()
{
	BEGININSTANCE;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	ENDINSTANCE;

	return S_OK;
}

HRESULT CGatherer::SetUp_ShadowShaderResources()
{
	BEGININSTANCE;

	if (FAILED(m_pShadowShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShadowShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_LightTransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShadowShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_LightTransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShadowShader->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	ENDINSTANCE;

	return S_OK;
}

void CGatherer::Check_MinMaxPoint(_float3 vPoint)
{
	if (m_vMinPoint.x > vPoint.x)
		m_vMinPoint.x = vPoint.x;
	if (m_vMinPoint.y > vPoint.y)
		m_vMinPoint.y = vPoint.y;
	if (m_vMinPoint.z > vPoint.z)
		m_vMinPoint.z = vPoint.z;

	if (m_vMaxPoint.x < vPoint.x)
		m_vMaxPoint.x = vPoint.x;
	if (m_vMaxPoint.y < vPoint.y)
		m_vMaxPoint.y = vPoint.y;
	if (m_vMaxPoint.z < vPoint.z)
		m_vMaxPoint.z = vPoint.z;
}

CGatherer* CGatherer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGatherer* pInstance = New CGatherer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CGatherer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGatherer::Clone(void* pArg)
{
	CGatherer* pInstance = New CGatherer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CGameObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGatherer::Free()
{
	__super::Free();

	Safe_Release(m_pShadowShader);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pRenderer);
}

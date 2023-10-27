#include "..\Public\Balloon.h"
#include "FlyGameManager.h"
#include "GameInstance.h"
#include "Racer.h"
#include "Balloon_Timer.h"

CBalloon::CBalloon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CBalloon::CBalloon(const CBalloon& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBalloon::Initialize(void* pArg)
{
	NULL_CHECK_RETURN(pArg, E_FAIL);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));

	BALLOONINITDESC* pInitDesc = static_cast<BALLOONINITDESC*>(pArg);

	m_iScore = pInitDesc->iScore;
	m_pTransform->Set_WorldMatrix(XMMatrixTranslation(pInitDesc->vPosition.x, pInitDesc->vPosition.y, pInitDesc->vPosition.z));
	m_pTransform->Set_Scale(pInitDesc->vScale);
	m_isColliderOn = true;
	m_isDead = true;

	// balloon timer 생성
	// 트랜스폼 필요해요 ㅠ
	FAILED_CHECK_RETURN(Make_Timer(), E_FAIL);

	return S_OK;
}

void CBalloon::Tick(_float fTimeDelta)
{
	m_pTransform->Turn(_float3(0.f, 1.f, 0.f), fTimeDelta);

	if (true == m_isColliderOn &&
		true == m_isDead)
	{
		m_isColliderOn = false;
		m_pRigidBody->Disable_Collision("Body");
	}

	if (true == m_pTimer->Is_Finished())
	{
		m_isDead = true;
		m_pRigidBody->Disable_Collision("Body");
		return;
	}

	__super::Tick(fTimeDelta);
}

void CBalloon::Late_Tick(_float fTimeDelta)
{
	if (true == m_isDead)
		return;

	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
#ifdef _DEBUG
		m_pRenderer->Add_DebugGroup(m_pRigidBody);
#endif // _DEBUG
	}
}

HRESULT CBalloon::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint iMeshCount = 0; iMeshCount < iNumMeshes; ++iMeshCount)
	{
		m_pModelCom->Bind_Material(m_pShader, "g_DiffuseTexture", iMeshCount, DIFFUSE);
		m_pModelCom->Bind_Material(m_pShader, "g_NormalTexture", iMeshCount, NORMALS);

		m_pShader->Begin("Mesh_Emissive");

		if (FAILED(m_pModelCom->Render(iMeshCount)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBalloon::Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
{
	if (FAILED(SetUp_ShadowShaderResources(LightViewMatrix, LightProjMatrix)))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint iMeshCount = 0; iMeshCount < iNumMeshes; ++iMeshCount)
	{
		if (FAILED(m_pShadowShader->Begin("Shadow")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(iMeshCount)))
			return E_FAIL;
	}

	return S_OK;
}

void CBalloon::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	CFlyGameManager* pMgr = static_cast<CFlyGameManager*>(m_pOwner);
	if (pMgr == nullptr)
		return;

	CRacer::COLLSIONREQUESTDESC* racerInfo = static_cast<CRacer::COLLSIONREQUESTDESC*>(CollisionEventDesc.pArg);
	if (racerInfo == nullptr)
		return;

	pMgr->Add_Score(racerInfo->iRacerNumber,m_iScore);
	pMgr->ReplaceBallon();
	pMgr->Racer_AddForce(racerInfo->iRacerNumber,m_eBallonActionType, m_fForce);

	BEGININSTANCE;
	pGameInstance->Play_Particle(TEXT("Particle_Balloon_Spread"),m_pTransform->Get_Position());
	pGameInstance->Play_Particle(TEXT("Particle_Balloon_Line"),m_pTransform->Get_Position());
	ENDINSTANCE;

	m_isDead = true;
	__super::OnCollisionEnter(CollisionEventDesc);
}

HRESULT CBalloon::Add_Components()
{
	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CMapObject Add_Component : (Com_Shader)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_ShadowShader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ShadowMesh"),
		TEXT("Com_ShadowShader"), reinterpret_cast<CComponent**>(&m_pShadowShader))))
	{
		MSG_BOX("Failed CMapObject Add_Component : (Com_ShadowShader)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("CLoadBalloon Failed Clone Component : Com_Renderer");
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBalloon::SetUp_ShaderResources()
{
	BEGININSTANCE;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	ENDINSTANCE;

	return S_OK;
}

HRESULT CBalloon::SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
{
	BEGININSTANCE;

	if (FAILED(m_pShadowShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShadowShader->Bind_Matrix("g_ViewMatrix", &LightViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShadowShader->Bind_Matrix("g_ProjMatrix", &LightProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShadowShader->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	ENDINSTANCE;

	return S_OK;
}

HRESULT CBalloon::Make_Timer()
{
	CBalloon_Timer::BALLOONTIMERDESC BalloonTimerDesc;
	BalloonTimerDesc.fTime = 10.f * (rand() % 10);
	BalloonTimerDesc.vScale = _float2(10.f, 10.f);
	BalloonTimerDesc.vPosition = m_pTransform->Get_Position();

	/* Com_Timer */
	if (FAILED(CComposite::Add_Component(LEVEL_SKY, TEXT("Prototype_GameObject_Balloon_Timer"),
		TEXT("Com_Timer"), reinterpret_cast<CComponent**>(&m_pTimer), &BalloonTimerDesc)))
	{
		MSG_BOX("CBalloon Failed Clone Component : Com_Timer");
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

void CBalloon::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShader);
	Safe_Release(m_pShadowShader);
	Safe_Release(m_pRigidBody);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pTimer);
}
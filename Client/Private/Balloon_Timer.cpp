#include "..\Public\Balloon_Timer.h"
#include "GameInstance.h"
#include "Layer.h"

CBalloon_Timer::CBalloon_Timer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CBalloon_Timer::CBalloon_Timer(const CBalloon_Timer& rhs)
	: CGameObject(rhs)
{
}

void CBalloon_Timer::Reset(_float3 vPosition, _float fFinishTime)
{
	m_fTime = fFinishTime;
	m_vInitPosition = vPosition;
	m_fTimeAcc = 0.f;
}

HRESULT CBalloon_Timer::Initialize(void* pArg)
{
	NULL_CHECK_RETURN(pArg, E_FAIL);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	BALLOONTIMERDESC TimerDesc = *static_cast<BALLOONTIMERDESC*>(pArg);

	m_pTransform->Set_Scale(_float3(TimerDesc.vScale.x, TimerDesc.vScale.y, 0.f));
	m_vInitPosition = TimerDesc.vPosition;
	m_fTime = TimerDesc.fTime;

	return S_OK;
}

void CBalloon_Timer::Late_Tick(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	m_pTransform->Set_Position(XMVectorLerp(m_vInitPosition, pGameInstance->Get_CamPosition()->xyz(), 0.35f));
	Safe_Release(pGameInstance);

	if (m_fTimeAcc < m_fTime - fTimeDelta) // 1frame
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CBalloon_Timer::Render()
{
	FAILED_CHECK(SetUp_ShaderResources());

	FAILED_CHECK(m_pShader->Begin("Point_Timer"));

	FAILED_CHECK(m_pBuffer->Render());

	return S_OK;
}

HRESULT CBalloon_Timer::Add_Components()
{
	/* For.Com_Renderer */
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer)));
	
	/* For.Com_Shader */
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader)));

	_uint iNum = 1;
	/* For.Com_Buffer */
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Instance"),
		TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pBuffer), &iNum));

	return S_OK;
}

HRESULT CBalloon_Timer::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr()));
	FAILED_CHECK(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ)));

	FAILED_CHECK(m_pShader->Bind_RawValue("g_vCamPosition", pGameInstance->Get_CamPosition(), sizeof(_float4)));
	_float fRatio = m_fTimeAcc / m_fTime;
	FAILED_CHECK(m_pShader->Bind_RawValue("g_fTimeRatio", &fRatio, sizeof(_float)));

	Safe_Release(pGameInstance);

	return S_OK;
}

CBalloon_Timer* CBalloon_Timer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBalloon_Timer* pInstance = New CBalloon_Timer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBalloon_Timer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBalloon_Timer::Clone(void* pArg)
{
	CBalloon_Timer* pInstance = New CBalloon_Timer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBalloon_Timer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBalloon_Timer::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pBuffer);
	Safe_Release(m_pRenderer);
}
#include "..\Public\DummyParticle.h"
#include "GameInstance.h"
DummyParticle::DummyParticle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

DummyParticle::DummyParticle(const DummyParticle& _rhs)
	: CGameObject(_rhs)
	, m_pParticle(_rhs.m_pParticle)
{
}

HRESULT DummyParticle::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	// ���̶� ���⿡�� �س���
	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT DummyParticle::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;
	
	//if (FAILED(Add_Components()))
	//	return E_FAIL;

	return S_OK;
}

void DummyParticle::Tick(_float _fTimeDelta)
{
	vector<_float4x4>		ParticleMatrices;
	for (auto& Particle : m_Particles)
	{
		Particle.dAge += _fTimeDelta;
		_float4 vPos;
		_float4 vVelocity;

		if (Particle.dAge > Particle.dLifeTime)
		{
			;
		}
		// ������������ ��ġ���� ������.
		vPos = Particle.WorldMatrix.Translation().TransCoord();

		// ���ӵ��� �̿��� ���� �ӵ��� ����.
		vVelocity = Particle.vVelocity;
		vVelocity += Particle.vAccel * _fTimeDelta;

		// ��ġ�� �ӵ��� ���ؼ� ���� ��ġ�� ����.
		vPos = vPos + vVelocity * _fTimeDelta;

		// SRT ����
		_Matrix ScaleMatrix = _Matrix::MatrixScale(0.1f);
		//_Matrix RotMatrix = _Matrix::MatrixScale(0.1f);
		_Matrix TransMatrix = _Matrix::MatrixScale(0.1f);
		Particle.WorldMatrix = ScaleMatrix * TransMatrix;

		ParticleMatrices.push_back(Particle.WorldMatrix);
	}

	//m_pBuffer->Tick(&ParticleMatrices[0], _fTimeDelta);
}

void DummyParticle::Late_Tick(_float _fTimeDelta)
{
	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT DummyParticle::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShader->Begin(0);

	m_pBuffer->Render();

	return S_OK;
}

HRESULT DummyParticle::Add_Components()
{
	BEGININSTANCE;

	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Fire_Particle")
		, TEXT("Com_Particle"), (CComponent**)&m_pParticle)))
	{
		return E_FAIL;
		ENDINSTANCE;
	}

	ENDINSTANCE;
	return S_OK;
}

HRESULT DummyParticle::SetUp_ShaderResources()
{
	//_float4x4 MyMatrix = m_pTransform->Get_WorldMatrix();
	//FAILED_CHECK_RETURN(m_pShader->Bind_Matrix("g_WorldMatrix", &MyMatrix), E_FAIL);

	//CPipeLine* pPipeLineInstance = CPipeLine::GetInstance();
	//Safe_AddRef(pPipeLineInstance);
	//
	//FAILED_CHECK_RETURN(m_pShader->Bind_Matrix("g_ViewMatrix", pPipeLineInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW)), E_FAIL);
	//FAILED_CHECK_RETURN(m_pShader->Bind_Matrix("g_ProjMatrix", pPipeLineInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ)), E_FAIL);
	//FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("g_vCamPosition", pPipeLineInstance->Get_CamPosition(), sizeof(_float4)), E_FAIL);
	//FAILED_CHECK_RETURN(m_pTexture->Bind_ShaderResources(m_pShader, "g_Texture"), E_FAIL);

	//Safe_Release(pPipeLineInstance);
	return S_OK;
}

DummyParticle* DummyParticle::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	DummyParticle* pInstance = New DummyParticle(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Faild to Created DummyParticle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* DummyParticle::Clone(void* _pArg)
{
	DummyParticle* pInstance = New DummyParticle(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Faild to Created DummyParticle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void DummyParticle::Free(void)
{
	__super::Free();
	Safe_Release(m_pRenderer);
	Safe_Release(m_pParticle);
	Safe_Release(m_pTexture);
	Safe_Release(m_pShader);
	Safe_Release(m_pBuffer);
}
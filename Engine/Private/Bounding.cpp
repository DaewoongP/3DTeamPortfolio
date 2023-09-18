#include "..\Public\Bounding.h"
#include "PipeLine.h"

CBounding::CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CBounding::CBounding(const CBounding& rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pContext(rhs.m_pContext)
	, m_isCloned(true)
#ifdef _DEBUG
	, m_pPrimitiveBatch(rhs.m_pPrimitiveBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
#endif // _DEBUG
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif // _DEBUG
}

HRESULT CBounding::Initialize_Prototype()
{
#ifdef _DEBUG
	m_pPrimitiveBatch = New PrimitiveBatch<DirectX::VertexPositionColor>(m_pContext);
	m_pEffect = New DirectX::BasicEffect(m_pDevice);

	NULL_CHECK_RETURN_MSG(m_pPrimitiveBatch, E_FAIL, TEXT("Failed Create PrimitiveBatch"));
	NULL_CHECK_RETURN_MSG(m_pEffect, E_FAIL, TEXT("Failed Create DirectX::BasicEffect"));

	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
	size_t		iShaderByteCodeLength = { 0 };

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	FAILED_CHECK_RETURN_MSG(m_pDevice->CreateInputLayout(DirectX::VertexPositionColor::InputElements, DirectX::VertexPositionColor::InputElementCount,
		pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout), E_FAIL, TEXT("Failed Create InputLayout"));
#endif // _DEBUG

	return S_OK;
}

#ifdef _DEBUG
HRESULT CBounding::Begin()
{
	if (nullptr == m_pPrimitiveBatch)
		return E_FAIL;

	// �ٿ�� ��ü���� DirectX Bounding ��ü ��ü�� ��ȯ�����ֱ� ������ ���⼭ ó�� �� �ʿ䰡 ����.
	// ������ ���⼭ ó���ϸ� �������Ҷ� ó���߱� ������ ���߿� �浹 ó���Ҷ��� ���� ó���Լ��� ����� �����.
	m_pEffect->SetWorld(XMMatrixIdentity());

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	m_pEffect->SetView(*pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(*pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

	Safe_Release(pPipeLine);
	// ���̴� Apply
	m_pEffect->Apply(m_pContext);
	// InputLayout ����
	m_pContext->IASetInputLayout(m_pInputLayout);
	// ���� ����
	m_pPrimitiveBatch->Begin();

	return S_OK;
}

HRESULT CBounding::End()
{
	m_pPrimitiveBatch->End();

	return S_OK;
}
#endif // _DEBUG

void CBounding::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

#ifdef _DEBUG
	if (false == m_isCloned)
	{
		// ���� �����Ҵ� ó���ϰ� �������� ó���Ѱű� ������ ���������� ��������.
		Safe_Delete(m_pPrimitiveBatch);
		Safe_Delete(m_pEffect);
	}
	Safe_Release(m_pInputLayout);
#endif // _DEBUG
}

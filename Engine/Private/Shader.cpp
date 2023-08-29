#include "..\Public\Shader.h"

CShader::CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{

}

CShader::CShader(const CShader& rhs)
	: CComponent(rhs)
	, m_pEffect(rhs.m_pEffect)
	, m_iNumPasses(rhs.m_iNumPasses)
	, m_InputLayouts(rhs.m_InputLayouts)
{
	for (auto& pair : m_InputLayouts)
		Safe_AddRef(pair.second);
	Safe_AddRef(m_pEffect);
}

HRESULT CShader::Initialize_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	_uint		iHlslFlag = { 0 };
#ifdef _DEBUG
	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	if (FAILED(D3DX11CompileEffectFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice, &m_pEffect, nullptr)))
		return E_FAIL;

	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	D3DX11_TECHNIQUE_DESC		TechniqueDesc;
	ZeroMemory(&TechniqueDesc, sizeof TechniqueDesc);

	pTechnique->GetDesc(&TechniqueDesc);

	m_iNumPasses = TechniqueDesc.Passes;

	for (_uint i = 0; i < m_iNumPasses; ++i)
	{
		ID3D11InputLayout* pInputLayout = { nullptr };

		ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(i);

		D3DX11_PASS_DESC	PassDesc;
		ZeroMemory(&PassDesc, sizeof PassDesc);
		pPass->GetDesc(&PassDesc);

		if (FAILED(m_pDevice->CreateInputLayout(pElements, iNumElements, 
			PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout)))
			return E_FAIL;

		if (nullptr == pInputLayout)
			return E_FAIL;

		m_InputLayouts.insert({ PassDesc.Name, pInputLayout });
	}


	return S_OK;
}

HRESULT CShader::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CShader::Begin(const _char* pPassName)
{
	if (nullptr == pPassName)
		return E_FAIL;

	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	ID3DX11EffectPass* pPass = pTechnique->GetPassByName(pPassName);
	if (nullptr == pPass)
		return E_FAIL;
		
	pPass->Apply(0, m_pContext);

	ID3D11InputLayout* pInputLayout = Find_InputLayout(pPassName);
	if (nullptr == pInputLayout)
		return E_FAIL;

	m_pContext->IASetInputLayout(pInputLayout);

	return S_OK;
}

HRESULT CShader::Bind_ShaderResource(const _char* pConstantName, ID3D11ShaderResourceView* pSRV)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pVariableShaderResource = pVariable->AsShaderResource();
	if (nullptr == pVariableShaderResource)
		return E_FAIL;

	return pVariableShaderResource->SetResource(pSRV);
}

HRESULT CShader::Bind_ShaderResources(const _char* pConstantName, ID3D11ShaderResourceView** ppSRVArray, _uint iNumTexture)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pVariableShaderResource = pVariable->AsShaderResource();
	if (nullptr == pVariableShaderResource)
		return E_FAIL;

	return pVariableShaderResource->SetResourceArray(ppSRVArray, 0, iNumTexture);
}

HRESULT CShader::Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;
	
	ID3DX11EffectMatrixVariable* pVariableMatrix = pVariable->AsMatrix();
	if (nullptr == pVariableMatrix)
		return E_FAIL;
	
	return pVariableMatrix->SetMatrix((_float*)pMatrix);
}

HRESULT CShader::Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrix, _uint iNumMatrices)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectMatrixVariable* pVariableMatrix = pVariable->AsMatrix();
	if (nullptr == pVariableMatrix)
		return E_FAIL;

	return pVariableMatrix->SetMatrixArray((_float*)pMatrix, 0, iNumMatrices);
}

HRESULT CShader::Bind_RawValue(const _char* pConstantName, const void* pData, _uint iSize)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetRawValue(pData, 0, iSize);
}

ID3D11InputLayout* CShader::Find_InputLayout(const _char* pPassName)
{
	ID3D11InputLayout* pInputLayout = { nullptr };

	auto iter = find_if(m_InputLayouts.begin(), m_InputLayouts.end(), [&](auto& pair) {
		if (!strcmp(pair.first, pPassName))
		{
			pInputLayout = pair.second;
			return true;
		}
		else
		{
			return false;
		}
	});

	if (m_InputLayouts.end() == iter)
		return nullptr;

	return pInputLayout;
}

CShader* CShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	CShader* pInstance = new CShader(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pShaderFilePath, pElements, iNumElements)))
	{
		MSG_BOX("Failed to Created CShader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CShader::Clone(void* pArg)
{
	CShader* pInstance = new CShader(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CShader");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CShader::Free()
{
	__super::Free();

	for (auto& pair : m_InputLayouts)
		Safe_Release(pair.second);
	m_InputLayouts.clear();

	Safe_Release(m_pEffect);
}

#pragma once
/* =============================================== */
// 
//	정 : 박정환
//	부 :
//
/* =============================================== */

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
private:
	CShader(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar * pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC * pElements, _uint iNumElements);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Begin(const _char* pPassName);

public:
	// 텍스처에서 만든 SRV를 던져 처리
	HRESULT Bind_ShaderResource(const _char * pConstantName, ID3D11ShaderResourceView * pSRV);
	// 텍스처에서 만든 SRV를 던져 처리
	HRESULT Bind_ShaderResources(const _char * pConstantName, ID3D11ShaderResourceView * *ppSRVArray, _uint iNumTexture);

	// 쉐이더파일에 바인딩 할 행렬
	HRESULT Bind_Matrix(const _char * pConstantName, const _float4x4 * pMatrix);
	// 쉐이더파일에 바인딩 할 행렬
	HRESULT Bind_Matrices(const _char * pConstantName, const _float4x4 * pMatrix, _uint iNumMatrices);
	// 쉐이더파일에 바인딩 할 벡터
	HRESULT Bind_Vectors(const _char * pConstantName, const _float3 * pVector, _uint iNumVectors);
	// 쉐이더파일에 바인딩 할 float 배열
	HRESULT Bind_FloatValues(const _char * pConstantName, const _float * pFloatValue, _uint iNumValues);
	// 쉐이더파일에 바인딩 할 Int 배열
	HRESULT Bind_IntValues(const _char * pConstantName, const _int * pIntValue, _uint iNumValues);
	// 쉐이더파일에 바인딩 할 일반 변수
	HRESULT Bind_RawValue(const _char * pConstantName, const void* pData, _uint iSize);

	// 셰이더 패스 이름들을 string 벡터에 담아서 반환한다.
	// 콤보박스에 활용하기 위해 만듬.
	vector<string> Get_PassList();

private:
	ID3DX11Effect*				m_pEffect = { nullptr };
	_uint						m_iNumPasses = { 0 };
	unordered_map<const _char*, ID3D11InputLayout*>	m_InputLayouts;

private:
	ID3D11InputLayout* Find_InputLayout(const _char * pPassName);

public:
	static CShader* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC * pElements, _uint iNumElements);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
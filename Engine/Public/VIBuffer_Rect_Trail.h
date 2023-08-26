#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_Trail final : public CVIBuffer
{
public:
	typedef struct tagTrailDesc
	{
		_uint				iTrailNum;
		const _float4x4*	pHighLocalMatrix;
		const _float4x4*	pLowLocalMatrix;
		const _float4x4*	pPivotMatrix;
		const _float4x4*	pWorldMatrix;
	}TRAILDESC;

private:
	explicit CVIBuffer_Rect_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CVIBuffer_Rect_Trail(const CVIBuffer_Rect_Trail& rhs);
	virtual ~CVIBuffer_Rect_Trail() = default;

public:
	HRESULT Reset_Trail();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick();
	// 쉐이더 컬러 변수이름, 컬러값, 쉐이더 변수, 패스 인덱스
	virtual HRESULT Render(const _char* pConstantName, _float4 vColor, class CShader* pShader, const _char* pPassName);
	// 쉐이더 텍스처 변수이름, 텍스처 변수, 쉐이더 변수, 패스 인덱스
	virtual HRESULT Render(const _char* pConstantName, class CTexture* pTexture, class CShader* pShader, const _char* pPassName);

private:
	TRAILDESC			m_TrailDesc;

private:
	HRESULT Setup_ShaderResources(class CShader* pShader);
	
public:
	static CVIBuffer_Rect_Trail* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
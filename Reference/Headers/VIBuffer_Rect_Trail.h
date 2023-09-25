#pragma once
/* =============================================== */
// 
//	정 : 박대웅
//	부 :
//
/* =============================================== */

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_Trail final : public CVIBuffer
{
private:
	enum GROUP { ACTIVATED ,IN_HEAD, GROUP_END };

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
	typedef struct tagTrail
	{
		_float3 vHighPosition = { _float3() };
		_float3 vLowPosition = { _float3() };
	}TRAIL;

	typedef struct tagTrailData
	{
		TRAIL Trail;
		_float fTimeAcc = { 0.f };
	}TRAIL_DATA;

private:
	explicit CVIBuffer_Rect_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CVIBuffer_Rect_Trail(const CVIBuffer_Rect_Trail& rhs);
	virtual ~CVIBuffer_Rect_Trail() = default;

public:
	HRESULT Reset_Trail();
	HRESULT Reset_Trail(_float3 vHighPos, _float3 vLowPos);

public:
	virtual HRESULT Initialize_Prototype(_uint iTrailNum);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick();
	virtual void Tick_Spline();
	virtual void Tick_Lightning(vector<_float3>* posVec);

	// 쉐이더 컬러 변수이름, 컬러값, 쉐이더 변수, 패스 인덱스
	virtual HRESULT Render(const _char* pConstantName, _float4 vColor, class CShader* pShader, const _char* pPassName);

	// 쉐이더 텍스처 변수이름, 텍스처 변수, 쉐이더 변수, 패스 인덱스
	virtual HRESULT Render(const _char* pConstantName, class CTexture* pTexture, class CShader* pShader, const _char* pPassName);
	virtual HRESULT Render() override;

private:
	TRAILDESC			m_TrailDesc;
	VTXPOSTEX*			m_pVertices = { nullptr };

private:
	HRESULT Setup_ShaderResources(class CShader* pShader);
	
public:
	static CVIBuffer_Rect_Trail* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iTrailNum);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
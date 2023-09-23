#pragma once
#include"Engine_Defines.h"
#include "Component.h"
BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;

class CFlowMap :
    public CComponent
{
private:
	explicit CFlowMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CFlowMap() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pTargetTag);
	virtual HRESULT Render() override;

private:
	CShader* m_pShader = { nullptr };
	CVIBuffer_Rect* m_pBuffer = { nullptr };
	_float4x4	m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;
	_tchar		m_szTargetTag[MAX_PATH] = TEXT("");
	_float		m_fCycle = 0.15f;
	_float		m_fHalfCycle		={ 0.f	};
	_float		m_fFlowMapOffset0  ={	0.f	};
	_float		m_fFlowMapOffset1  ={	0.f	};

	_float		m_fFrameTime = { 0.f };
	_float		m_fFlowPower = { 0.05f };


	class CTexture* m_pFlowTexture1 = { nullptr };//flowtexture는 두장을 합성해야한다고함~
	class CTexture* m_pFlowTexture2 = { nullptr }; //
private:
	HRESULT Add_Components();




public:
	static CFlowMap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTargetTag);
	virtual CComponent* Clone(void* pArg) { return nullptr; }
	virtual void Free() override;
};

END
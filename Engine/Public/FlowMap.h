//#pragma once
//#include"Engine_Defines.h"
//#include "Component.h"
//BEGIN(Engine)
//class CShader;
//class CVIBuffer_Rect;
//
//class CFlowMap :
//    public CComponent
//{
//private:
//	explicit CFlowMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
//	virtual ~CFlowMap() = default;
//
//public:
//	virtual HRESULT Initialize_Prototype(const _tchar* pTargetTag, _float LightPower);
//	virtual HRESULT Render() override;
//
//private:
//	CShader* m_pShader = { nullptr };
//	CVIBuffer_Rect* m_pBuffer = { nullptr };
//	_float4x4	m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;
//	_tchar		m_szTargetTag[MAX_PATH] = TEXT("");
//	_float		m_fCycle = 0.15f;
//	_float		m_fFlowSpeed		={ 0.05f };
//	_float		m_fHalfCycle		={ 0.f	};
//	_float		m_fFlowMapOffset0  ={0.f	};
//	_float		m_fFlowMapOffset1  ={0.f	};
//
//private:
//	HRESULT Add_Components();
//
//public:
//	static CFlowMap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTargetTag, _float LightPower = 0.f);
//	virtual CComponent* Clone(void* pArg) { return nullptr; }
//	virtual void Free() override;
//};
//
//END
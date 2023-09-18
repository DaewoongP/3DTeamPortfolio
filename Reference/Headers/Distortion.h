#pragma once
#include"Engine_Defines.h"
#include "Component.h"
BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;

class CDistortion :
    public CComponent
{
private:
	explicit CDistortion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CDistortion() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pTargetTag);
	virtual HRESULT Render() override;

private:
	CShader* m_pShader = { nullptr };
	CVIBuffer_Rect* m_pBuffer = { nullptr };
	_float4x4			m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;
	_tchar				m_szTargetTag[MAX_PATH] = TEXT("");
	_float m_fFrameTime = { 0.f };


private:
	HRESULT Add_Components();

private:
	class CTexture* m_pTexture = { nullptr };
	class CTexture* m_pTexture2 = { nullptr };
	class CTexture* m_pTexture3 = { nullptr };
	
	

public:
	static CDistortion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTargetTag);
	virtual CComponent* Clone(void* pArg) { return nullptr; }
	virtual void Free() override; 
};

END

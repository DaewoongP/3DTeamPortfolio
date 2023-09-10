
#pragma once

#include "GameObject.h"
#include "Engine_Defines.h"
#include "Trail.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect_Trail;

END

BEGIN(Engine)
class ENGINE_DLL CTrail : public CGameObject
{
protected:
	CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrail(const CTrail& rhs);
	virtual ~CTrail() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* _pDirectoryPath, _uint _iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected: /* For. Component */
	
	string	m_strPass = "Default";
	_float4x4 m_PivotMatrix = _float4x4();
	_float4x4 m_HighLocalMatrix = _float4x4();
	_float4x4 m_LowLocalMatrix = _float4x4();

protected:
	CShader* m_pShader = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CVIBuffer_Rect_Trail* m_pBuffer = { nullptr };
	CTexture* m_pTexture = { nullptr };
	wstring m_wstrPath;

protected:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* _pDirectoryPath, _uint _iLevel = 0);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END


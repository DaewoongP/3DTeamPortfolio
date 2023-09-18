
#pragma once
#include "GameObject.h"
#include "Engine_Defines.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CCollider;
class CNavigation;
class CModel;
class CVIBuffer_Rect;
END

BEGIN(Engine)
class ENGINE_DLL CTexture_Flipbook : public CGameObject
{
protected:
	CTexture_Flipbook(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture_Flipbook(const CTexture_Flipbook& rhs);
	virtual ~CTexture_Flipbook() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel, const _tchar* pFilePath, _uint iWidthNum, _uint iHeightNum);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected: /* For. Component */
	CShader* m_pShader = { nullptr };
	CTexture* m_pTexture = { nullptr };
	CTexture* m_pNormalTexture = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CVIBuffer_Rect* m_pBuffer = { nullptr };

protected:
	_uint m_iLevel = { 0 };
	wstring m_wstrPath;
	_uint m_iCurIndex = { 0 };
	_uint m_iMaxIndex = { 0 };
	_uint m_iWidthLength = { 0 };
	_uint m_iHeightLength = { 0 };

	string m_strClipChannel = { "Red" };
	_bool m_isAffectColor = { false };
	_float4 m_vColor = { 1.f, 1.f, 1.f, 1.f };

	string m_strLoopOption = { "Once" }; // Once, Loop, Ping-Pong

	_bool m_isUseNormalTexture = { false };
	wstring m_wstrNormalPath = { TEXT("")};

protected:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CTexture_Flipbook* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel, const _tchar* pFilePath, _uint iWidthNum, _uint iHeightNum);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END

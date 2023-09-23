#pragma once

//#include "Composite.h"
#include "GameObject.h"
#include "Model.h"

BEGIN(Engine)
class CRenderer;
class CTexture;
class CVIBuffer;
class CShader;
class CTransform;
END

BEGIN(Engine)
class ENGINE_DLL CMeshEffect : public CGameObject
{
protected:
	enum PATH { TEXTURE_PATH, ALPHA_CLIP_TEXTURE_PATH, MODEL_PATH, SHADER_PATH, PATH_END};

protected:
	explicit CMeshEffect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CMeshEffect(const CMeshEffect& _rhs);
	virtual ~CMeshEffect();

public:
	void Set_PassName(string strPassName);
	void Set_Path(wstring wstrPath, PATH ePath);

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath, _uint _iLevel);
	virtual HRESULT Initialize(void* _pArg) override;
	void Tick(_float _fTimeDelta) override;
	void Late_Tick(_float _fTimeDelta) override;
	HRESULT Render() override;
	
protected:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources();

	HRESULT Save(const _tchar* pFilePath);
	HRESULT Load(const _tchar* pFilePath);

protected:
	_uint m_iLevel = { 0 };
	_float2 m_vOffset = { 0.f, 0.f, };
	_float2 m_vTililing = { 1.f, 1.f };

protected:
	CModel::TYPE m_eAnimType;
	string m_strPassName = { "Default" };
	wstring m_Path[PATH_END];
	_bool m_isAlphaClipTexture = { false };
	_bool m_isAnimModel = { false };
	_float4x4 m_PivotMatrix = { _float4x4() };

protected:
	CModel* m_pModel = { nullptr };
	CTexture* m_pTexture = { nullptr };
	CTexture* m_pAlphaClipTexture = { nullptr };
	CShader* m_pShader = { nullptr };
	CRenderer* m_pRenderer = { nullptr };

public:
	static CMeshEffect* Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext, const _tchar* pFilePath, _uint _iLevel = 0);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END

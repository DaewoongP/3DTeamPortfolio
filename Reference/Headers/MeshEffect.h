#pragma once

//#include "Composite.h"
#include "GameObject.h"
#include "Modules.h"

BEGIN(Engine)
class CRenderer;
class CTexture;
class CVIBuffer;
class CShader;
class CModel;
class CTransform;
END

typedef struct ENGINE_DLL tagMeshEffect
{
	_float2 vOffset = { 0.f, 0.f };
}MESHEFFECT;

BEGIN(Engine)
class ENGINE_DLL CMeshEffect final : public CGameObject
{
	enum PATH { TEXTURE_PATH, MODEL_PATH, SHADER_PATH, PATH_END};

private:
	explicit CMeshEffect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CMeshEffect(const CMeshEffect& _rhs);
	virtual ~CMeshEffect() = default;

public:
	MESHEFFECT* Get_MeshEffect_Desc();

public:
	void Set_PassName(string strPassName);
	void Set_Path(wstring wstrPath, PATH ePath);

public:
	virtual HRESULT Initialize_Prototype(const _tchar * _pDirectoryPath);
	virtual HRESULT Initialize(void* _pArg) override;
	void Tick(_float _fTimeDelta) override;
	void Late_Tick(_float _fTimeDelta) override;
	HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources();

private:
	MESHEFFECT m_eMeshEffectDesc;

private:
	string m_strPassName = { "Default" };
	wstring m_Path[PATH_END];

private:
	CModel* m_pModel = { nullptr };
	CTexture* m_pTexture = { nullptr };
	CShader* m_pShader = { nullptr };
	CRenderer* m_pRenderer = { nullptr };

public:
	static CMeshEffect* Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext, const _tchar * _pDirectoryPath);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END
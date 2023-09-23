#pragma once
#include "GameObject.h"
#include"Client_Defines.h"

BEGIN(Engine)
class CMesh;
class CModel;
class CShader;
class CRenderer;
class CTexture;
END

BEGIN(Client)
class CMapEffect :
	public CGameObject
{
public:
	typedef struct tagMapObjectDesc
	{
		_float4x4 WorldMatrix; // 상태 행렬
		_uint iTagLen = { 0 }; // 문자열 길이
		_tchar wszTag[MAX_PATH] = TEXT(""); // 오브젝트 종류(모델 컴포넌트 이름)
	}MAPOBJECTDESC;

protected:
	enum PATH { TEXTURE_PATH, ALPHA_CLIP_TEXTURE_PATH, MODEL_PATH, SHADER_PATH, PATH_END };

private:
	explicit CMapEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMapEffect(const CMapEffect& _rhs);
	virtual ~CMapEffect() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	void Tick(_float _fTimeDelta) override;
	void Late_Tick(_float _fTimeDelta) override;
	HRESULT Render() override;

private:
	HRESULT Add_Components(MAPOBJECTDESC* pMapObjectDesc);
	HRESULT Setup_ShaderResources();

private:
	_uint m_iLevel = { 0 };
	_float2 m_vOffset = { 0.f, 0.f, };
	_float2 m_vTililing = { 1.f, 1.f };

private:
	CShader* m_pShader = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CModel* m_pModel = { nullptr };

public:
	static CMapEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END
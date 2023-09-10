#pragma once
#include "GameObject.h"
#include "Tool_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Tool)

class CTriangleColMesh final : public CGameObject
{
	// ÀÏ¹Ý ·»´õÀÎÁö ÇÇÅ·¿ë ·»´õÀÎÁö Á¤ÇØÁÜ
	enum eRenderType { RT_NORMAL, RT_PICKING, RT_END };

private:
	explicit CTriangleColMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTriangleColMesh(const CTriangleColMesh& rhs);
	virtual ~CTriangleColMesh() = default;

public:
	void	Set_Pos(_float3 vPos) { m_pTransform->Set_Position(vPos); }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	vector<_float3> Get_VertexPositions();
	vector<_ulong>  Get_Indices();
	HRESULT Add_Model_Component(const _tchar* wszModelTag);
	HRESULT Add_Shader_Component(const _tchar* wszShaderTag);

private:
	CShader* m_pShader = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CModel* m_pModel = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CTriangleColMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END

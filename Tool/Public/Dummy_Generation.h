#pragma once
#include "GameObject.h"
#include "Tool_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTriangleMesh;
class CModel;
END

BEGIN(Tool)

class CDummy_Generation final : public CGameObject
{
private:
	explicit CDummy_Generation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDummy_Generation(const CDummy_Generation& rhs);
	virtual ~CDummy_Generation() = default;

public:
	void	Set_Pos(_float3 vPos) { m_pTransform->Set_Position(vPos); }
	void	Set_WorldMatrix(_float4x4 vWorldMatrix) { m_pTransform->Set_WorldMatrix(vWorldMatrix); }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Add_Model_Component(const wchar_t* wszShaderTag);
	HRESULT Add_Shader_Component(const wchar_t* wszShaderTag);
	HRESULT Change_Model_Component(const _tchar* pPrototypeTag);

private:
	CShader*			m_pShader = { nullptr };
	CRenderer*			m_pRenderer = { nullptr };
	CModel*				m_pModel = { nullptr };
	CTriangleMesh*		m_pTriangleMesh = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CDummy_Generation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END

#pragma once
#include "GameObject.h"
#include "Tool_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CModel;
END

BEGIN(Tool)

class CMapDummy final : public CGameObject
{
private:
	explicit CMapDummy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMapDummy(const CMapDummy& rhs);
	virtual ~CMapDummy() = default;

public:
	_tchar* Get_GameObject_Tag() { return m_pTag; }
	void	Set_GameObject_Tag(_tchar* wszTag) { lstrcpy(m_pTag, wszTag); }
	void	Set_Pos(_float3 vPos) { m_pTransform->Set_Position(vPos); }

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

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CMapDummy* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END

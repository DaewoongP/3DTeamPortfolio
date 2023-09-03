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

class CMapObject final : public CGameObject
{
public:
	void Set_Color(int iColor) {
		m_vColor.x = ((iColor >> 24) & 0xff) * 1.f; // r
		m_vColor.y = ((iColor >> 16) & 0xff) * 1.f; // g
		m_vColor.z = ((iColor >> 8) & 0xff) * 1.f;  // b
		m_vColor.w = (iColor & 0xff) * 1.f;			// a
	}

private:
	explicit CMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMapObject(const CMapObject& rhs);
	virtual ~CMapObject() = default;

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
	HRESULT Add_Model_Component(const _tchar* wszModelTag);
	HRESULT Add_Shader_Component(const _tchar* wszShaderTag);

private:
	CShader* m_pShader = { nullptr };
	CTexture* m_pTexture = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CModel* m_pModel = { nullptr };

	_float4 m_vColor = { 0.f, 0.f, 0.f, 0.f }; // Fast Picking에 사용할 객체마다 고유한 색상 값, rgba 순이다.

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CMapObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END

#pragma once
/* =============================================== */
//	[CDugbog]
// 
//	정 : 주성환
//	부 :
//
/* =============================================== */

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CRigidBody;
class CRootBehavior;
END

BEGIN(Client)

class CDugbog final : public CGameObject
{
private:
	explicit CDugbog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDugbog(const CDugbog& rhs);
	virtual ~CDugbog() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth() override;

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };
	CRootBehavior* m_pRootBehavior = { nullptr };

private:
	HRESULT Make_AI();
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

#ifdef _DEBUG
	_int m_iIndex = { 0 };
	void Tick_ImGui();
#endif // _DEBUG

public:
	static CDugbog* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
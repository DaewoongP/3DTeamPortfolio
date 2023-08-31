#pragma once

/* =============================================== */
//	[CTest_Enemy]
//	- �׽�Ʈ�� �� ��ü (���߿� ������� �̸� ������ ��)
// 
//	�� : �ּ�ȯ
//	�� :
//
/* =============================================== */

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
END

BEGIN(Client)

class CTest_Enemy final : public CGameObject
{
private:
	explicit CTest_Enemy(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CTest_Enemy(const CTest_Enemy& rhs);
	virtual ~CTest_Enemy() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void OnCollisionEnter(COLLISIONDESC CollisionDesc) override;
	virtual void OnCollisionStay(COLLISIONDESC CollisionDesc) override;
	virtual void OnCollisionExit(COLLISIONDESC CollisionDesc) override;

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRenderer = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CTest_Enemy* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CTest_Enemy* Clone(void* pArg);
	virtual void Free() override;
};

END
#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CRigidBody;
END

BEGIN(Client)
class CMagicBall;
class CMagicSlot;
class CWeapon_Fig_Wand;
END

BEGIN(Client)

class CCard_Fig final : public CGameObject
{
public:
	typedef struct tagCardFigInitDesc
	{
		const _float4x4* pParentWorldMatrix = { nullptr };
	}CARDFIGINITDESC;

public:
	void On_Enter_Vault_Script() { m_isEnterVault = true; }
	void On_Dragon_Death() { m_isDragonDeath = true; }
	void On_Dragon_Hp_Down_Script() { m_isDragonHpDown = true; }

private:
	explicit CCard_Fig(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCard_Fig(const CCard_Fig& rhs);
	virtual ~CCard_Fig() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix) override;

public:
	void Spawn_Fig(const CGameObject* pTarget);

private:
	_float4x4 m_OffsetMatrix;
	const _float4x4* m_pParentWorldMatrix = { nullptr };
	
	const CGameObject* m_pTarget = { nullptr };
	_bool m_isSpawn = { false };
	_bool m_isAction1 = { true };
	_bool m_isAction2 = { false };
	_bool m_isAction3 = { false };
	_bool m_isAction4 = { false };

	_float m_fTimeAcc = { 0.f };

	vector<wstring> m_AttackTags;
	_uint m_iCurrentTagIndex = { 0 };

private: /* 피그 교수 이벤트 관련 데이터 */
	_bool m_isEnterVault = { false };
	_bool m_isDragonDeath = { false };
	_bool m_isDragonHpDown = { false };

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CShader* m_pShadowShaderCom = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CMagicSlot* m_pMagicSlot = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };
	CWeapon_Fig_Wand* m_pWeapon = { nullptr };

	CMagicBall* m_CastingMagic = { nullptr };

private:
	HRESULT Make_Magics();
	HRESULT Make_Notifies();
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix);

private:
	void Action(const _float& fTimeDelta);
	void Jump_Up(const _float& fTimeDelta);

private: /* Notify */
	void Cast_Crucio();
	void Cast_Bombarda();
	void Cast_Finisher();
	void Shot_Magic();

public:
	static CCard_Fig* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
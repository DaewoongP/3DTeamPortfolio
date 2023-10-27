#pragma once
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
class CMagicBall;
class CMagicSlot;
class CWeapon_Fig_Wand;
class CUI_Dissolve;
class CScript;
END

BEGIN(Client)

class CCard_Fig final : public CGameObject
{
private:
	enum CARDFIGSCRIPT
	{
		ENTERVAULT,
		ENTERSANCTUM,
		CREATEDRAGON,
		DRAGONHPDOWN,
		DRAGONDEATH,
		CARDFIGSCRIPT_END
	};

public:
	typedef struct tagCardFigInitDesc
	{
		const _float4x4* pParentWorldMatrix = { nullptr };
	}CARDFIGINITDESC;

public:
	void On_Enter_Vault_Script() { m_isEnterVault = true; }
	void On_Enter_Sanctum_Script() { m_isEnterSanctum = true; }
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
	void		Set_ShowCard(_bool isShow = true) {
		m_isShowCard = isShow;
	}
	void		Set_CreateDragon(_bool isCreate = true) {
		m_isCreateDragon = isCreate;
	}

public:
	void Spawn_Fig(CGameObject* pTarget);

private:
	_float4x4 m_OffsetMatrix;
	const _float4x4* m_pParentWorldMatrix = { nullptr };

	CGameObject* m_pTarget = { nullptr };
	_bool m_isSpawn = { false };
	_bool m_isJump = { false };
	_bool m_isChangeAnimation = { false };

	_float m_fTimeAcc = { 0.f };

	vector<wstring> m_AttackTags;
	_uint m_iCurrentTagIndex = { 0 };

private: /* 피그 교수 이벤트 관련 데이터 */
	_bool m_isEnterVault = { false };
	_bool m_isEnterSanctum = { false };
	_bool m_isCreateDragon = { false };
	_bool m_isDragonHpDown = { false };
	_bool m_isDragonDeath = { false };

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CShader* m_pShadowShaderCom = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CMagicSlot* m_pMagicSlot = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };
	CWeapon_Fig_Wand* m_pWeapon = { nullptr };
	CRootBehavior* m_pRootBehavior = { nullptr };

	CMagicBall* m_CastingMagic = { nullptr };

	CUI_Dissolve* m_pUI_Card = { nullptr };
	vector<CScript*> m_pScripts = { nullptr };

private:
	HRESULT Make_AI();
	HRESULT Make_Magics();
	HRESULT Make_Notifies();
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix);

private:
	void Jump_Up(const _float& fTimeDelta);

private: /* Notify */
	void Change_Animation() {
		m_isChangeAnimation = true;
	}
	void Cast_Crucio();
	void Cast_Bombarda();
	void Cast_Finisher();
	void Shot_Magic();

private:
	_bool		m_isShowCard = { false };
	_bool		m_isPlayScript = { false };
	_uint		m_iScriptIndex = { 0 };

	_bool		m_isEnterValutScriptEnd = { false };
	_bool		m_isEnterSanctumScriptEnd = { false };
	_bool		m_isCreateDragonScriptEnd = { false };
	_bool		m_isDragonHpDownScriptEnd = { false };
	_bool		m_isDragonDeathScriptEnd = { false };

private:
	void On_Gravity();
	void Off_Gravity();

private: // Four UI
	void		Ready_Card_UI();
	void		Script_Finish_Check();
	void		Play_Script(_float fTimeDelta);

public:
	static CCard_Fig* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
#pragma once
/* =============================================== */
//	[CWolf]
// 
//	�� : ����ȯ ���� ������
//	�� :
//
/* =============================================== */

#include "Enemy.h"
#include "Client_Defines.h"

BEGIN(Client)

class CWolf final : public CEnemy
{
public:
	enum W0LF_ANIMATION {
		WF_ATTACK_START, WF_TRUN_180, WF_ATTACK_END, WF_BARK, WF_HOWL, WF_IDLE, WF_IDLE_END, 
		WF_RUN_START, WF_ATTACK_AFTER_180_TURN, WF_JUMP_R, WF_ATTACK, WF_DIE, WF_GETUP, WF_PROTEGO,
		WF_LEVIOSO_END, WF_LEVIOSO_LOOP, WF_LEVIOSO_START,
		WF_2SPIN, WF2SPIN_2, WF_1SPIN, WF_END
	};

private:
	explicit CWolf(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CWolf(const CWolf& rhs);
	virtual ~CWolf() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionExit(COLLEVENTDESC CollisionEventDesc) override;
	virtual HRESULT Render() override;

private:
	CTexture* m_pDissolveTexture = { nullptr };				 // ������� ������ �ؽ�ó

private:
	_float m_fDeathTimeAcc = { 0.f }; // ��� �ִϸ��̼��� ������ �� �ڿ� ���� ����
	_float m_fLeviosoDur = { 0.f };   // ������� ���� �ð�
	_float m_fDecendoDur = { 0.f };   // �𼾵� ���� �ð�
	_float m_fDissolveAmount = { 0.f }; // ������ ��

	_bool m_isDecendo = { false }; // �𼾵��� �ǰ��ߴ°�?
	_bool m_isDissolveStart = { false };


private:
	W0LF_ANIMATION m_eCurrentAnim = { WF_END };
	W0LF_ANIMATION m_ePreAnim = { m_eCurrentAnim }; // ���� �ִϸ��̼��� ����

private:
	HRESULT Make_Notifies();
	HRESULT Add_Components();
	HRESULT Add_Components_Level(_uint iCurrentLevelIndex);
	HRESULT Bind_HitMatrices();
	HRESULT SetUp_ShaderResources();

private:
	void Wolf_Attack();						// ���� ����, Notify
	void Wolf_ChangeAnim();					// �߰� �����ӿ��� �ִϸ��̼� ����, Notify

	void Wolf_Animation();					// ���� �ִϸ��̼� ����
	void Wolf_Turn(_float fTimeDelta);		// ���밡 �÷��̾ ���� ȸ��
	void Wolf_Levioso(_float fTimeDelta);	// ������� �¾��� �� ����
	void Wolf_Decendo(_float fTimeDelta);	// �𼾵� �¾��� �� ����
	void Wolf_Protego();					// �����װ� ������ �� ����

public:
	static CWolf* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
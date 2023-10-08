#pragma once

/* =============================================== */
//	[CAction]
//	: ��ü�� �ִϸ��̼��� ����ϴ� Ŭ����
//	�� : �ּ�ȯ
//	�� :
//
/* =============================================== */

#include "Model.h"
#include "Behavior.h"
#include "Client_Defines.h"

BEGIN(Client)

class CAction final : public CBehavior
{
private:
	explicit CAction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CAction(const CAction& rhs);
	virtual ~CAction() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Tick(const _float& fTimeDelta);

public:
	/*
		�Ű�����
		1. ���ε� �� �ִϸ��̼� �±�
		2. �� ������
		3. �ൿ üũ �Ұ��� (Action�� ���ε� �� ����� ������ Ż���ϱ� ���ϴ� ��� true�� ����)
		4. ��Ÿ��
		5. �ѹ��� ���� �� �ൿ����
		6. �ִϸ��̼� ���� ����
		7. ����ü ������ ���) ��ü�ִϸ��̼����� ��ü�ִϸ��̼�����
	*/
	void Set_Options(const wstring& _wstrAnimationTag, CModel* _pModel,
		_bool _isCheckBehavior = false, const _float& _fCoolTime = 0.f,
		_bool _isOneTimeAction = false, _bool _isLerp = true,
		CModel::ANIMTYPE eType = CModel::UPPERBODY);
	/* Action Ŭ���� Ż�� ���� ���� �Լ�. */
	HRESULT Add_Exit_Condition(function<_bool(class CBlackBoard*)> Func);

public:
	virtual void Reset_Behavior(HRESULT result) override;

private:
	_float m_fLimit = { 0.f };
	_float m_fPreWorldTimeAcc = { 0.f };

	wstring m_wstrAnimationTag = { TEXT("") };

	_bool m_isFinishBehaviors = { false };
	_bool m_isCheckBehavior = { false };
	_bool m_isOneTimeAction = { false };
	_bool m_isEndFirstPlay = { false };
	_bool m_isFirst = { true };

	list<CDecorator*>	m_ConditionFunctions;

private:
	CModel* m_pModel = { nullptr };
	CModel::ANIMTYPE m_eAnimationType = { CModel::ANIM_END };

private:
	_bool Is_Success_Action();
	_bool Check_Exit_Conditions();

public:
	static CAction* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CAction* Clone(void* pArg) override;
	virtual void Free() override;
};

END
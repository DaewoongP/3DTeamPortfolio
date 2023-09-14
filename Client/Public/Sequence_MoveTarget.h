#pragma once

/* =============================================== */
//	[CSequence_MoveTarget]
//	: ��ü�� ���� �̵��Ѵ�.
//	�� : �ּ�ȯ
//	�� : 
//
/* =============================================== */

#include "Sequence.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CTransform;
END

BEGIN(Client)
class CAction;
END

BEGIN(Client)

class CSequence_MoveTarget final : public CSequence
{
private:
	explicit CSequence_MoveTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSequence_MoveTarget(const CSequence_MoveTarget& rhs);
	virtual ~CSequence_MoveTarget() = default;

public:
	/*
		�Ű�����
		1. ���ε� �� �ִϸ��̼� �±�
		2. �� ������
		3. �ൿ üũ �Ұ��� (Action�� ���ε� �� ����� ������ Ż���ϱ� ���ϴ� ��� true�� ����)
		4. ��Ÿ��
		5. Ÿ�̸� �±�
		6. �� �� ���� ������ ����
		7. �ѹ��� ���� �� �ൿ����
		8. �ִϸ��̼� ���� ����
	*/
	void Set_Action_Options(const wstring& _wstrAnimationTag, CModel* _pModel,
		_bool _isCheckBehavior = false, const _float& _fCoolTime = 0.f,
		const wstring& _wstrTimerTag = TEXT(""), const _float& _fDurationTime = 0.f,
		_bool _isOneTimeAction = false, _bool _isLerp = true);

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Tick(const _float& fTimeDelta);

private:
	virtual HRESULT Assemble_Childs() override;

private:
	_float m_fLimit = { 0.f };
	_float m_fPreWorldTimeAcc = { 0.f };

private:
	CAction* m_pAction = { nullptr };

public:
	static CSequence_MoveTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSequence_MoveTarget* Clone(void* pArg) override;
	virtual void Free() override;
};

END
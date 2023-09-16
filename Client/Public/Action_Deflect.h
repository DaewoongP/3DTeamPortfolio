#pragma once

/* =============================================== */
//	[CAction_Deflect]
//	: ��ü�� �и� �ִϸ��̼��� ����ϴ� Ŭ����
//	�� : �ּ�ȯ
//	�� :
//
/* =============================================== */

#include "Behavior.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CAction_Deflect final : public CBehavior
{
private:
	explicit CAction_Deflect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CAction_Deflect(const CAction_Deflect& rhs);
	virtual ~CAction_Deflect() = default;

public:
	/*
		�Ű�����
		1. ���ε� �� �ִϸ��̼� �±�
		2. �� ������
		3. �ൿ üũ �Ұ��� (Action�� ���ε� �� ����� ������ Ż���ϱ� ���ϴ� ��� true�� ����)
		4. ��Ÿ��
		5. �ѹ��� ���� �� �ൿ����
		6. �ִϸ��̼� ���� ����
	*/
	void Set_Options(const wstring& _wstrAnimationTag, CModel* _pModel,
		_bool _isCheckBehavior = false, const _float& _fCoolTime = 0.f,
		_bool _isOneTimeAction = false, _bool _isLerp = true);
	/* ���� �� �ൿ�� ���������� */
	_bool isPlayAction() const {
		return m_isPlay;
	}

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Tick(const _float& fTimeDelta);

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
	_bool m_isPlay = { false }; // ���� ����������.
	_bool m_isFirst = { true };

private:
	CModel* m_pModel = { nullptr };

public:
	static CAction_Deflect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CAction_Deflect* Clone(void* pArg) override;
	virtual void Free() override;
};

END
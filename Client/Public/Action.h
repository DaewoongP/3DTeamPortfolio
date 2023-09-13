#pragma once

/* =============================================== */
//	[CAction]
//	: ��ü�� �ִϸ��̼��� ����ϴ� Ŭ����
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
		5. Ÿ�̸� �±�
		6. �� �� ���� ������ ����
		7. �ѹ��� ���� �� �ൿ����
		8. �ִϸ��̼� ���� ����
	*/
	void Set_Options(const wstring& _wstrAnimationTag, CModel* _pModel, 
		_bool _isCheckBehavior = false, const _float& _fCoolTime = 0.f,
		const wstring& _wstrTimerTag = TEXT(""), const _float & _fDurationTime = 0.f,
		_bool _isOneTimeAction = false, _bool _isLerp = true);

private:
	_float m_fLimit = { 0.f };
	_float m_fPreWorldTimeAcc = { 0.f };

	wstring m_wstrAnimationTag = { TEXT("") };
	wstring m_wstrTimerTag = { TEXT("") };

	_bool m_isFinishBehaviors = { false };
	_bool m_isCheckBehavior = { false };
	_bool m_isOneTimeAction = { false };
	_bool m_isPlayAction = { false };

private:
	CModel* m_pModel = { nullptr };

public:
	static CAction* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CAction* Clone(void* pArg) override;
	virtual void Free() override;
};

END
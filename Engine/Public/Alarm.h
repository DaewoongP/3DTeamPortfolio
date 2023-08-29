#pragma once

/* =============================================== */
//	[CAlarm]
//	- �˶� Ŭ���� ��.. ���ּ���..(�Ʒ�)
// 
//	�� : �ּ�ȯ
//	�� :
//
/* =============================================== */

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CAlarm final : public CComponent
{
public:
	typedef struct tagAlarmDesc
	{
		// �ݺ� ����. false�� �����ϸ� ���� �ð� ���� ��� true�� ��ȯ��.
		_bool m_isRepeat = { false };
		// ���� �ð�
		_float m_fDuration = { 0.f };
		// ���� �ð� (���� �����൵ ��)
		_float m_fTimeAcc = { 0.f };
	}ALARMDESC;

private:
	explicit CAlarm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CAlarm(const CAlarm& rhs);
	virtual ~CAlarm() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override { return S_OK; }
	/* (����) ��� ��Ų Ÿ�̸Ӹ� ��ü ��ȸ �ϸ鼭 m_fTimeAcc�� ������Ŵ	*/
	virtual void Tick(_float fTimeDelta) override;

public:
	HRESULT Add_Timer(const wstring& wstrTimerTag, const ALARMDESC& AlarmDesc);
	HRESULT Remove_Timer(const wstring& wstrTimerTag);
	/* Ÿ�̸��� m_fTimeAcc�� 0.f�� �ʱ�ȭ */
	HRESULT Reset_Timer(const wstring& wstrTimerTag);
	/* �������� �ð� üũ �Լ�. �ش� �±��� �ð��� Duration���� ���� ��� false�� ��ȯ.
	   ũ�ų� ������ true ��ȯ */
	_bool Check_Timer(const wstring& wstrTimerTag);

private:
	unordered_map<wstring, ALARMDESC> m_Timers;

public:
	static CAlarm* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CAlarm* Clone(void* pArg);
	virtual void Free() override;
};

END
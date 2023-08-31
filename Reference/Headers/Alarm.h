#pragma once

/* =============================================== */
//	[CAlarm]
//	- 알람 클래스 잘.. 써주세요..(아련)
// 
//	정 : 주성환
//	부 :
//
/* =============================================== */

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CAlarm final : public CComponent
{
public:
	typedef struct tagAlarmDesc
	{
		// 반복 유무. false로 설정하면 설정 시간 이후 계속 true를 반환함.
		_bool m_isRepeat = { false };
		// 설정 시간
		_float m_fDuration = { 0.f };
		// 누적 시간 (설정 안해줘도 됨)
		_float m_fTimeAcc = { 0.f };
	}ALARMDESC;

private:
	explicit CAlarm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CAlarm(const CAlarm& rhs);
	virtual ~CAlarm() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override { return S_OK; }
	/* (주의) 등록 시킨 타이머를 전체 순회 하면서 m_fTimeAcc를 증가시킴	*/
	virtual void Tick(_float fTimeDelta) override;

public:
	HRESULT Add_Timer(const wstring& wstrTimerTag, const ALARMDESC& AlarmDesc);
	HRESULT Remove_Timer(const wstring& wstrTimerTag);
	/* 타이머의 m_fTimeAcc를 0.f로 초기화 */
	HRESULT Reset_Timer(const wstring& wstrTimerTag);
	/* 실질적인 시간 체크 함수. 해당 태그의 시간이 Duration보다 작은 경우 false를 반환.
	   크거나 같으면 true 반환 */
	_bool Check_Timer(const wstring& wstrTimerTag);

private:
	unordered_map<wstring, ALARMDESC> m_Timers;

public:
	static CAlarm* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CAlarm* Clone(void* pArg);
	virtual void Free() override;
};

END
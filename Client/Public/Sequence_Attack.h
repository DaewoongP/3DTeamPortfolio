#pragma once

/* =============================================== */
//	[CSequence_Attack]
//	: 객체의 공격관련 행동을 담당하는 클래스
//	정 : 주성환
//	부 : 
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

class CSequence_Attack final : public CSequence
{
private:
	explicit CSequence_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSequence_Attack(const CSequence_Attack& rhs);
	virtual ~CSequence_Attack() = default;

public:
	/*
		매개변수
		1. 바인딩 할 애니메이션 태그
		2. 모델 포인터
		3. 행동 체크 할건지 (Action에 바인딩 한 비헤비어가 성공시 탈출하길 원하는 경우 true로 변경)
		4. 쿨타임
		5. 타이머 태그
		6. 몇 초 동안 진행할 건지
		7. 한번만 실행 할 행동인지
		8. 애니메이션 러프 유무
	*/
	void Set_Attack_Action_Options(const wstring& _wstrAnimationTag, CModel* _pModel,
		_bool _isCheckBehavior = false, const _float& _fCoolTime = 0.f,
		const wstring& _wstrTimerTag = TEXT(""), const _float& _fDurationTime = 0.f,
		_bool _isOneTimeAction = false, _bool _isLerp = true);
	/* 공격 사거리 세팅 */
	void Set_Attack_Option(const _float& _fRange) {
		m_fAttackRange = _fRange;
	}

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Tick(const _float& fTimeDelta);

private:
	virtual HRESULT Assemble_Childs() override;

private:
	_float m_fLimit = { 0.f };
	_float m_fPreWorldTimeAcc = { 0.f };
	_float m_fAttackRange = { 0.f };

private:
	CAction* m_pAction = { nullptr };

public:
	static CSequence_Attack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSequence_Attack* Clone(void* pArg) override;
	virtual void Free() override;
};

END
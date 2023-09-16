#pragma once

/* =============================================== */
//	[CAction_Deflect]
//	: 객체의 패링 애니메이션을 담당하는 클래스
//	정 : 주성환
//	부 :
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
		매개변수
		1. 바인딩 할 애니메이션 태그
		2. 모델 포인터
		3. 행동 체크 할건지 (Action에 바인딩 한 비헤비어가 성공시 탈출하길 원하는 경우 true로 변경)
		4. 쿨타임
		5. 한번만 실행 할 행동인지
		6. 애니메이션 러프 유무
	*/
	void Set_Options(const wstring& _wstrAnimationTag, CModel* _pModel,
		_bool _isCheckBehavior = false, const _float& _fCoolTime = 0.f,
		_bool _isOneTimeAction = false, _bool _isLerp = true);
	/* 현재 이 행동이 진행중인지 */
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
	_bool m_isPlay = { false }; // 현재 진행중인지.
	_bool m_isFirst = { true };

private:
	CModel* m_pModel = { nullptr };

public:
	static CAction_Deflect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CAction_Deflect* Clone(void* pArg) override;
	virtual void Free() override;
};

END
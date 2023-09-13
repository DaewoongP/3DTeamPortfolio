#pragma once

/* =============================================== */
//	[CAction]
//	: 객체의 애니메이션을 담당하는 클래스
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
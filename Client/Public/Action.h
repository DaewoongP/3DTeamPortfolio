#pragma once

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
	/*	1. 바인딩 할 애니메이션 태그
		2. 쿨타임
		3. 한번만 실행 할 행동인지
		4. 애니메이션 러프 유무
	*/
	void Set_Options(const wstring& _strAnimationTag, CModel* _pModel, const _float& _fCoolTime = 0.f,
		_bool _isOneTimeAction = false, _bool _isLerp = true);

private:
	_float m_fLimit = { 0.f };
	_float m_fTimeAcc = { 0.f };
	_float m_fPreTimeAcc = { 0.f };

	wstring m_wstrAnimationTag = { TEXT("") };

	_bool m_isFinishBehaviors = { false };
	_bool m_isOneTimeAction = { false };
	_bool m_isFirstAction = { true };
	_bool m_isFirst = { true };
	_bool m_isLerp = { true };

private:
	CModel* m_pModel = { nullptr };

public:
	static CAction* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CAction* Clone(void* pArg) override;
	virtual void Free() override;
};

END
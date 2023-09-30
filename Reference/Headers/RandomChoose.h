#pragma once

/* =============================================== */
//	[CRandomChoose]
//	- ����ġ�� ���� �ൿ�� ���� ����
// 
//	�� : �ּ�ȯ
//	�� :
//
/* =============================================== */

#include "Behavior.h"

BEGIN(Engine)
class CDecorator;
END

BEGIN(Engine)

class ENGINE_DLL CRandomChoose : public CBehavior
{
protected:
	explicit CRandomChoose(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CRandomChoose(const CBehavior& rhs);
	virtual ~CRandomChoose() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override { return S_OK; }
	virtual HRESULT Tick(const _float & fTimeDelta) override;

public:
	virtual HRESULT Assemble_Behavior(const wstring& BehaviorTag, CBehavior* pBehavior, const _float& fWeight);
	virtual void Reset_Behavior(HRESULT result) override;

	void Set_Random_Behavior();
	/* Ư�� ��ȯ������ Behavior�� ������ ������ �߰��Ѵ�. */
	HRESULT Add_Change_Condition(HRESULT ReturnType, function<_bool(class CBlackBoard*)> Func);

protected:
	/* CurrentBehavior�� ���� �� �� �����ϴ� ���ǵ��� ������ ����Ʈ */
	list<CDecorator*>	m_ConditionFunctions[BEHAVIOR_END];
	vector<_float>		m_ChildWeights;

protected:
	virtual HRESULT Assemble_Childs() override { return S_OK; }
	_bool Check_ChangeConditions(HRESULT eBehaviorType);

public:
	static CRandomChoose* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CRandomChoose* Clone(void* pArg) override;
	virtual void Free() override;
};

END
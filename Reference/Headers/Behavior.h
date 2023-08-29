#pragma once
#include "Component.h"

/*
	Behavior트리의 구성요소는 다음과 같다.
	Root	-	ai 트리의 시작단계. 블랙보드를 가지고 있으며 모든 ai는
				각자의 root를 가지고 있다.

	Selector-	ai의 세부 분리자 역할1. 깊이우선 탐색의 역할을 가지고 있으며,
				자식 노드가 false를 반환하면 다음 노드를 계속 탐색해서 true를 반환하는
				노드를 만날 때 까지 순회를 한다. 만약 true를 반환하는 노드를 만날 경우
				true를 반환하고 모든 노드를 순회했는데 다 false인경우 false를 반환한다.

	Sequence-	ai의 세부 분리자 역할2. 모든 노드를 순회하는 역할을 가지고 있으며,
				자식노드가 하나이라도 False를 반환할 경우 False를 반환한다.
				하지만 true를 반환하면 다음 자식노드로 이동한다.
				그리고 모든 자식 노드들을 순회 한 경우 True를 반환한다.

	Task	-	ai의 말단 기능부. ai의 기능을 담당하고 있으며, 프로그래머가
				필요한 경우 Behavior클래스를 상속받아서 기능을 구현하면 된다.

	이 중 프로그래머가 직접적으로 제작을 맡는 부분은 Task 부분이다.
*/

BEGIN(Engine)

class ENGINE_DLL CBehavior abstract : public CComponent
{
protected:
	enum RETURNVALUE { BEHAVIOR_SUCCESS, BEHAVIOR_RUNNING, BEHAVIOR_FAIL, BEHAVIOR_ERROR, BEHAVIOR_END };

protected:
	explicit CBehavior(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBehavior(const CBehavior& rhs);
	virtual ~CBehavior() = default;

public:
	const HRESULT& Get_Return_Data() const {
		return m_ReturnData;
	}
	void Set_Return_Data(const HRESULT& iReturnData) {
		m_ReturnData = iReturnData;
	}

public:
	virtual HRESULT Initialize_Prototype() override = 0;
	virtual HRESULT Initialize(void* pArg) override = 0;
	virtual HRESULT Tick(const _float & fTimeDelta) = 0;

public:
	HRESULT Add_Decoration(function<_bool(class CBlackBoard*)> Func);
	HRESULT Assemble_Behavior(const wstring & BehaviorTag, CBehavior * pBehavior);

protected:
	wstring m_wstrBehaviorTag = { L"" };
	/* 블랙보드는 루트노드당 하나씩 가지고 루트노드의 자식들은 루트노드의 블랙보드를 공유하는 형태로 가져갈 것이기 때문에 블랙보드의 생성은 루트노드에서 수행한다. */
	class CBlackBoard* m_pBlackBoard = { nullptr };
	/* 비헤비어트리의 상위 노드 포인터 */
	CBehavior* m_pParentBehavior = { nullptr };
	HRESULT m_ReturnData = { 0 };

protected:
	list<class CDecorator*> m_Decorations;
	list<CBehavior*> m_Behaviors;
	list<CBehavior*>::iterator m_iterCurBehavior;

protected:
	_bool Check_Decorations();
	CBehavior* Find_Behavior(const wstring & BehaviorTag);

protected:
	virtual CBehavior* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
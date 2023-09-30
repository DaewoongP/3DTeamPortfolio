#pragma once
#include "Component.h"

/* =============================================== */
//	[CBehavior]
// 
//	�� : �ּ�ȯ
//	�� :
//
/* =============================================== */

/*
	BehaviorƮ���� ������Ҵ� ������ ����.
	RootBehavior - ai Ʈ���� ���۴ܰ�. �����带 ������ ������ ��� ai��
				 ������ root�� ������ �ִ�.
				 
	Selector	 - ai�� ���� �и��� ����1. ���̿켱 Ž���� ������ ������ ������,
				 �ڽ� ��尡 false�� ��ȯ�ϸ� ���� ��带 ��� Ž���ؼ� true�� ��ȯ�ϴ�
				 ��带 ���� �� ���� ��ȸ�� �Ѵ�. ���� true�� ��ȯ�ϴ� ��带 ���� ���
				 true�� ��ȯ�ϰ� ��� ��带 ��ȸ�ߴµ�, ���� false�ΰ�� false�� ��ȯ�Ѵ�.
				 
	Sequence	 - ai�� ���� �и��� ����2. ��� ��带 ��ȸ�ϴ� ������ ������ ������,
				 �ڽĳ�尡 �ϳ��̶� False�� ��ȯ�� ��� False�� ��ȯ�Ѵ�.
				 ������ true�� ��ȯ�ϸ� ���� �ڽĳ��� �̵��Ѵ�.
				 �׸��� ��� �ڽ� ������ ��ȸ �� ��� True�� ��ȯ�Ѵ�.
				 
	Task		 - ai�� ���� ��ɺ�. ai�� ����� ����ϰ� ������, ���α׷��Ӱ�
				 �ʿ��� ��� BehaviorŬ������ ��ӹ޾Ƽ� ����� �����ϸ� �ȴ�.

	�� �� ���α׷��Ӱ� ���������� ������ �ô� �κ��� Task �κ��̴�.
*/

BEGIN(Engine)

class ENGINE_DLL CBehavior abstract : public CComponent
{
public:
	enum RETURNVALUE { BEHAVIOR_SUCCESS, BEHAVIOR_RUNNING, BEHAVIOR_FAIL, BEHAVIOR_END };

protected:
	explicit CBehavior(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBehavior(const CBehavior& rhs);
	virtual ~CBehavior() = default;

public:
	HRESULT Get_ReturnData() const {
		return m_ReturnData;
	}

public:
	virtual HRESULT Initialize_Prototype() = 0;
	virtual HRESULT Initialize(void* pArg) = 0;
	virtual HRESULT Tick(const _float & fTimeDelta) = 0;

public:
	HRESULT Add_Decorator(function<_bool(class CBlackBoard*)> Func);
	/* �����̺�� ���� �� �����ϴ� �Լ��� �߰��Ѵ�. */
	HRESULT Add_End_Decorator(function<_bool(class CBlackBoard*)> Func);
	/* �����̺�� ���� �� �����ϴ� �Լ��� �߰��Ѵ�. */
	HRESULT Add_Success_Decorator(function<_bool(class CBlackBoard*)> Func);
	/* �����̺�� ���� �� �����ϴ� �Լ��� �߰��Ѵ�. */
	HRESULT Add_Fail_Decorator(function<_bool(class CBlackBoard*)> Func);
	/* ����. pBehavior�� AddRef�� �������� ������ SafeRelease�� ȣ���Ѵ�. */
	HRESULT Assemble_Behavior(const wstring & BehaviorTag, CBehavior * pBehavior);
	virtual void Reset_Behavior(HRESULT result) {};

protected:
	wstring m_wstrBehaviorTag = { L"" };

	/* ������� ��Ʈ���� �ϳ��� ������ ��Ʈ����� �ڽĵ��� ��Ʈ����� �����带 �����ϴ� ���·� ������ ���̱� ������ �������� ������ ��Ʈ��忡�� �����Ѵ�. */
	class CBlackBoard* m_pBlackBoard = { nullptr };
	HRESULT m_ReturnData = { 0 };

protected: /* For.Decorators */
	list<class CDecorator*> m_Decorators;
	list<class CDecorator*> m_EndDecorators; // �����̺�� ���� �� ������ �۾��� ������ �Լ���ü����Ʈ
	list<class CDecorator*> m_SuccessDecorators; // �����̺�� ���� ��ȯ�� ������ �۾��� ������ �Լ���ü����Ʈ
	list<class CDecorator*> m_FailDecorators; // �����̺�� ���� ��ȯ�� ������ �۾��� ������ �Լ���ü����Ʈ
	
protected: /* Behavior List */
	list<CBehavior*> m_Behaviors;
	list<CBehavior*>::iterator m_iterCurBehavior;

protected:
	_bool Check_Decorators(); // �ൿ�� �����ϴ� ���������� �����ϴ� �Լ����� üũ�Ѵ�.
	_bool Check_End_Decorators(); // �ൿ�� �����ϴ� �������� �����ϴ� �Լ����� üũ�Ѵ�.
	_bool Check_Success_Decorators(); // �ൿ�� Success�� ��ȯ�� ��쿡 �����ϴ� �Լ����� üũ�Ѵ�. 
	_bool Check_Fail_Decorators(); //�ൿ�� Fail�� ��ȯ�� ��쿡 �����ϴ� �Լ����� üũ�Ѵ�.
	CBehavior* Find_Behavior(const wstring & BehaviorTag);
	virtual HRESULT Assemble_Childs() { return S_OK; }

protected:
	virtual CBehavior* Clone(void* pArg) = 0;
	virtual void Free() override;

#ifdef _DEBUG
protected:
	void Find_Running_Behavior(_Inout_ vector<wstring>& BehaviorTags);
#endif // _DEBUG
};

END
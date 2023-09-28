#pragma once
#include "GameObject.h"
#include"Client_Defines.h"

BEGIN(Client)
class CQuest :
	public CGameObject
{
protected: // ������������ ���Բ� protected
	explicit CQuest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CQuest(const CQuest& rhs);
	virtual ~CQuest() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_uint Get_MonsterCount()	 { return m_iMonsterCount; }
	_uint GeT_KillCount()		 { return m_iMonster_Kill_Count; }
	_uint Get_BehaviorCount()	 { return m_iBehaviorCount; }
	_float Get_QuestTime()		 { return m_fQuestTime; }
	_bool Get_QuestResult()		 { return m_isComplete; }

	_uint Set_MonsterCount(_uint MonsterCount)	 { m_iMonsterCount=MonsterCount; }
	_uint Set_KillCount(_uint KillCount)		 {  m_iMonster_Kill_Count=KillCount; }
	_uint Set_BehaviorCount(_uint BehaviorCount) {  m_iBehaviorCount=BehaviorCount; }
	_float Set_QuestTime(_float QuestTime)		 {  m_fQuestTime= QuestTime; }
	_bool Set_QuestResult(_bool Result)			 {  m_isComplete= Result; }


private:
	_uint					m_iMonsterCount			= { 0 }; //���� ���� ������� �޾ƿ�����.
	_uint					m_iMonster_Kill_Count   = { 0 }; //���� ����׿�����.
	_uint					m_iBehaviorCount		= { 0 };  //�ൿī��Ʈ. ��� ~�� �ض�
	_float					m_fQuestTime			= { 0.f }; //Ÿ�Ӿ��� � ���.
	

	_bool					m_isComplete = { false };



public:
	virtual void Free() override;

};
END
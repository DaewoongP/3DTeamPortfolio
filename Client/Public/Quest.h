#pragma once
#include "GameObject.h"
#include"Client_Defines.h"

BEGIN(Client)
class CQuest :
	public CGameObject
{
protected: // 스테이지마다 갖게끔 protected
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
	_uint					m_iMonsterCount			= { 0 }; //몬스터 생성 몇마리인지 받아와주자.
	_uint					m_iMonster_Kill_Count   = { 0 }; //몬스터 몇마리죽였는지.
	_uint					m_iBehaviorCount		= { 0 };  //행동카운트. 몇번 ~를 해라
	_float					m_fQuestTime			= { 0.f }; //타임어택 등에 사용.
	

	_bool					m_isComplete = { false };



public:
	virtual void Free() override;

};
END
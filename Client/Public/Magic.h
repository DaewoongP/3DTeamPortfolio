#pragma once
/* =============================================== */
// 
//	정 : 안철민
//	부 :
//
/* =============================================== */
#include "Composite.h"

BEGIN(Client)

// 마법임.
// 마법은 총 7가지로 구분됨.
// 1. 제어 : 상대에게 노란 방어막이 있다면, 제어 마법을 맞히면 제거된다. 전부 상태이상이 핵심이고 다른 마법과 연계해서 즉사에 가까운 피해를 줄 수 있다
// 2. 위력 : 상대에게 보라색 방어벽이 있다면, 위력 마법을 맞히면 해제된다. 보통 이동기라서 직접 피해는 적고 중형 이상의 적이 면역이지만 충돌피해가 좋다.
// 3. 피해 : 상대에게 빨간색 방어벽이 있다면, 피해 마법을 맞히면 해제된다.
// 4. 유용 : 유틸
// 5. 변환 : 하우징 전용 마법.
// 6. 저주 : 모든 주문이 공통적으로 캐릭터를 줌인하는 컷신을 사용 / 컷신 동안 무적이라 공격 회피로 쓸 수 있다 / 적의 보호막을 색과 무관하게 파괴한다.
// 7. 필수 : 여러가지 포함.
//
// 데미지(_float) / 마법의 구분(enum) / 버프(enum flag) / 쿨타임(_float) / 사거리(_float) / 시전 마법의 타입(enum)

class CMagic : public CComposite
{
public:
	enum BUFF_TYPE
	{
		BUFF_NONE			= 0,
		BUFF_SLOW			= 1 << 0,
		BUFF_FROZEN			= 1 << 1,
		BUFF_CHANGE			= 1 << 2,
		BUFF_RIGID			= 1 << 3,
		BUFF_WEAPONLOSS		= 1 << 4,
		BUFF_FIRE			= 1 << 5,
		BUFF_INVINCIBLE		= 1 << 6,
		BUFF_LIGHT			= 1 << 7,
		BUFF_UNGRAVITY		= 1 << 8,
		BUFF_CURSE			= 1 << 9,
		BUFF_SEE			= 1 << 10,
		BUFF_SHILED			= 1 << 11,
		BUFF_STUN			= 1 << 12,
		BUFF_LOCK			= 1 << 13,
		BUFF_GRAB			= 1 << 14,
		BUFF_CONTROL		= 1 << 15,
		BUFF_END			= 1 << 16
	};

	enum MAGIC_GROUP { MG_CONTROL, MG_POWER, MG_DAMAGE, MG_UTILITY, MG_CHANGE, MG_CURSE, MG_ESSENTIAL, MG_END };
	enum MAGIC_TYPE { MT_NOTHING, MT_YELLOW, MT_PURPLE, MT_RED, MT_ALL, MT_END };

	typedef struct MagicDesc
	{
		_tchar			wszMagicName[MAX_PATH] = {};
		MAGIC_GROUP		eMagicGroup = { MG_END };
		MAGIC_TYPE		eMagicType = { MT_END };
		BUFF_TYPE		eBuffType = { BUFF_NONE };
		_float			fCoolTime = { 0 };
		_float			fDamage = { 0 };
		_float			fDistance = { 0 };
	}MAGICDESC;

protected:
	explicit CMagic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMagic(const CMagic& rhs);
	virtual ~CMagic() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	virtual _bool Action_Magic(class CTransform* pTarget);
	virtual HRESULT Add_ActionFunc(function<void()> func);

protected:
	// 마법의 이름
	_tchar						m_wszMagicName[MAX_PATH] = {};
	// 어디 그룹에 속한 마법인지?
	MAGIC_GROUP					m_eMagicGroup = { MG_END };
	// 어떤 색의 보호막을 공격하는 마법인지?
	MAGIC_TYPE					m_eMagicType = { MT_END };
	// 마법이 입히는 버프/디버프는 뭔지?
	BUFF_TYPE					m_eBuffType = { BUFF_NONE };
	//발동시 적용시켜줄 쿨타임
	_float						m_fInitCoolTime = { 0 };
	_float						m_fCurrentCoolTime = { 0 };
	//데미지
	_float						m_fDamage = { 0 };
	//마법의 사거리
	_float						m_fDistance = { 0 };
	//마법 사용시 같이 불러주고싶은 외부 함수
	vector<function<void()>>	m_ActionVec;

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
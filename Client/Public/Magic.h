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
		BUFF_NONE			= 0,		// 아무 디버프 없음.( 평타 )
		BUFF_SLOW			= 1 << 0,	// 느려지게 만듦
		BUFF_FROZEN			= 1 << 1,	// 얼어붙게 만듦
		BUFF_CHANGE			= 1 << 2,	// 외형을 변화시킴
		BUFF_RIGID			= 1 << 3,	// 물리적 효과를 줌
		BUFF_WEAPONLOSS		= 1 << 4,	// 무기를 날려버림
		BUFF_FIRE			= 1 << 5,	// 화상 디버프
		BUFF_INVINCIBLE		= 1 << 6,	// 투명화 버프
		BUFF_LIGHT			= 1 << 7,	// 빛을 생성함.
		BUFF_UNGRAVITY		= 1 << 8,	// 중력을 없앰
		BUFF_CURSE			= 1 << 9,	// 저주를 걺
		BUFF_SEE			= 1 << 10,	// 상호작용 오브젝트를 보여줌
		BUFF_SHILED			= 1 << 11,	// 쉴드
		BUFF_STUN			= 1 << 12,	// 기절시킴
		BUFF_LOCK			= 1 << 13,	// 자물쇠 따기 마법
		BUFF_GRAB			= 1 << 14,	// 움켜쥠
		BUFF_CONTROL		= 1 << 15,	// 
		BUFF_END			= 1 << 16
	};

	// 마법의 구분( 어떤 파티션에 분류되는 친구인가? )
	enum MAGIC_GROUP { MG_CONTROL, MG_POWER, MG_DAMAGE, MG_UTILITY, MG_CHANGE, MG_CURSE, MG_ESSENTIAL, MG_END };
	
	// 마법이 영향을 줄 상대 실드의 색
	enum MAGIC_TYPE { MT_NOTHING, MT_YELLOW, MT_PURPLE, MT_RED, MT_ALL, MT_END };

	typedef struct MagicDesc
	{
		MAGIC_GROUP		eMagicGroup = { MG_END };
		MAGIC_TYPE		eMagicType = { MT_END };
		BUFF_TYPE		eBuffType = { BUFF_NONE };
		SPELL			eMagicTag = { SPELL_END };
		_float			fCoolTime = { 0 };
		_float			fDamage = { 0 };
		//마법의 발동 사거리
		_float			fCastDistance = { 0 };
		//투사체의 사거리.
		_float			fBallDistance = { 0 };
		_float			fLifeTime = { 1.0f };
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
	//목표와 시작 위치를 설정해줌. 지팡이에서 파티클 재생을 해주기위해 지팡이도 던져줌.
	virtual _bool Magic_Cast(class CTransform* pTarget,class CWeapon_Player_Wand* pWeapon);
	virtual HRESULT Add_ActionFunc(function<void()> func);

protected:
	MAGIC_GROUP					m_eMagicGroup = { MG_END };
	// 어떤 색의 보호막을 공격하는 마법인지?
	MAGIC_TYPE					m_eMagicType = { MT_END };
	// 마법이 입히는 버프/디버프는 뭔지?
	BUFF_TYPE					m_eBuffType = { BUFF_NONE };
	// 풀 혹은 UI에 보내주기 위한 내가 어떤 마법인지.
	SPELL						m_eMagicTag = { SPELL_END };
	//발동시 적용시켜줄 쿨타임
	_float						m_fInitCoolTime = { 0 };
	_float						m_fCurrentCoolTime = { 0 };
	//데미지
	_float						m_fDamage = { 0 };
	// 마법의 사거리
	_float						m_fCastDistance = { 0 };
	// 투사체의 사거리
	_float						m_fBallDistance = { 0 };
	//마법의 생명주기
	_float						m_fLifeTime = { 1.0f };
	//마법 사용시 같이 불러주고싶은 외부 함수
	vector<function<void()>>	m_ActionVec;

private:
	//프로토타입 생성을 위한 enum type 별 이름 지정.
	_tchar m_szTagArray[SPELL_END][MAX_PATH] = {
		{TEXT("Accio")} , {TEXT("Alohomora")} , {TEXT("Alteration")} , {TEXT("Arrestomomentum")} , {TEXT("Avadakedavra")} , {TEXT("Bombarda")} ,
		{TEXT("Confringo")} , {TEXT("Conjuration")} , {TEXT("Crucio")} , {TEXT("Depulso")} , {TEXT("Descendo")} , {TEXT("Diffindo")} ,
		{TEXT("Disillusionment")} , {TEXT("Evaesco")} , {TEXT("Expelliarmus")} , {TEXT("Flipendo")} , {TEXT("Glacius")} , {TEXT("Imperio")} ,
		{TEXT("Ncendio")} , {TEXT("Levioso")} , {TEXT("Lumos")} , {TEXT("Protego")} , {TEXT("Reparo")} , {TEXT("Revelio")} ,
		{TEXT("Transformation")} , {TEXT("Transformationoverland")} , {TEXT("Wingardiumleviosa")} , {TEXT("BasicCast")} , {TEXT("Stupefy")} , {TEXT("Petrificustotalus")} ,
		{TEXT("MagicThrow") }, {TEXT("Finisher")} };

protected:
	HRESULT Add_Component();

public:
	static CMagic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CMagic* Clone(void* pArg) override;
	virtual void Free() override;
};

END
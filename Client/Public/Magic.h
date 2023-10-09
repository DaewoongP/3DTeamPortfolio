#pragma once
/* =============================================== */
// 
//	정 : 안철민
//	부 :
//
/* =============================================== */
#include "Composite.h"

BEGIN(Engine)
class CGameObject;
END

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
		//추적 옵션
		_bool			isChase = { false };
		//데미지
		_int			iDamage = { 0 };
		//쿨타임	
		_float			fInitCoolTime = { 1.f };
		//속도
		_float			fLifeTime = { 1.0f };
	}MAGICDESC;


public:
	_bool IsCoolOn() { return (m_fCurrentCoolTime <= 0); }

	//겟셋 비추
	_float Get_CoolTime() { return m_fCurrentCoolTime; }
	void Set_CoolTime(_float value) { m_fCurrentCoolTime = value; }
	_float Get_CoolSpeedTime() { return m_fCoolSpeed; }
	void Set_CoolSpeedTime(_float value) { m_fCoolSpeed = value; }
	_float Get_CoolMultipleTimer() { return m_fCoolMultipleTimer; }
	void Set_CoolMultipleTimer(_float value) { m_fCoolMultipleTimer = value; }

	_float Get_CoolTimeRatio() { return m_fCurrentCoolTime / m_MagicDesc.fInitCoolTime; }

	//스킬 가속
	void Set_SkillCoolMultiple(_float fTime, _float fValue) 
	{
		m_fCoolSpeed = fValue;
		m_fCoolMultipleTimer = fTime;
	}

protected:
	explicit CMagic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMagic(const CMagic& rhs);
	virtual ~CMagic() = default;

public:


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	//구조체 재설정부임.
	virtual HRESULT ResetMagicDesc(MAGICDESC SkillDesc);

public:
	//목표와 시작 위치를 설정해줌. 지팡이에서 파티클 재생을 해주기위해 지팡이도 던져줌.
	virtual class CMagicBall* Magic_Cast(const CGameObject* pTarget, const CGameObject* pWeapon, COLLISIONFLAG eCollisionFlag,_bool PowerUp);

protected:
	MAGICDESC					m_MagicDesc = {};
	//사용 후 적용시켜줄 쿨타임
	_float						m_fCurrentCoolTime = { 0.f };
	//쿨 배속
	_float						m_fCoolSpeed = { 1.0f };
	//쿨 배속 적용 시간
	_float						m_fCoolMultipleTimer = { 0.f };

private:
	//프로토타입 생성을 위한 enum type 별 이름 지정.
	_tchar m_szTagArray[SPELL_END][MAX_PATH] = {
		{TEXT("Accio")} , {TEXT("Alohomora")} , {TEXT("Alteration")} , {TEXT("Arrestomomentum")} , {TEXT("Avadakedavra")} , {TEXT("Bombarda")} ,
		{TEXT("Confringo")} , {TEXT("Conjuration")} , {TEXT("Crucio")} , {TEXT("Depulso")} , {TEXT("Descendo")} , {TEXT("Diffindo")} ,
		{TEXT("Disillusionment")} , {TEXT("Evaesco")} , {TEXT("Expelliarmus")} , {TEXT("Flipendo")} , {TEXT("Glacius")} , {TEXT("Imperio")} ,
		{TEXT("Ncendio")} , {TEXT("Levioso")} , {TEXT("Lumos")} , {TEXT("Protego")} , {TEXT("Reparo")} , {TEXT("Revelio")} ,
		{TEXT("Transformation")} , {TEXT("Transformationoverland")}, {TEXT("Wingardiumleviosa")} , {TEXT("BasicCast")} , {TEXT("Stupefy")} , {TEXT("Petrificustotalus")} ,
		{TEXT("MagicThrow") }, {TEXT("Finisher")} };

public:
	static CMagic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CMagic* Clone(void* pArg) override;
	virtual void Free() override;
};

END
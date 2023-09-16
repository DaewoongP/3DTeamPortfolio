#pragma once
/* =============================================== */
// 
//	�� : ��ö��
//	�� :
//
/* =============================================== */
#include "Composite.h"

BEGIN(Client)

// ������.
// ������ �� 7������ ���е�.
// 1. ���� : ��뿡�� ��� ���� �ִٸ�, ���� ������ ������ ���ŵȴ�. ���� �����̻��� �ٽ��̰� �ٸ� ������ �����ؼ� ��翡 ����� ���ظ� �� �� �ִ�
// 2. ���� : ��뿡�� ����� ���� �ִٸ�, ���� ������ ������ �����ȴ�. ���� �̵���� ���� ���ش� ���� ���� �̻��� ���� �鿪������ �浹���ذ� ����.
// 3. ���� : ��뿡�� ������ ���� �ִٸ�, ���� ������ ������ �����ȴ�.
// 4. ���� : ��ƿ
// 5. ��ȯ : �Ͽ�¡ ���� ����.
// 6. ���� : ��� �ֹ��� ���������� ĳ���͸� �����ϴ� �ƽ��� ��� / �ƽ� ���� �����̶� ���� ȸ�Ƿ� �� �� �ִ� / ���� ��ȣ���� ���� �����ϰ� �ı��Ѵ�.
// 7. �ʼ� : �������� ����.
//
// ������(_float) / ������ ����(enum) / ����(enum flag) / ��Ÿ��(_float) / ��Ÿ�(_float) / ���� ������ Ÿ��(enum)

class CMagic : public CComposite
{
public:
	enum BUFF_TYPE
	{
		BUFF_NONE			= 0,		// �ƹ� ����� ����.( ��Ÿ )
		BUFF_SLOW			= 1 << 0,	// �������� ����
		BUFF_FROZEN			= 1 << 1,	// ���ٰ� ����
		BUFF_CHANGE			= 1 << 2,	// ������ ��ȭ��Ŵ
		BUFF_RIGID			= 1 << 3,	// ������ ȿ���� ��
		BUFF_WEAPONLOSS		= 1 << 4,	// ���⸦ ��������
		BUFF_FIRE			= 1 << 5,	// ȭ�� �����
		BUFF_INVINCIBLE		= 1 << 6,	// ����ȭ ����
		BUFF_LIGHT			= 1 << 7,	// ���� ������.
		BUFF_UNGRAVITY		= 1 << 8,	// �߷��� ����
		BUFF_CURSE			= 1 << 9,	// ���ָ� ��
		BUFF_SEE			= 1 << 10,	// ��ȣ�ۿ� ������Ʈ�� ������
		BUFF_SHILED			= 1 << 11,	// ����
		BUFF_STUN			= 1 << 12,	// ������Ŵ
		BUFF_LOCK			= 1 << 13,	// �ڹ��� ���� ����
		BUFF_GRAB			= 1 << 14,	// ������
		BUFF_CONTROL		= 1 << 15,	// 
		BUFF_END			= 1 << 16
	};

	// ������ ����( � ��Ƽ�ǿ� �з��Ǵ� ģ���ΰ�? )
	enum MAGIC_GROUP { MG_CONTROL, MG_POWER, MG_DAMAGE, MG_UTILITY, MG_CHANGE, MG_CURSE, MG_ESSENTIAL, MG_END };
	
	// ������ ������ �� ��� �ǵ��� ��
	enum MAGIC_TYPE { MT_NOTHING, MT_YELLOW, MT_PURPLE, MT_RED, MT_ALL, MT_END };

	typedef struct MagicDesc
	{
		MAGIC_GROUP		eMagicGroup = { MG_END };
		MAGIC_TYPE		eMagicType = { MT_END };
		BUFF_TYPE		eBuffType = { BUFF_NONE };
		SPELL			eMagicTag = { SPELL_END };
		_float			fCoolTime = { 0 };
		_float			fDamage = { 0 };
		//������ �ߵ� ��Ÿ�
		_float			fCastDistance = { 0 };
		//����ü�� ��Ÿ�.
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
	//��ǥ�� ���� ��ġ�� ��������. �����̿��� ��ƼŬ ����� ���ֱ����� �����̵� ������.
	virtual _bool Magic_Cast(class CTransform* pTarget,class CWeapon_Player_Wand* pWeapon);
	virtual HRESULT Add_ActionFunc(function<void()> func);

protected:
	MAGIC_GROUP					m_eMagicGroup = { MG_END };
	// � ���� ��ȣ���� �����ϴ� ��������?
	MAGIC_TYPE					m_eMagicType = { MT_END };
	// ������ ������ ����/������� ����?
	BUFF_TYPE					m_eBuffType = { BUFF_NONE };
	// Ǯ Ȥ�� UI�� �����ֱ� ���� ���� � ��������.
	SPELL						m_eMagicTag = { SPELL_END };
	//�ߵ��� ��������� ��Ÿ��
	_float						m_fInitCoolTime = { 0 };
	_float						m_fCurrentCoolTime = { 0 };
	//������
	_float						m_fDamage = { 0 };
	// ������ ��Ÿ�
	_float						m_fCastDistance = { 0 };
	// ����ü�� ��Ÿ�
	_float						m_fBallDistance = { 0 };
	//������ �����ֱ�
	_float						m_fLifeTime = { 1.0f };
	//���� ���� ���� �ҷ��ְ���� �ܺ� �Լ�
	vector<function<void()>>	m_ActionVec;

private:
	//������Ÿ�� ������ ���� enum type �� �̸� ����.
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
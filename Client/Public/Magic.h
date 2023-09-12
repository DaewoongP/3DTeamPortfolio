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
	// ������ �̸�
	_tchar						m_wszMagicName[MAX_PATH] = {};
	// ��� �׷쿡 ���� ��������?
	MAGIC_GROUP					m_eMagicGroup = { MG_END };
	// � ���� ��ȣ���� �����ϴ� ��������?
	MAGIC_TYPE					m_eMagicType = { MT_END };
	// ������ ������ ����/������� ����?
	BUFF_TYPE					m_eBuffType = { BUFF_NONE };
	//�ߵ��� ��������� ��Ÿ��
	_float						m_fInitCoolTime = { 0 };
	_float						m_fCurrentCoolTime = { 0 };
	//������
	_float						m_fDamage = { 0 };
	//������ ��Ÿ�
	_float						m_fDistance = { 0 };
	//���� ���� ���� �ҷ��ְ���� �ܺ� �Լ�
	vector<function<void()>>	m_ActionVec;

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
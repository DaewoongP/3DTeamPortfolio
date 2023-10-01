#pragma once
/* =============================================== */
// 
//	�� : ��ö��
//	�� :
//
/* =============================================== */
#include "Composite.h"

BEGIN(Engine)
class CGameObject;
END

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
		//������
		_int			iDamage = { 0 };
		//��Ÿ��	
		_float			fInitCoolTime = { 1.f };
		//�ӵ�
		_float			fLifeTime = { 1.0f };
	}MAGICDESC;

protected:
	explicit CMagic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMagic(const CMagic& rhs);
	virtual ~CMagic() = default;

public:
	void Set_PowerUp(_bool isPowerUp) { m_isPowerUp; }


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	//����ü �缳������.
	virtual HRESULT ResetMagicDesc(MAGICDESC SkillDesc);

public:
	//��ǥ�� ���� ��ġ�� ��������. �����̿��� ��ƼŬ ����� ���ֱ����� �����̵� ������.
	virtual class CMagicBall* Magic_Cast(const CGameObject* pTarget, const CGameObject* pWeapon, COLLISIONFLAG eCollisionFlag);

protected:
	MAGICDESC					m_MagicDesc = {};
	//��� �� ��������� ��Ÿ��
	_float						m_fCurrentCoolTime = { 0 };
	_bool						m_isPowerUp = {false};
	
private:
	//������Ÿ�� ������ ���� enum type �� �̸� ����.
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
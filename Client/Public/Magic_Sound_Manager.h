#pragma once
/* =============================================== */
// 
//	�� : ��ö��
//	�� :
//
/* =============================================== */
#include "Composite.h"
#include "Client_Defines.h"

//������Ʈ �� ������ ��簡 �޶����.
//�̹� ������Ʈ�� �ִϸ��̼ǵ� �ٸ��� ������ �� �ο��س���.
//�ϳ��ϳ� �߰��ϴ°ź��� �������� �Ŵ����� ����� �ο��ϴ°� �� ���غ�����.
//������ �ʿ� ���� ������ ó�� ����Ű� ����������.

BEGIN(Client)

class CMagic_Sound_Manager : public CComposite
{
public:
	typedef enum OwnerType
	{
		OWNER_PLAYER,
		OWNER_BLACKMAGIC_A, 
		OWNER_BLACKMAGIC_B,
		OWNER_BLACKMAGIC_C, 
		OWNER_BLACKMAGIC_D,
		OWNER_FIG,
		OWENR_PENSIVE,
		OWNER_DRAGON,
		OWNER_END
	}OWNERTYPE;

protected:
	explicit CMagic_Sound_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMagic_Sound_Manager() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	void	Spell_Magic(OWNERTYPE eOwner, SPELL eSpell, _float fVolum);

private:
	vector<wstring>		m_szSoundTag[OWNER_END][SPELL_END] = {};

public:
	static CMagic_Sound_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CMagic_Sound_Manager* Clone(void* pArg) override;
	virtual void Free() override;
};

END
#pragma once
/* =============================================== */
// 
//	정 : 안철민
//	부 :
//
/* =============================================== */
#include "Composite.h"
#include "Client_Defines.h"

//오브젝트 별 나오는 대사가 달라야함.
//이미 오브젝트에 애니메이션도 다르고 마법을 다 부여해놓음.
//하나하나 추가하는거보다 마법사운드 매니져를 만들어 부여하는게 더 편해보였음.
//복사할 필요 없이 랜더러 처럼 만들거고 돌려쓸거임.

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
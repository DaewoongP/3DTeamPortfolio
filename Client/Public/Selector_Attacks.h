#pragma once

/* =============================================== */
//	[CSelector_Attacks]
//	: ���� ������ ���� �����ϰ� �����װ� ���� �׷α� �ִϸ��̼� ���� 
//	: �����ϴ� Ŭ���� 
// 
//	�� : �ּ�ȯ
//	�� :
//
/* =============================================== */

#include "Selector.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CSelector_Attacks final : public CSelector
{
private:
	explicit CSelector_Attacks(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSelector_Attacks(const CSelector_Attacks& rhs);
	virtual ~CSelector_Attacks() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Tick(const _float& fTimeDelta);

public:
	/* �� ������ ���� ��ų ����� ������ �� */
	HRESULT Assemble_Childs(CBehavior* pBehavior);

public:
	static CSelector_Attacks* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSelector_Attacks* Clone(void* pArg) override;
	virtual void Free() override;
};

END
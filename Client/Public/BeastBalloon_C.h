#pragma once
#include "Balloon.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CBeastBalloon_C final : public CBalloon
{
private:
	explicit CBeastBalloon_C(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBeastBalloon_C(const CBeastBalloon_C& rhs);
	virtual ~CBeastBalloon_C() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture* m_pEmissiveTexture = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Add_Components_Level(_uint iCurrentLevelIndex);

public:
	static CBeastBalloon_C* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
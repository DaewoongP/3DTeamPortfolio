#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CMesh;
class CModel;
class CShader;
class CRenderer;
END

BEGIN(Client)

class CGuide_Book final : public CGameObject
{
	enum BOOK_ANIMATION {
		BK_OPEN, BK_IDLE, BK_RIGHT, 
		BK_LEFT, BK_FAST_OPEN, BK_END
	};

private:
	explicit CGuide_Book(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CGuide_Book(const CGuide_Book& rhs);
	virtual ~CGuide_Book() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShader = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CModel* m_pModel = { nullptr };

private:
	BOOK_ANIMATION  m_CurAnim = { BK_END };
	BOOK_ANIMATION  m_PreAnim = { m_CurAnim }; // 이전 애니메이션 인덱스

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CGuide_Book* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
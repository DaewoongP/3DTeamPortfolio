#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)

class CShader;
class CRenderer;
class CCollider;
class CCustomModel;

END

BEGIN(Client)
class CPlayer_Camera;
class CWeapon_Player_Wand;
END

BEGIN(Client)
class CPlayer final : public CGameObject
{
private:
	explicit CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLISIONDESC CollisionDesc) override;
	virtual void OnCollisionStay(COLLISIONDESC CollisionDesc) override;
	virtual void OnCollisionExit(COLLISIONDESC CollisionDesc) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth() override;

private:
	CShader*		m_pShader = { nullptr };
	CRenderer*		m_pRenderer = { nullptr };
	CCustomModel*	m_pCustomModel = { nullptr };
	
private:
	CPlayer_Camera* m_pPlayer_Camera = { nullptr };

private:
	//카메라룩과 플레이어룩의 차이 각을 담기위한 변수(음수일 경우 오른쪽, 양수일 경우 왼쪽)
	_float m_fLookAngle{};

	
	/* 마법을 위한 공간 */
	class CMagic*	m_pMagic = { nullptr };
	CWeapon_Player_Wand*	m_pWeapon = { nullptr };
	
#ifdef _DEBUG

	CCollider*		m_pTestCollider = { nullptr };

#endif // _DEBUG

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

private:
	void Key_Input(_float fTimeDelta);

private:
	HRESULT Ready_MeshParts();
	HRESULT Ready_Caemra();

public:
	// 마법에 함수가 잘 들어가나 테스트용도입니다.
	// 추후에 마법 사용시 지팡이가 빛나게 하는 파티클 action을 여기에 넣어주면 될거같음.
	void MagicTestTextOutput();

#ifdef _DEBUG
private:
	HRESULT Ready_Test_Collider();
	void Tick_ImGui();
#endif // _DEBUG

private:
	//카메라와 플레이어의 각을 검사해서 저장한다.
	void UpdateLookAngle();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
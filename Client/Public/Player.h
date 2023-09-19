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
class CStateContext;
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
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionStay(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionExit(COLLEVENTDESC CollisionEventDesc) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth() override;

private:
	CShader*		m_pShader = { nullptr };
	CRenderer*		m_pRenderer = { nullptr };
	CCustomModel*	m_pCustomModel = { nullptr };
	CRigidBody*		m_pRigidBody = { nullptr };

private:
	CPlayer_Camera* m_pPlayer_Camera = { nullptr };

private:
	//카메라룩과 플레이어룩의 차이 각을 담기위한 변수(음수일 경우 오른쪽, 양수일 경우 왼쪽)
	_float m_fLookAngle{};
	//방향키 입력이 들어왔는지 확인하는 변수
	_bool m_isDirectionKeyPressed { false };

	CStateContext* m_pStateContext = { nullptr };
	
	//평타, 실드가 이미 탑재된 마법 슬롯 
	class CMagicSlot*	m_pMagicSlot = { nullptr };
	CWeapon_Player_Wand*	m_pWeapon = { nullptr };


private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT Add_Magic();

private:
	void Key_Input(_float fTimeDelta);

private:
	HRESULT Ready_MeshParts();
	HRESULT Ready_Camera();

public:
	// 마법에 함수가 잘 들어가나 테스트용도입니다.
	// 추후에 마법 사용시 지팡이가 빛나게 하는 파티클 action을 여기에 넣어주면 될거같음.
	void MagicTestTextOutput();

#ifdef _DEBUG
private:
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
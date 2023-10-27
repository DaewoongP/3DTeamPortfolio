#include "MapObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CMesh;
class CModel;
class CShader;
class CRenderer;
class CParticleSystem;
class CLight;
END

BEGIN(Client)

class CFireWorks : public CMapObject
{
public:
	typedef struct tagMapObjectDesc
	{
		_float4x4 WorldMatrix; // ���� ���
		_uint iTagLen = { 0 }; // ���ڿ� ����
		_tchar wszTag[MAX_PATH] = TEXT(""); // ������Ʈ ����(�� ������Ʈ �̸�)
	}MAPOBJECTDESC;

protected:
	explicit CFireWorks(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CFireWorks(const CFireWorks& rhs);
	virtual ~CFireWorks() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShader = { nullptr };
	CShader* m_pShadowShader = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CModel* m_pModel = { nullptr };
	vector<CParticleSystem*> m_vecParticle;
private:
	// ����ü �ø��� ���� Bounding Box�� ���� �ϱ����� �ּ�, �ִ� ����
	_float3			m_vMinPoint, m_vMaxPoint, m_vCenterPoint;
	_float			m_fRadius = { 0.f };
	MAPOBJECTDESC	m_ObjectDesc;

	_float4			m_vEmissive;

private:
	HRESULT Add_Components();
	

public:
	static CFireWorks* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END
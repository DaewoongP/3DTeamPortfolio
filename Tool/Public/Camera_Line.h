#pragma once
#ifdef _DEBUG
#include "GameObject.h"
#include "Tool_Defines.h"

BEGIN(Engine)

class CShader;
class CRenderer;
class CRenderer;
class CVIBuffer_Line;

END

BEGIN(Tool)

class CCamera_Line final : public CGameObject
{
public:

private:
	explicit CCamera_Line(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCamera_Line(const CCamera_Line& rhs);
	virtual ~CCamera_Line() = default;

public:

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:

	HRESULT LineBufferInitialize(void *_pArg);
	void LineBufferClear();


	
	void Set_Color(_float4 _vColor) { m_vColor = _vColor; };



private:


private:
	//먼저 준비
	CShader* m_pShader = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	//사용이 필요할때만 준비
	CVIBuffer_Line* m_pLineBuffer = { nullptr };

	//색상
	_float4 m_vColor{ _float4(1.0f, 1.0f, 1.0f, 1.0f) };
	
private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CCamera_Line* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);

	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free();
};

END
#endif // _DEBUG
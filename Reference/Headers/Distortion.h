#pragma once
#include"Engine_Defines.h"
#include "Component.h"
BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;

class CDistortion :
    public CComponent
{
private:
	explicit CDistortion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CDistortion() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pTargetTag);
	virtual HRESULT Render();
	void Set_Textures(_tchar* pShaderPass, CTexture* pOriTexture, CTexture* pNoisetexture, CTexture* pAlphaTexture);
private:
	CShader* m_pShader = { nullptr };
	CVIBuffer_Rect* m_pBuffer = { nullptr };
	_float4x4			m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;
	_tchar				m_szTargetTag[MAX_PATH] = TEXT("");
	_tchar				ShaderPass[MAX_PATH] = TEXT("");
	_float m_fFrameTime = { 0.f };


private:
	HRESULT Add_Components();

private:
	class CTexture* m_pOriginTexture = { nullptr };//디스토션을먹일 원본텍스쳐
	class CTexture* m_pNoiseTexture = { nullptr }; //디스토션먹일 노이즈 텍스쳐
	class CTexture* m_pAlphaTexture = { nullptr };// 디스토션먹인 후 알파값으로 잘라낼 텍스쳐
	
	

public:
	static CDistortion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTargetTag);
	virtual CComponent* Clone(void* pArg) { return nullptr; }
	virtual void Free() override; 
};

END

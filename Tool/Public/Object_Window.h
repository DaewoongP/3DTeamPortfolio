#pragma once
#include "ImWindow.h"

BEGIN(Tool)

class CMapDummy;

class CObject_Window final : public CImWindow
{
typedef struct QuadTerrainDesc {
	// �� ����ü�� ������ 4�����ؼ� ������ ���� ���콺 ��ġ�� ��������
	// �ش��ϴ� ������ �����鸸 ��ȯ�ϰ� ����� ���� ����
	_uint iOriginStart = { 0 }; // ���� ����
	_uint iOriginEnd = { 0 };	// ���� ��
	_uint iStart = { 0 };		// ����� ����
	_uint iEnd = { 0 };			// ���ϵ� ��
	_uint iCenter = { 0 };		// ����� ������ �߾Ӱ�
}QUADTERRAIN;

private:
	explicit CObject_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CObject_Window() = default;

public:
	virtual HRESULT Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float3 Find_PickingPos(); // ������ ��ŷ ��ġ�� ã�� �Լ�
	_float3 Find_PickingPosTest(); // ����ȭ �׽�Ʈ �Լ�
	HRESULT Create_Dummy(); // ���� 1�� Dummy ����
	void InitializeQuadTerrain(); // ���� 1�� QUADTERRAIN�� �ʱ�ȭ�ϴ� �Լ�

private:
	_float m_fDist = { FLT_MAX }; // Find_PickingPos���� ����ϴ� �Ÿ� �񱳸� ���� ����

	/* ���� 4���� ����ȭ ���� ����*/
	_bool m_isInitQuadTerrain = { true }; // ���� 1�� QUADTERRAIN�� �ʱ�ȭ
	QUADTERRAIN m_TerrainXDesc;
	QUADTERRAIN m_TerrainZDesc;

	CMapDummy* m_pDummy = { nullptr }; // �����ص� Dummy�� �ּ�

public:
	static CObject_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free(void) override;
};

END
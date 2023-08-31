#include "..\Public\Calculator.h"
#include "PipeLine.h"
#include <limits>

IMPLEMENT_SINGLETON(CCalculator)

CCalculator::CCalculator()
{
}

HRESULT CCalculator::Get_MouseRay(ID3D11DeviceContext* pContext, HWND hWnd, _float4x4 PickingWorldMatrix_Inverse, _Inout_ _float4* vRayPos, _Inout_ _float4* vRayDir)
{
    D3D11_VIEWPORT ViewPort;
    UINT iNumViewPorts = 1;

    ZEROMEM(&ViewPort);
    if (nullptr == pContext)
        return E_FAIL;
    pContext->RSGetViewports(&iNumViewPorts, &ViewPort);

	POINT	pt{};
	GetCursorPos(&pt);
	ScreenToClient(hWnd, &pt);

	_float4		vMouse;
	vMouse = XMVectorSet(
		pt.x / (ViewPort.Width * 0.5f) - 1.f,
		pt.y / -(ViewPort.Height * 0.5f) + 1.f,
		0.f, 
		1.f);

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);
	
	_float4x4		ProjMatrix_Inverse;
	ProjMatrix_Inverse = *pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ);
	vMouse = XMVector3TransformCoord(vMouse, ProjMatrix_Inverse);

	_float4x4		ViewMatrix_Inverse;
	ViewMatrix_Inverse = *pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW);

	Safe_Release(pPipeLine);

	_float4 vPickPos, vPickDir;
	vPickPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	vPickDir = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	vPickDir = vMouse - vPickPos;
	vPickDir = XMVector3Normalize(vPickDir);

	vPickPos = XMVector3TransformCoord(vPickPos, ViewMatrix_Inverse);
	vPickDir = XMVector3TransformNormal(vPickDir, ViewMatrix_Inverse);

	vPickPos = XMVector3TransformCoord(vPickPos, PickingWorldMatrix_Inverse);
	vPickDir = XMVector3TransformNormal(vPickDir, PickingWorldMatrix_Inverse);

	XMStoreFloat4(vRayPos, vPickPos);
	XMStoreFloat4(vRayDir, vPickDir);

	return S_OK;
}

HRESULT CCalculator::Get_WorldMouseRay(ID3D11DeviceContext* pContext, HWND hWnd, _Inout_ _float4* vRayPos, _Inout_ _float4* vRayDir)
{
	D3D11_VIEWPORT ViewPort;
	UINT iNumViewPorts = 1;

	ZEROMEM(&ViewPort);
	if (nullptr == pContext)
		return E_FAIL;
	pContext->RSGetViewports(&iNumViewPorts, &ViewPort);

	POINT	pt{};
	GetCursorPos(&pt);
	ScreenToClient(hWnd, &pt);

	_float4		vMouse;
	vMouse = XMVectorSet(
		pt.x / (ViewPort.Width * 0.5f) - 1.f,
		pt.y / -(ViewPort.Height * 0.5f) + 1.f,
		0.f,
		1.f);

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	_float4x4		ProjMatrix_Inverse;
	ProjMatrix_Inverse = *pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ);
	vMouse = XMVector3TransformCoord(vMouse, ProjMatrix_Inverse);

	_float4x4		ViewMatrix_Inverse;
	ViewMatrix_Inverse = *pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW);

	Safe_Release(pPipeLine);

	_float4 vPickPos, vPickDir;
	vPickPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	vPickDir = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	vPickDir = vMouse - vPickPos;
	vPickDir = XMVector3Normalize(vPickDir);

	vPickPos = XMVector3TransformCoord(vPickPos, ViewMatrix_Inverse);
	vPickDir = XMVector3TransformNormal(vPickDir, ViewMatrix_Inverse);

	XMStoreFloat4(vRayPos, vPickPos);
	XMStoreFloat4(vRayDir, vPickDir);

	return S_OK;
}

_bool CCalculator::IsMouseInClient(ID3D11DeviceContext* pContext, HWND hWnd)
{
	D3D11_VIEWPORT ViewPort;
	UINT iNumViewPorts = 1;

	ZEROMEM(&ViewPort);
	if (nullptr == pContext)
		return false;
	pContext->RSGetViewports(&iNumViewPorts, &ViewPort);

	POINT	pt{};
	GetCursorPos(&pt);

	RECT rc{};
	GetWindowRect(hWnd, &rc);

	if (rc.left <= pt.x &&
		rc.right >= pt.x &&
		rc.top <= pt.y &&
		rc.bottom >= pt.y)
		return true;

	return false;
}

_int CCalculator::RandomChoose(vector<_float> Weights, _uint iChooseSize)
{
	// 입력 잘못한경우
	if (Weights.size() > iChooseSize)
		return -1;

	if (Weights.size() < iChooseSize)
	{
		_uint iRemainders = { 0 };
		_float fPercents = { 0.f };
		_float fRemainPercents = { 0.f };

		iRemainders = iChooseSize - (_uint)Weights.size();

		for (auto& Weight : Weights)
		{
			fPercents += Weight;
		}

		fRemainPercents = 1.f - fPercents;

		for (_uint i = 0; i < iRemainders; ++i)
		{
			// 남은 퍼센트를 분배해서 대입해줌.
			Weights.push_back(fRemainPercents / iRemainders);
		}
	}

	_float fRand = (rand() % 101) * 0.01f;

	for (_uint iIndex = 0; iIndex < Weights.size(); ++iIndex)
	{
		if (Weights[iIndex] >= fRand)
		{
			return iIndex;
		}

		// 마지막 인덱스 예외처리.
		if (iIndex + 1 == Weights.size() - 1)
			return iIndex + 1;
		else
		{
			Weights[iIndex + 1] += Weights[iIndex];
		}
	}

	// 오류
	return -1;
}

_float4 CCalculator::Get_RandomVectorInSphere(_float fRadius)
{
	_float fRandX = (rand() % 1001) * 0.001f;
	_float fRandY = (rand() % 1001) * 0.001f;
	_float fRandZ = (rand() % 1001) * 0.001f;

	_float3 vDir = _float3(fRandX, fRandY, fRandZ);
	XMStoreFloat3(&vDir, XMVector3Normalize(XMLoadFloat3(&vDir)) * fRadius);
	
	switch (rand() % 8)
	{
	case 0:
		// 1사분면
		break;
	case 1:
		// 2사분면
		vDir.x *= -1.f;
		break;
	case 2:
		// 3사분면
		vDir.x *= -1.f;
		vDir.y *= -1.f;
		break;
	case 3:
		// 4사분면
		vDir.y *= -1.f;
		break;
		// -z분면
	case 4:
		// 1사분면
		vDir.z *= -1.f;
		break;
	case 5:
		// 2사분면
		vDir.x *= -1.f;
		vDir.z *= -1.f;
		break;
	case 6:
		// 3사분면
		vDir.x *= -1.f;
		vDir.y *= -1.f;
		vDir.z *= -1.f;
		break;
	case 7:
		// 4사분면
		vDir.y *= -1.f;
		vDir.z *= -1.f;
		break;
	}

	return XMLoadFloat3(&vDir);
}

HRESULT CCalculator::ReadFileInDirectory(_Inout_ vector<wstring>& OutVector, const _tchar* pFilePath, const _tchar* pExt)
{
	// 디렉토리 경로를 순회할 iterator
	fs::directory_iterator iter(fs::absolute(pFilePath));

	while (iter != fs::end(iter))
	{
		// 실제 디렉토리 경로를 담고있는 변수 (iterator의 원본)
		const fs::directory_entry& entry = *iter;

		// 현재 entry 변수가 디렉토리인지 확인 후 디렉토리이면 재귀
		if (fs::is_directory(entry.path()))
		{
			if (FAILED(ReadFileInDirectory(OutVector, entry.path().c_str(), pExt)))
				return E_FAIL;
		}
		else
		{
			// 파일 확장자 체크
			if (!_wcsicmp(entry.path().extension().c_str(), pExt))
			{
#ifdef _DEBUG
				cout << entry.path() << endl;
#endif // _DEBUG
				OutVector.push_back(entry.path().wstring());
			}
		}

		iter++;
	}

	return S_OK;
}

void CCalculator::Free()
{
}

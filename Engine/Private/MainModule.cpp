#include "..\Public\MainModule.h"
#include "ParticleSystem.h"

CMainModule::CMainModule()
	: CParticleModule()
{
}

CMainModule::CMainModule(const CMainModule& _rhs)
	: CParticleModule(_rhs)
	, m_MainModuleDesc(_rhs.m_MainModuleDesc)
{
}

HRESULT CMainModule::Initialize_Prototype(CParticleSystem* pParticleSystem)
{
	__super::Initialize_Prototype();
	m_pParticleSystem = pParticleSystem;
	// ../../Resources/Effects/Particles/Particles/Fire/_pFilePath.ptc

	return S_OK;
}

void CMainModule::Tick(_float _fTimeDelta, vector<CVIBuffer_Color_Instance::COLORINSTANCE>& _ColInsts)
{
	_ColInsts.clear();
	
	for (auto& ParticleDesc : m_ParticleDescs)
	{
		CVIBuffer_Color_Instance::COLORINSTANCE colInstDesc;
		//ParticleDesc.fAge += _fTimeDelta;
		//_float4 vPos;

		//if (ParticleDesc.fAge > ParticleDesc.fLifeTime)
		//{
		//	ResetParticle(ParticleDesc);
		//}

		//// 이전프레임의 위치값을 가져옴.
		//vPos = ParticleDesc.WorldMatrix.Translation().TransCoord();


		//// 위치에 속도를 더해서 최종 위치를 정함.
		//vPos = vPos + ParticleDesc.vVelocity * _fTimeDelta;
		//memcpy(&ParticleDesc.WorldMatrix.m[3], &vPos, sizeof _float4);

		// SRT 연산
		//_float4x4 ScaleMatrix = _float4x4::MatrixScale(0.2f);
		////_Matrix RotMatrix = _Matrix::MatrixScale(0.1f);
		//_float4x4 TransMatrix = _float4x4::MatrixTranslation(vPos.xyz());
		//_float4x4 TransfomationMatrix = ScaleMatrix * TransMatrix;

		//colInstDesc.InstanceLocalMatrix = TransfomationMatrix;
		//colInstDesc.vInstanceColor = { 1.f, 1.f, 1.f, 1.f };
		//_ColInsts.push_back(colInstDesc);
	}
}

HRESULT CMainModule::SaveModule(const _tchar* pDirectoyPath)
{
	fs::path fsFilePath = pDirectoyPath;
	fsFilePath = fsFilePath / TEXT("MainModule.ptc");

	HANDLE hFile = CreateFile(fsFilePath.wstring().data()
		, GENERIC_WRITE
		, 0
		, 0
		, CREATE_ALWAYS
		, FILE_ATTRIBUTE_NORMAL
		, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;
	_ulong dwByte = 0;

	WriteFile(hFile, &m_MainModuleDesc.fDuration, sizeof _float, &dwByte, nullptr);
	WriteFile(hFile, &m_MainModuleDesc.isLooping, sizeof _bool, &dwByte, nullptr);
	WriteFile(hFile, &m_MainModuleDesc.isPrewarm, sizeof _bool, &dwByte, nullptr);
	WriteFile(hFile, &m_MainModuleDesc.fStartDelay, sizeof _float, &dwByte, nullptr);
	WriteFile(hFile, &m_MainModuleDesc.fStartLifeTime, sizeof _float, &dwByte, nullptr);
	WriteFile(hFile, &m_MainModuleDesc.fStartSpeed, sizeof _float, &dwByte, nullptr);

	CloseHandle(hFile);
}

HRESULT CMainModule::LoadModule(const _tchar* _pDirectoyPath)
{
	fs::path fsFilePath = _pDirectoyPath;
	fsFilePath = fsFilePath / TEXT("MainModule.ptc");

	HANDLE hFile = CreateFile(fsFilePath.wstring().data(),
		GENERIC_READ,
		0,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		return E_FAIL;
	}
	_ulong	dwByte = 0;

	ReadFile(hFile, &m_MainModuleDesc.fDuration, sizeof _float, &dwByte, nullptr);
	ReadFile(hFile, &m_MainModuleDesc.isLooping, sizeof _bool, &dwByte, nullptr);
	ReadFile(hFile, &m_MainModuleDesc.isPrewarm, sizeof _bool, &dwByte, nullptr);
	ReadFile(hFile, &m_MainModuleDesc.fStartDelay, sizeof _float, &dwByte, nullptr);
	ReadFile(hFile, &m_MainModuleDesc.fStartLifeTime, sizeof _float, &dwByte, nullptr);
	ReadFile(hFile, &m_MainModuleDesc.fStartSpeed, sizeof _float, &dwByte, nullptr);

	CloseHandle(hFile);
}

void CMainModule::ResetParticle(MAIN_MODULE& particleDesc)
{

}

void CMainModule::ResetAllParticle()
{
}

CMainModule* CMainModule::Create(CParticleSystem* pParticleSystem)
{
	CMainModule* pInstance = new CMainModule();

	if (FAILED(pInstance->Initialize_Prototype(pParticleSystem)))
	{
		MSG_BOX("Failed to Created CMainModule");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainModule::Free()
{
	__super::Free();
}
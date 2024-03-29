#include "stdafx.h"
#include "Terrain.h"
#include "Transform.h"
#include "ModuleMgr.h"	
#include "VIBuffer.h"


USING(MyGame)

CTerrain::CTerrain(PDIRECT3DDEVICE9 _pGraphic_Device)
	:CGameObject(_pGraphic_Device)
{
	ZeroMemory(m_PrototypeTag, sizeof(m_PrototypeTag));
}

CTerrain::CTerrain(CTerrain & _rhs)
	: CGameObject(_rhs),
	m_pTexture(_rhs.m_pTexture),
	m_iCurFrame(_rhs.m_iCurFrame),
	m_eState(_rhs.m_eState),
	m_tInfo(_rhs.m_tInfo)
{
	memcpy(m_PrototypeTag, _rhs.m_PrototypeTag, sizeof(m_PrototypeTag));
	Safe_AddRef(m_pTexture);
	Safe_AddRef(m_pVIBuffer);
	Add_Module(L"Texture_Tile", m_pTexture);
}

HRESULT CTerrain::Initialize_Prototype(TERRAIN _tData, const _tchar* _pTextureTag, SCENEID _eTextureScene, const _tchar* _pPrototypeTag, _tchar * _pFilePath)
{
	memcpy(m_PrototypeTag, _pPrototypeTag, sizeof(_tchar) * lstrlen(_pPrototypeTag));

	if (FAILED(Set_Module(_pTextureTag, _eTextureScene, (CModule**)&m_pTexture)))
		return E_FAIL;

	Set_Module(L"Transform", SCENEID::SCENE_STATIC, (CModule**)&m_pTransform);
	Set_Module(L"VIBuffer", SCENEID::SCENE_STATIC, (CModule**)&m_pVIBuffer);
	m_tInfo = _tData;
	m_pTransform->Set_Size(Vector4(TILECX, TILECY));
	return S_OK;
}

HRESULT CTerrain::Initialize()
{
	Set_Module(L"Transform", SCENEID::SCENE_STATIC, (CModule**)&m_pTransform);
	Set_Module(L"VIBuffer", SCENEID::SCENE_STATIC, (CModule**)&m_pVIBuffer);


	m_pTransform->Set_Size(Vector4(TILECX, TILECY));
	m_pTransform->Set_ColliderSize(Vector3(5.f, 5.f));
	return S_OK;
}

_int CTerrain::LateUpate(_double _timeDelta)
{

	m_pTransform->Set_Position(Vector3(m_vBasePos.x - m_iScrollX, m_vBasePos.y - m_iScrollY));
	m_pTransform->Late_Update();

	if (FAILED(m_pRenderer->Add_To_RenderGrop(this, CRenderer::RENDER_YSORT)))
		return -1;
	return 0;
}

void CTerrain::Set_InitialPos(Vector3 _vPos)
{
	if (nullptr == m_pTransform)
		return;

	m_vBasePos = _vPos;
	m_pTransform->Set_Position(_vPos);
}

HRESULT CTerrain::Render()
{
	if (FAILED(m_pVIBuffer->Set_Transform(m_pTransform->Get_Matrix())))
		return E_FAIL;

	if (FAILED(m_pTexture->Set_Texture(m_iCurFrame)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;


	return S_OK;
}


CTerrain::SAVE_DATA CTerrain::Get_SaveData()
{
	SAVE_DATA tSaveData;
	tSaveData.m_vPosition = m_pTransform->Get_Position();
	tSaveData.m_vRotation = m_pTransform->Get_Rotation();
	tSaveData.m_vSize = m_pTransform->Get_Size();
	tSaveData.m_iCurFrame = m_iCurFrame;
	ZeroMemory(&tSaveData.m_PrototypeTag, sizeof(tSaveData.m_PrototypeTag));
	memcpy(tSaveData.m_PrototypeTag, m_PrototypeTag, sizeof(_tchar) * lstrlen(m_PrototypeTag));

	return tSaveData;
}

HRESULT CTerrain::Load_Data(SAVE_DATA& _eSaveData)
{
	if (nullptr == m_pTransform)
		return E_FAIL;

	m_pTransform->Set_Position(_eSaveData.m_vPosition);
	m_vBasePos = _eSaveData.m_vPosition;
	m_pTransform->Set_Rotation(_eSaveData.m_vRotation);
	m_pTransform->Set_Size(_eSaveData.m_vSize);
	memcpy(m_PrototypeTag, _eSaveData.m_PrototypeTag, sizeof(m_PrototypeTag));
	m_iCurFrame = _eSaveData.m_iCurFrame;
	m_eState = _eSaveData.m_eState;

	m_pTransform->Late_Update();

	OnLoadData();
	return S_OK;
}




bool CTerrain::IsMovable(CTransform * _pTransform)
{
	if (nullptr == _pTransform)
		return m_tInfo.m_bMovable;

	return  m_tInfo.m_bMovable && (m_pCharacterTranform == nullptr || m_pCharacterTranform == _pTransform);
}

HRESULT CTerrain::Forward_Frame()
{

	++m_iCurFrame;

	if (FAILED(OnMoveFrame()))
		return E_FAIL;

	if (m_iCurFrame < 0)
		m_iCurFrame = 0;

	if (m_iCurFrame >= (_int)m_pTexture->Get_TextureSize())
		m_iCurFrame = (_int)m_pTexture->Get_TextureSize() - 1;

	return S_OK;
}

HRESULT CTerrain::Backward_Frame()
{
	--m_iCurFrame;

	if (FAILED(OnMoveFrame()))
		return E_FAIL;

	if (m_iCurFrame < 0)
		m_iCurFrame = 0;

	if (m_iCurFrame >= (_int)m_pTexture->Get_TextureSize())
		m_iCurFrame = (_int)m_pTexture->Get_TextureSize() - 1;

	return S_OK;
}

void CTerrain::OnCollisionEnter(CGameObject * _pOther)
{

}

void CTerrain::OnCollisionStay(CGameObject * _pOther)
{


}

void CTerrain::OnCollisionExit(CGameObject * _pOther)
{

}

void CTerrain::OnCollisionEnterTerrain(CGameObject * _pOther)
{

}

void CTerrain::OnCollisionStayTerrain(CGameObject * _pOther)
{
}

void CTerrain::OnCollisionExitTerrain(CGameObject * _pOther)
{
}

HRESULT CTerrain::OnMoveFrame()
{
	return S_OK;
}

HRESULT CTerrain::OnLoadData()
{
	return S_OK;
}





CTerrain * CTerrain::Create(PDIRECT3DDEVICE9 _pGraphic_Device, TERRAIN _tData, const _tchar* _pTextureTag, SCENEID _eTextureScene, _tchar* _pFilePath)
{
	CTerrain* pInstance = new CTerrain(_pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype(_tData, _pTextureTag, _eTextureScene, _pTextureTag, _pFilePath)))
	{
		MSG_BOX("Fail to create CTerrain");
		Safe_Release(pInstance);

	}
	return pInstance;
}

CGameObject * CTerrain::Clone(void * _param)
{
	CTerrain* pInstance = new CTerrain(*this);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Fail to clone CTile");
		Safe_Release(pInstance);

	}
	return pInstance;
}

void CTerrain::Free()
{
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);
	Safe_Release(m_pVIBuffer);
	CGameObject::Free();
}



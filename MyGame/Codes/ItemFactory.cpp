#include "stdafx.h"
#include "..\Headers\ItemFactory.h"
#include "Item.h"
#include "MeleeWeapon.h"
#include "wand.h"
#include "Ring.h"
#include "Armor.h"
#include "ObjMgr.h"
USING(MyGame)

HRESULT CItemFactory::Make_Prototpyes(PDIRECT3DDEVICE9 _pGraphic_Device)
{
	CObjMgr* pObjMgr = CObjMgr::Get_Instance();
	if (nullptr == pObjMgr)
		return E_FAIL;

	pObjMgr->Add_Prototype(L"MeleeWeapon", SCENE_STAGE, CMeleeWeapon::Create(_pGraphic_Device));
	pObjMgr->Add_Prototype(L"Wand", SCENE_STAGE, CWand::Create(_pGraphic_Device));
	pObjMgr->Add_Prototype(L"Ring", SCENE_STAGE, CRing::Create(_pGraphic_Device));
	pObjMgr->Add_Prototype(L"Armor", SCENE_STAGE, CArmor::Create(_pGraphic_Device));

	return S_OK;
}



CItem* CItemFactory::Make_Item( BASEDESC _tDesc, ITEM_ID _eID)
{
	CObjMgr* pObjMgr = CObjMgr::Get_Instance();
	if (nullptr == pObjMgr)
		return nullptr;

	CEquipment::STATEDESC tDesc;
	CEquipment::STATS tStats;

	CItem* pItem = nullptr;

	switch (_eID)
	{
	case MyGame::CItemFactory::ITEM_SHORTSWORD:
		tDesc.m_iTextureID = 4;
		tDesc.m_pDescription = L"ª�� ���̴�. �����̳� ��Ƹ��� �� �ְڴ�.";
		tDesc.m_pItemName = L"���ҵ�";
		tDesc.m_tBaseDesc = _tDesc;
		tStats.m_fAtt = 3.f;
		tDesc.m_tStats = tStats;
		pItem = (CItem*)pObjMgr->Add_GO_To_Layer(L"MeleeWeapon", SCENE_STAGE, L"Item", SCENE_STAGE, &tDesc);
		break;
	case MyGame::CItemFactory::ITEM_LONGSWORD:
		tDesc.m_iTextureID = 2;
		tDesc.m_pDescription = L"�� ���̴�. ���԰� �ϳ�����.";
		tDesc.m_pItemName = L"�ռҵ�";
		tDesc.m_tBaseDesc = _tDesc;
		tStats.m_fAtt = 5.f;
		tDesc.m_tStats = tStats;
		pItem = (CItem*)pObjMgr->Add_GO_To_Layer(L"MeleeWeapon", SCENE_STAGE, L"Item", SCENE_STAGE, &tDesc);
		break;
	case MyGame::CItemFactory::ITEM_DAGGER:
		tDesc.m_iTextureID = 3;
		tDesc.m_pDescription = L"�������̳� �ϻ쿡 ������ �� �ϴ�.";
		tDesc.m_pItemName = L"���";
		tStats.m_fAtt = 2.f;
		tDesc.m_tBaseDesc = _tDesc;
		tDesc.m_tStats = tStats;
		pItem = (CItem*)pObjMgr->Add_GO_To_Layer(L"MeleeWeapon", SCENE_STAGE, L"Item", SCENE_STAGE, &tDesc);
		break;
	case MyGame::CItemFactory::ITEM_IRONSWORD:
		tDesc.m_iTextureID = 5;
		tDesc.m_pItemName = L"ö��";
		tDesc.m_pDescription = L"������ �ܴ��� ���̴� ���̴�.";
		tDesc.m_tBaseDesc = _tDesc;
		tStats.m_fAtt = 6.f;
		tDesc.m_tStats = tStats;
		pItem = (CItem*)pObjMgr->Add_GO_To_Layer(L"MeleeWeapon", SCENE_STAGE, L"Item", SCENE_STAGE, &tDesc);
		break;
	case MyGame::CItemFactory::ITEM_BATTLEAXE:
		tDesc.m_iTextureID = 7;
		tDesc.m_pItemName = L"��������";
		tDesc.m_pDescription = L"�߸����� ����ϴ� ����";
		tDesc.m_tBaseDesc = _tDesc;
		tStats.m_fAtt = 6.f;
		tDesc.m_tStats = tStats;
		pItem = (CItem*)pObjMgr->Add_GO_To_Layer(L"MeleeWeapon", SCENE_STAGE, L"Item", SCENE_STAGE, &tDesc);
		break;
	case MyGame::CItemFactory::ITEM_GOLDAXE:
		tDesc.m_iTextureID = 8;
		tDesc.m_pItemName = L"Ȳ�ݸ�ġ";
		tDesc.m_pDescription = L"������ �� ��ġ��. ��� ���ڰ� �������.";
		tDesc.m_tBaseDesc = _tDesc;
		tStats.m_fAtt = 5.f;
		tDesc.m_tStats = tStats;
		pItem = (CItem*)pObjMgr->Add_GO_To_Layer(L"MeleeWeapon", SCENE_STAGE, L"Item", SCENE_STAGE, &tDesc);
		break;
	case MyGame::CItemFactory::ITEM_SPEAR:
		tDesc.m_iTextureID = 9;
		tDesc.m_pItemName = L"â";
		tDesc.m_pDescription = L"����� â�̴�. �� ������ ���� ���� �� ����.";
		tDesc.m_tBaseDesc = _tDesc;
		tStats.m_fAtt = 5.f;
		tDesc.m_tStats = tStats;
		pItem = (CItem*)pObjMgr->Add_GO_To_Layer(L"MeleeWeapon", SCENE_STAGE, L"Item", SCENE_STAGE, &tDesc);
		break;
	case MyGame::CItemFactory::ITEM_TRIDENT:
		tDesc.m_iTextureID = 10;
		tDesc.m_pItemName = L"����â";
		tDesc.m_pDescription = L"�ϳ� ���ִ�.";
		tDesc.m_tBaseDesc = _tDesc;
		tStats.m_fAtt = 7.f;
		tDesc.m_tStats = tStats;
		pItem = (CItem*)pObjMgr->Add_GO_To_Layer(L"MeleeWeapon", SCENE_STAGE, L"Item", SCENE_STAGE, &tDesc);
		break;
	case MyGame::CItemFactory::ITEM_MACE:
		tDesc.m_iTextureID = 11;
		tDesc.m_pItemName = L"���̽�";
		tDesc.m_pDescription = L"�׳� ��� ������ �ȴ�.";
		tDesc.m_tBaseDesc = _tDesc;
		tStats.m_fAtt = 4.f;
		tDesc.m_tStats = tStats;
		pItem = (CItem*)pObjMgr->Add_GO_To_Layer(L"MeleeWeapon", SCENE_STAGE, L"Item", SCENE_STAGE, &tDesc);
		break;
	case MyGame::CItemFactory::ITEM_LETHERARMOR:
		tDesc.m_iTextureID = 2;
		tDesc.m_pItemName = L"���װ���";
		tDesc.m_pDescription = L"� ������ �����ϱ�?";
		tDesc.m_tBaseDesc = _tDesc;
		tStats.m_fArmor = 2.f;
		tDesc.m_tStats = tStats;
		pItem = (CItem*)pObjMgr->Add_GO_To_Layer(L"Armor", SCENE_STAGE, L"Item", SCENE_STAGE, &tDesc);
		break;
	case MyGame::CItemFactory::ITEM_CHAINARMOR:
		tDesc.m_iTextureID = 6;
		tDesc.m_pItemName = L"�罽����";
		tDesc.m_pDescription = L"�����϶����� �Ҹ�������.";
		tDesc.m_tBaseDesc = _tDesc;
		tStats.m_fArmor = 3.f;
		tDesc.m_tStats = tStats;
		pItem = (CItem*)pObjMgr->Add_GO_To_Layer(L"Armor", SCENE_STAGE, L"Item", SCENE_STAGE, &tDesc);
		break;
	case MyGame::CItemFactory::ITEM_PLATEARMOR:
		break;
	case MyGame::CItemFactory::ITEM_IRONARMOR:
		break;
	case MyGame::CItemFactory::ITEM_DRAGONARMOR:
		break;
	case MyGame::CItemFactory::ITEM_ARROW:
		break;
	case MyGame::CItemFactory::ITEM_BOMB:
		break;
	case MyGame::CItemFactory::ITEM_BOTTLE:
		break;
	case MyGame::CItemFactory::ITEM_NUCKLE:
		break;
	case MyGame::CItemFactory::ITEM_REDRING:
		break;
	case MyGame::CItemFactory::ITEM_END:
		break;
	default:
		break;
	}

	return pItem;
}

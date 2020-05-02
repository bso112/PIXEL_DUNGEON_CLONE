#include "stdafx.h"
#include "..\Headers\World.h"
#include "Terrain.h"
#include "Transform.h"
#include "ObjMgr.h"
#include "Door.h"
#include "CollisionMgr.h"

USING(MyGame)

IMPLEMENT_SINGLETON(CWorld)

CWorld::CWorld()
{
	ZeroMemory(&m_pTerrains, sizeof(m_pTerrains));
}

HRESULT CWorld::Initialize(PDIRECT3DDEVICE9 _pGraphic_Device, SCENEID _eSceneID, _tchar* _pFilePath)
{

	m_eSceneID = _eSceneID;
	if (_pFilePath != nullptr)
	{
		//지형 프로토타입을 만든다.
		Initalize_Prototypes(_pGraphic_Device, _eSceneID);
		//월드 정보 파일을 읽어 지형을 배치한다.
		Load_World(_pFilePath, _eSceneID);
	}


	return S_OK;

}

_int CWorld::Update()
{
	//충돌처리
	CGameObject* pPlayer = CObjMgr::Get_Instance()->Get_Player(m_eSceneID);
	if (nullptr == pPlayer)
		return -1;
	Collision_Terrain(pPlayer);

	return 0;
}


HRESULT CWorld::Render()
{

	for (int i = 0; i < WORLDY; ++i)
	{
		for (int j = 0; j < WORLDX; ++j)
		{
			if (nullptr != m_pTerrains[i][j])

				m_pTerrains[i][j]->Render();
		}
	}
	return S_OK;
}

HRESULT CWorld::Set_Terrain(CTerrain * _pTerrain, POINT& _pt)
{
	if (nullptr == _pTerrain)
		return E_FAIL;

	_uint x = _pt.x / TILECX;
	_uint y = _pt.y / TILECY;


	//타일의 인덱스에 따른 타일의 좌표를 구한다.
	float fX = (TILECX >> 1) + (float)(TILECX * x);
	float fY = (TILECY >> 1) + (float)(TILECY * y);

	if (x >= WORLDX || y >= WORLDY)
		return E_FAIL;

	//만약 이미 타일이 있다면 지우기
	if (nullptr != m_pTerrains[y][x])
	{
		Safe_Release(m_pTerrains[y][x]);
		return S_OK;
	}


	//깊은복사
	CTerrain* pTerrain = (CTerrain*)_pTerrain->Clone();

	//지형의 위치를 지정한다.
	CTransform* pTransform = (CTransform*)pTerrain->Get_Module(L"Transform");
	if (nullptr != pTransform)
	{
		pTransform->Set_Position(Vector2(fX, fY, 0.f, 1.f));
		pTransform->Late_Update();
	}

	//타일을 채운다.
	m_pTerrains[y][x] = pTerrain;

	return S_OK;
}

HRESULT CWorld::Get_TerrainPos(POINT _dst, Vector3& _out)
{
	int x = _dst.x / TILECX;
	int y = _dst.y / TILECY;

	if (x >= WORLDX || y >= WORLDY)
		return E_FAIL;

	if (nullptr == m_pTerrains[y][x])
		return E_FAIL;

	CTransform* pDstTransform = (CTransform*)m_pTerrains[y][x]->Get_Module(L"Transform");
	_out = pDstTransform->Get_Position();

	return S_OK;
}


HRESULT CWorld::Get_Route(Vector3 _src, POINT _dst, vector<Vector3>& _out)
{
	_int srcX = (_int)_src.x / TILECX;
	_int srcY = (_int)_src.y / TILECY;
	_int dstX = (_int)_dst.x / TILECX;
	_int dstY = (_int)_dst.y / TILECY;

	if (dstX >= WORLDX || dstY >= WORLDY)
		return E_FAIL;
	if (srcX >= WORLDX || srcY >= WORLDY)
		return E_FAIL;

	if (nullptr == m_pTerrains[srcY][srcX] || nullptr == m_pTerrains[dstY][dstX])
		return E_FAIL;

	//에이스타
	/*
	Gcost // distance from staring node
	Hcost // distance from end node
	Fcost // Gcost + Hcost
	*/

#pragma region 초기화

	for (int i = 0; i < WORLDY; ++i)
	{
		for (int j = 0; j < WORLDX; ++j)
		{
			if (nullptr != m_pTerrains[i][j])
				m_pTerrains[i][j]->Clear_Node();
		}
	}

	//지나온 노드(최종경로가 아니다)
	vector<CTerrain*> visited;
	//탐사한 노드
	set<CTerrain*> open;
	//검사의 중심이 되는 현재노드(그냥 visited의 마지막 노드)
	CTerrain* pCurrNode = m_pTerrains[srcY][srcX];

	_int currX = srcX;
	_int currY = srcY;

	_int minFcost = INT_MAX;
	//Fcost가 같으면 Hcost 기준으로 루트정함
	_int minHcost = INT_MAX;

	_int diaCost = (int)sqrt(TILECX * TILECX + TILECY * TILECY);

	int startHcost = 0;
	_int tmpX = abs(dstX - srcX);
	_int tmpY = abs(dstY - srcY);
	if (tmpX > tmpY)
		startHcost = tmpY * diaCost + (tmpX - tmpY) * TILECX;
	else
		startHcost = tmpX * diaCost + (tmpY - tmpX) * TILECX;

	//시작노드를 open리스트에 넣음
	pCurrNode->Set_Node(CTerrain::NODE(0, startHcost, startHcost, srcX, srcY, nullptr));
	open.emplace(pCurrNode);

#pragma endregion
	
	//더이상 탐색할 노드가 없을때까지
	while (open.size() > 0)
	{
		minFcost = INT_MAX;
		minHcost = INT_MAX;

		/*
		open set을 검사해서 Fcost가 가장 작은 노드를 찾는다.
		주의) 만약 INT_MAX가 아닌 curFcost랑 비교하면 Fcost가 현재 노드보다 더 커지는 경우는 배제해버린다.
		즉, 장애물을 돌아가는 경로는 아예 생각도 안해버림.
		*/
		for (auto& pTerrain : open)
		{
			CTerrain::NODE node = pTerrain->Get_Node();
			if (minFcost > node.Fcost)
			{
				minFcost = node.Fcost;
				pCurrNode = pTerrain;
				currX = node.X;
				currY = node.Y;
			}
			else if (minFcost == node.Fcost)
			{
				if (minHcost > node.Hcost)
				{
					minHcost = node.Hcost;
					pCurrNode = pTerrain;
					currX = node.X;
					currY = node.Y;
				}
			}

		}

		//선택된 노드는 open리스트에서 제거한다.
		open.erase(pCurrNode);
		//방문했다고 표시
		visited.push_back(pCurrNode);

		//목적지에 도달했으면 끝
		if (m_pTerrains[dstY][dstX] == pCurrNode)
			break;


		//현재노드의 왼쪽 위 타일의 인덱스
		int tmpY = currY - 1;
		int tmpX = currX - 1;

		//주변 8 타일을 검사해서 cost를 셋팅한다.
		for (int i = tmpY; i <= tmpY + 2; ++i)
		{
			for (int j = tmpX; j <= tmpX + 2; ++j)
			{
#pragma region 예외처리
				//유효하지 않은 타일 제외
				if (i >= WORLDX || j >= WORLDY || i < 0 || j < 0)
					continue;
				if (nullptr == m_pTerrains[i][j])
					continue;
				//가지 못하는 곳이면 제외
				if (!m_pTerrains[i][j]->IsMovable())
					continue;
				//이미 방문한 곳이면 제외
				if (find(visited.begin(), visited.end(), m_pTerrains[i][j]) != visited.end())
					continue;
				//자기자신 제외
				if (i == currY && j == currX)
					continue;
				//자기자신이 nullptr면 끝냄
				if (nullptr == pCurrNode)
					return E_FAIL;

#pragma endregion
				//cost 구하기
				_int Gcost = 0;
				_int Hcost = 0;
				_int Fcost = 0;

				//끝 노드와의 거리
				_int distX = abs(dstX - j);
				_int distY = abs(dstY - i);

				/*
				대각선을 이용해야 최단거리다. 따라서 대각선을 만든다.
				X방향으로 1번 + Y방향으로 1번 = 대각선으로 1번
				*/
				//cost는 양수가 나와야 되기 때문에 나눠서 처리 
				if (distX > distY)
					Hcost = diaCost * distY + (distX - distY) * TILECX;
				else
					Hcost = diaCost * distX + (distY - distX) * TILECX;

				//현재노드와의 거리
				distX = abs(currX - j);
				distY = abs(currY - i);

				if (distX > distY)
					Gcost = pCurrNode->Get_Node().Gcost + (diaCost * distY + (distX - distY) * TILECX);
				else
					Gcost = pCurrNode->Get_Node().Gcost + (diaCost * distX + (distY - distX) * TILECX);

				Fcost = Hcost + Gcost;

				//만약 구한 Fcost가 기존의 것보다 작으면 기존 것을 갱신, 부모설정
				if (m_pTerrains[i][j]->Get_Node().Fcost > Fcost)
					m_pTerrains[i][j]->Set_Node(CTerrain::NODE(Gcost, Hcost, Fcost, j, i, pCurrNode));

				//검사대상에 넣는다. (중복없이)
				open.emplace(m_pTerrains[i][j]);


			}
		}
	}


	//visited에 들어가 있는건 방문했던 모든 노드다. 그 중에서 경로를 뽑아내야한다.

	//도착지점
	CTerrain* pLastNode = visited.back();
	
	//길을 찾지 못했을때는 목표와 가장 가까운 곳을 도착지로 설정
	if (open.size() == 0)
	{
		//경로중에 Hcost가 가장 낮은것을 목적지로함.
		//Hcost가 같으면 Gcost를 비교 낮은걸 취함. (더 이동이 적은것을 택함)
		if (visited.size() > 0)
		{
			pLastNode = visited[0];

			for (int i = 1; i < visited.size(); ++i)
			{
				_int lastHcost = pLastNode->Get_Node().Hcost;
				_int lastGcost = pLastNode->Get_Node().Gcost;
				_int currHcost = visited[i]->Get_Node().Hcost;
				_int currGcost = visited[i]->Get_Node().Gcost;

				if (lastHcost > currHcost)
				{
					pLastNode = visited[i];
				}
				else if (lastHcost == currHcost)
				{
					if (lastGcost > currGcost)
						pLastNode = visited[i];
				}
			}
		}
	}

	//부모가 없을때까지(시작노드까지)
	while (pLastNode->Get_Node().pParent != nullptr)
	{
		CTransform* pTransform = (CTransform*)pLastNode->Get_Module(L"Transform");
		if (nullptr == pTransform)
			return E_FAIL;

		_out.push_back(pTransform->Get_Position());

		//부모를 타고 들어가기
		pLastNode = pLastNode->Get_Node().pParent;
	}

	//뽑아낸건 경로의 반대이기 때문에 제대로 해줌
	reverse(_out.begin(), _out.end());
	return S_OK;
}

Vector3 CWorld::Get_RandomPos()
{
	_uint ranX = 0;
	_uint ranY = 0;

	do
	{
		ranX = rand() % WORLDX;
		ranY = rand() % WORLDY;

	} while (m_pTerrains[ranY][ranX] == nullptr || !m_pTerrains[ranY][ranX]->IsMovable());
	
	return Vector3((float)ranX * TILECX, (float)ranY * TILECY, 0.f, 1.f);
}




HRESULT CWorld::Save_World(const _tchar* _filePath)
{
	HANDLE hFile = CreateFile(_filePath, GENERIC_WRITE
		, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD dwByte = 0;
	int zero = 0;
	int one = 1;
	//데이터 앞에 표시를 달자. 널이면 0, 널이 아니면 1
	for (auto& terrainArr : m_pTerrains)
	{
		for (auto& terrain : terrainArr)
		{
			if (nullptr == terrain)
			{
				WriteFile(hFile, &zero, sizeof(int), &dwByte, NULL);
			}
			else
			{
				WriteFile(hFile, &one, sizeof(int), &dwByte, NULL);
				WriteFile(hFile, &terrain->Get_SaveData(), sizeof(CTerrain::SAVE_DATA), &dwByte, NULL);
			}
		}
	}
	CloseHandle(hFile);
	MessageBox(g_hWnd, L"Tile Save", L"Success", MB_OK);
	return S_OK;
}

HRESULT CWorld::Load_World(const _tchar* _filePath, SCENEID _eSceneID)
{
	HANDLE hFile = CreateFile(_filePath, GENERIC_READ
		, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	//현재 타일을 모두 지운다.
	Free();

	DWORD				dwByte = 0;
	int					prefix = 0;
	CTerrain::SAVE_DATA tSaveData = {};

	_uint index = 0;
	while (true)
	{
		ReadFile(hFile, &prefix, sizeof(int), &dwByte, NULL);

		if (0 == dwByte)
			break;

		_uint y = index / WORLDX;
		_uint x = index % WORLDX;
		//읽을 데이터가 nullptr가 아니면 읽는다.
		if (0 != prefix)
		{

			ReadFile(hFile, &tSaveData, sizeof(CTerrain::SAVE_DATA), &dwByte, NULL);

			if (0 == dwByte)
				break;

			/*
			프로토타입 태그를 통해 올바른 클래스로 변환한다.
			예를들어 CDoor의 프로토타입에서 복사된 CGameObject가 CTerrain으로 변환되어 Load_Data를 통해 데이터가 셋팅된다.
			*/
			m_pTerrains[y][x] = (CTerrain*)CObjMgr::Get_Instance()->Add_GO_To_Layer(tSaveData.m_PrototypeTag, m_eSceneID, L"Terrain", _eSceneID);
			if (nullptr == m_pTerrains[y][x])
				return E_FAIL;
			Safe_AddRef(m_pTerrains[y][x]);

			m_pTerrains[y][x]->Load_Data(tSaveData);



		}
		else
			m_pTerrains[y][x] = nullptr;


		if (0 == dwByte)
			break;

		++index;

	}
	CloseHandle(hFile);
	MessageBox(g_hWnd, L"Tile Load", L"Success", MB_OK);
	return S_OK;
}

HRESULT CWorld::Initalize_Prototypes(PDIRECT3DDEVICE9 _pGraphic_Device, SCENEID _eSceneID)
{

#pragma region 타일생성

	CObjMgr* pObjMgr = CObjMgr::Get_Instance();
	if (nullptr == pObjMgr)
		return E_FAIL;
	Safe_AddRef(pObjMgr);

	//팔레트에 쓸 타일(프로토타입)을 만든다.
	CTerrain* pTerrain = CTerrain::Create(_pGraphic_Device, TERRAIN(false), L"lv_One_bookshelf", _eSceneID);
	//오브젝트 매니저에 등록한다.
	pObjMgr->Add_Prototype(L"lv_One_bookshelf", _eSceneID, pTerrain);
	pTerrain = CDoor::Create(_pGraphic_Device, TERRAIN(false), L"lv_One_door", _eSceneID);
	pObjMgr->Add_Prototype(L"lv_One_door", _eSceneID, pTerrain);
	pTerrain = CTerrain::Create(_pGraphic_Device, TERRAIN(true), L"lv_One_floor", _eSceneID);
	pObjMgr->Add_Prototype(L"lv_One_floor", _eSceneID, pTerrain);
	pTerrain = CTerrain::Create(_pGraphic_Device, TERRAIN(true), L"lv_One_floor_burned", _eSceneID);
	pObjMgr->Add_Prototype(L"lv_One_floor_burned", _eSceneID, pTerrain);
	pTerrain = CTerrain::Create(_pGraphic_Device, TERRAIN(true), L"lv_One_floor_grass", _eSceneID);
	pObjMgr->Add_Prototype(L"lv_One_floor_grass", _eSceneID, pTerrain);
	pTerrain = CTerrain::Create(_pGraphic_Device, TERRAIN(false), L"lv_One_floor_mask_A", _eSceneID);
	pObjMgr->Add_Prototype(L"lv_One_floor_mask_A", _eSceneID, pTerrain);
	pTerrain = CTerrain::Create(_pGraphic_Device, TERRAIN(false), L"lv_One_floor_mask_B", _eSceneID);
	pObjMgr->Add_Prototype(L"lv_One_floor_mask_B", _eSceneID, pTerrain);
	pTerrain = CTerrain::Create(_pGraphic_Device, TERRAIN(false), L"lv_One_floor_mask_C", _eSceneID);
	pObjMgr->Add_Prototype(L"lv_One_floor_mask_C", _eSceneID, pTerrain);
	pTerrain = CTerrain::Create(_pGraphic_Device, TERRAIN(false), L"lv_One_floor_mask_D", _eSceneID);
	pObjMgr->Add_Prototype(L"lv_One_floor_mask_D", _eSceneID, pTerrain);
	pTerrain = CTerrain::Create(_pGraphic_Device, TERRAIN(false), L"lv_One_floor_mask_E", _eSceneID);
	pObjMgr->Add_Prototype(L"lv_One_floor_mask_E", _eSceneID, pTerrain);
	pTerrain = CTerrain::Create(_pGraphic_Device, TERRAIN(false), L"lv_One_floor_mask_F", _eSceneID);
	pObjMgr->Add_Prototype(L"lv_One_floor_mask_F", _eSceneID, pTerrain);
	pTerrain = CTerrain::Create(_pGraphic_Device, TERRAIN(false), L"lv_One_floor_mask_G", _eSceneID);
	pObjMgr->Add_Prototype(L"lv_One_floor_mask_G", _eSceneID, pTerrain);
	pTerrain = CTerrain::Create(_pGraphic_Device, TERRAIN(false), L"lv_One_floor_mask_H", _eSceneID);
	pObjMgr->Add_Prototype(L"lv_One_floor_mask_H", _eSceneID, pTerrain);
	pTerrain = CTerrain::Create(_pGraphic_Device, TERRAIN(false), L"lv_One_floor_mask_I", _eSceneID);
	pObjMgr->Add_Prototype(L"lv_One_floor_mask_I", _eSceneID, pTerrain);
	pTerrain = CTerrain::Create(_pGraphic_Device, TERRAIN(true), L"lv_One_floor_mold", _eSceneID);
	pObjMgr->Add_Prototype(L"lv_One_floor_mold", _eSceneID, pTerrain);
	pTerrain = CTerrain::Create(_pGraphic_Device, TERRAIN(false), L"lv_One_prison", _eSceneID);
	pObjMgr->Add_Prototype(L"lv_One_prison", _eSceneID, pTerrain);
	pTerrain = CTerrain::Create(_pGraphic_Device, TERRAIN(false), L"lv_One_sign", _eSceneID);
	pObjMgr->Add_Prototype(L"lv_One_sign", _eSceneID, pTerrain);
	pTerrain = CTerrain::Create(_pGraphic_Device, TERRAIN(true), L"lv_One_stair_down", _eSceneID);
	pObjMgr->Add_Prototype(L"lv_One_stair_down", _eSceneID, pTerrain);
	pTerrain = CTerrain::Create(_pGraphic_Device, TERRAIN(true), L"lv_One_stair_up", _eSceneID);
	pObjMgr->Add_Prototype(L"lv_One_stair_up", _eSceneID, pTerrain);
	pTerrain = CTerrain::Create(_pGraphic_Device, TERRAIN(false), L"lv_One_statue_rock", _eSceneID);
	pObjMgr->Add_Prototype(L"lv_One_statue_rock", _eSceneID, pTerrain);
	pTerrain = CTerrain::Create(_pGraphic_Device, TERRAIN(false), L"lv_One_statue_wood", _eSceneID);
	pObjMgr->Add_Prototype(L"lv_One_statue_wood", _eSceneID, pTerrain);
	pTerrain = CTerrain::Create(_pGraphic_Device, TERRAIN(true), L"lv_One_trap_delusion", _eSceneID);
	pObjMgr->Add_Prototype(L"lv_One_trap_delusion", _eSceneID, pTerrain);
	pTerrain = CTerrain::Create(_pGraphic_Device, TERRAIN(true), L"lv_One_trap_fire", _eSceneID);
	pObjMgr->Add_Prototype(L"lv_One_trap_fire", _eSceneID, pTerrain);
	pTerrain = CTerrain::Create(_pGraphic_Device, TERRAIN(true), L"lv_One_trap_orange", _eSceneID);
	pObjMgr->Add_Prototype(L"lv_One_trap_orange", _eSceneID, pTerrain);
	pTerrain = CTerrain::Create(_pGraphic_Device, TERRAIN(true), L"lv_One_trap_paralyze", _eSceneID);
	pObjMgr->Add_Prototype(L"lv_One_trap_paralyze", _eSceneID, pTerrain);
	pTerrain = CTerrain::Create(_pGraphic_Device, TERRAIN(true), L"lv_One_trap_poison", _eSceneID);
	pObjMgr->Add_Prototype(L"lv_One_trap_poison", _eSceneID, pTerrain);
	pTerrain = CTerrain::Create(_pGraphic_Device, TERRAIN(true), L"lv_One_trap_pupple", _eSceneID);
	pObjMgr->Add_Prototype(L"lv_One_trap_pupple", _eSceneID, pTerrain);
	pTerrain = CTerrain::Create(_pGraphic_Device, TERRAIN(true), L"lv_One_trap_remain", _eSceneID);
	pObjMgr->Add_Prototype(L"lv_One_trap_remain", _eSceneID, pTerrain);
	pTerrain = CTerrain::Create(_pGraphic_Device, TERRAIN(false), L"lv_One_wall", _eSceneID);
	pObjMgr->Add_Prototype(L"lv_One_wall", _eSceneID, pTerrain);
	pTerrain = CTerrain::Create(_pGraphic_Device, TERRAIN(false), L"lv_One_wall_sewer", _eSceneID);
	pObjMgr->Add_Prototype(L"lv_One_wall_sewer", _eSceneID, pTerrain);


#pragma endregion

	Safe_Release(pObjMgr);

	return S_OK;
}

HRESULT CWorld::Collision_Terrain(CGameObject* _pObj)
{
	if (nullptr == _pObj)
		return E_FAIL;

	CTransform* pTransform = (CTransform*)_pObj->Get_Module(L"Transform");

	int x = (int)pTransform->Get_Position().x / TILECX;
	int y = (int)pTransform->Get_Position().y / TILECY;

	if (x < 0 || y < 0 || x >= WORLDX || y >= WORLDY)
		return E_FAIL;

	if (nullptr == m_pTerrains[y][x])
		return E_FAIL;

	//m_pTerrains[y][x]이 null이 아니라는 말은 해당 타일과 충돌했다는 것.
	CCollisionMgr::Collision(list<CGameObject*>(1, _pObj), list<CGameObject*>(1, m_pTerrains[y][x]), true);

	return S_OK;
}


void CWorld::Free()
{
	for (auto& tileArr : m_pTerrains)
	{
		for (auto& tile : tileArr)
			Safe_Release(tile);
	}

}


#pragma once
#include "Base.h"

BEGIN(MyGame)
#define	MAX_LAYER_CNT 3
class CLayer;
class CGameObject;
/*
턴을 종료한다는 개념과 State를 변경한다는 개념을 분리했다.
애초에 그걸 셋트로 생각하는게 잘못이었음.
*/
class CTurnMgr : public CBase
{
	DECLARE_SINGLETON(CTurnMgr)
private:
	explicit CTurnMgr();
	explicit CTurnMgr(CTurnMgr& _rhs);
	virtual ~CTurnMgr() = default;

private:
	SCENEID	m_eCurrScene = SCENE_END;
	size_t	m_iLayerIndex = 0;
	size_t	m_iObjIndex = 0;
	_int	m_iCurrTurn = 0;
	_int	m_iMaxTurn = 0;

	_bool	m_bActLock = false;

private:
	CLayer*				m_pActorLayers[MAX_LAYER_CNT];
	list<CBase*>		m_listObservers;

private:
	CGameObject*		m_pCurrActor = nullptr;
	_bool				m_bTurnStart = false;
	//모든 액터의 턴이 끝난상태인가
	_bool				m_bTurnEnd = true;
public:
	HRESULT				Initialize();
	_int				Get_TurnCnt() { return m_iMaxTurn; }

public:
	//모든 액터의 턴이 끝난상태인가
	_bool	CanAct() { return m_bTurnEnd; }
public:
	_int	Update_Simultaneously();

	HRESULT	MoveTurn_Simultaneously(_int _iTurnCnt);

public:
	HRESULT RegisterObserver(CBase* _pObserver);
	HRESULT UnRegisterObserver(CBase* _pObserver);
	HRESULT	ClearObservers();


public:
	virtual void Free() override;
};
END

#pragma once
#include "Monster.h"

BEGIN(MyGame)
class CTexture;
class CAIStateCon;
class CGoo final : public CMonster
{
private:
	explicit CGoo(PDIRECT3DDEVICE9 _pGraphic_Device) :CMonster(_pGraphic_Device) {};
	explicit CGoo(CGoo& _rhs);
	virtual ~CGoo() = default;



public:
	virtual HRESULT	Initialize_Prototype(_tchar* _pFilePath = nullptr);
	virtual HRESULT Initialize(void * _param = nullptr);

public:
	static CGoo* Create(PDIRECT3DDEVICE9 _pGraphic_Device);
	// CMonster을(를) 통해 상속됨
	virtual CGameObject * Clone(void * _param = nullptr) override;


public:
	void OnDead() override;

public:
	// CBase을(를) 통해 상속됨
	virtual void Free() override;

};
END

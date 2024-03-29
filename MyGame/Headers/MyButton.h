#pragma once
#include "GameObject.h"

BEGIN(MyGame)


class CVIBuffer;
class CTransform;
class CTexture;
class CShader;

class CMyButton : public CGameObject
{

public:
	enum RENDER_STATE { STATE_ENABLE = 0, STATE_DISABLE = 1, STATE_END};
protected:
	explicit CMyButton(PDIRECT3DDEVICE9 _pGraphic_Device) :CGameObject(_pGraphic_Device) {};
	explicit CMyButton(CMyButton& _rhs);
	virtual ~CMyButton() = default;


protected:
	CVIBuffer*		m_pVIBuffer = nullptr;
	CTransform*		m_pTransform = nullptr;
	CTexture*		m_pTexture = nullptr;
	CShader*		m_pShader = nullptr;
	RECT			m_tRect = {};
	const _tchar*	m_pText = L"";
	RENDER_STATE	m_eRenderState = STATE_ENABLE;
	_bool			m_bClicked = false;

protected:
	//여러개의 리스너
	vector<function<void()>> m_vecOnListener;
	//하나의 리스너(계속 덮어쓰는 용도)
	function<void()>	m_Listener;


public:
	virtual HRESULT Initialize(Vector4 _vPos, Vector2 _vSize, _tchar* _pTextureTag, SCENEID _eTextureSceneID);
	virtual HRESULT Initialize(Vector4 _vPos, Vector2 _vSize, CTexture* _pTexture);
	virtual _int	Update(_double _timeDelta)	override;
	virtual _int	LateUpate(_double _timeDelta);
	virtual HRESULT	Render();

public:
	virtual HRESULT	Add_Listener(function<void()> _listener)
	{
		m_vecOnListener.push_back(_listener);
		return S_OK;
	}
	virtual HRESULT	Set_Listener(function<void()> _listener)
	{
		m_Listener = _listener;
		return S_OK;
	}
	

	void	Set_Text(const _tchar* pText) { m_pText = pText; }
	HRESULT	Set_RenderState(RENDER_STATE _eRenderState);
	
public:
	virtual HRESULT	OnKeyDown(_int KeyCode) override;

public:
	static CMyButton*	Create(PDIRECT3DDEVICE9 _pGraphic_Device, Vector4 _vPos, Vector2 _vSize, _tchar* _pTextureTag, SCENEID _eTextureSceneID);
	static CMyButton*	Create(PDIRECT3DDEVICE9 _pGraphic_Device, Vector4 _vPos, Vector2 _vSize, CTexture* _pTexture);

public:
	virtual CGameObject * Clone(void * _pArg = nullptr) override;
	virtual void Free() override;
};

END
#include "CUTest.h"

// App.cpp : Defines the entry point for the application.
//

class CFrameWindowWnd : public CWindowWnd, public INotifyUI
{
public:
	CFrameWindowWnd() { };
	LPCTSTR GetWindowClassName() const { return _T("UIMainFrame"); };
	UINT GetClassStyle() const { return  CS_DBLCLKS ; }
	void OnFinalMessage(HWND /*hWnd*/) { delete this; };

	void Init() 
	{
		m_pCloseBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("closebtn")));
		m_pMaxBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("maxbtn")));
		m_pRestoreBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("restorebtn")));
		m_pMinBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("minbtn")));
		CComboUI* pAccountCombo = static_cast<CComboUI*>(m_pm.FindControl(_T("accountcombo")));
		CEditUI* pAccountEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("accountedit")));
		if( pAccountCombo && pAccountEdit ) pAccountEdit->SetText(pAccountCombo->GetText());
		pAccountEdit->SetFocus();
	}

	bool OnHChanged(void* param) {
		TNotifyUI* pMsg = (TNotifyUI*)param;
		if( pMsg->sType == _T("valuechanged") ) {
			short H, S, L;
			CPaintManagerUI::GetHSL(&H, &S, &L);
			CPaintManagerUI::SetHSL(true, (static_cast<CSliderUI*>(pMsg->pSender))->GetValue(), S, L);
		}
		return true;
	}

	bool OnSChanged(void* param) {
		TNotifyUI* pMsg = (TNotifyUI*)param;
		if( pMsg->sType == _T("valuechanged") ) {
			short H, S, L;
			CPaintManagerUI::GetHSL(&H, &S, &L);
			CPaintManagerUI::SetHSL(true, H, (static_cast<CSliderUI*>(pMsg->pSender))->GetValue(), L);
		}
		return true;
	}

	bool OnLChanged(void* param) {
		TNotifyUI* pMsg = (TNotifyUI*)param;
		if( pMsg->sType == _T("valuechanged") ) {
			short H, S, L;
			CPaintManagerUI::GetHSL(&H, &S, &L);
			CPaintManagerUI::SetHSL(true, H, S, (static_cast<CSliderUI*>(pMsg->pSender))->GetValue());
		}
		return true;
	}

	bool OnAlphaChanged(void* param) {
		TNotifyUI* pMsg = (TNotifyUI*)param;
		if( pMsg->sType == _T("valuechanged") ) {
			m_pm.SetTransparent((static_cast<CSliderUI*>(pMsg->pSender))->GetValue());
		}
		return true;
	}

	void OnPrepare() 
	{
		CSliderUI* pSilder = static_cast<CSliderUI*>(m_pm.FindControl(_T("alpha_controlor")));
		if( pSilder ) pSilder->OnNotify += MakeDelegate(this, &CFrameWindowWnd::OnAlphaChanged);
		pSilder = static_cast<CSliderUI*>(m_pm.FindControl(_T("h_controlor")));
		if( pSilder ) pSilder->OnNotify += MakeDelegate(this, &CFrameWindowWnd::OnHChanged);
		pSilder = static_cast<CSliderUI*>(m_pm.FindControl(_T("s_controlor")));
		if( pSilder ) pSilder->OnNotify += MakeDelegate(this, &CFrameWindowWnd::OnSChanged);
		pSilder = static_cast<CSliderUI*>(m_pm.FindControl(_T("l_controlor")));
		if( pSilder ) pSilder->OnNotify += MakeDelegate(this, &CFrameWindowWnd::OnLChanged);
	}

	void Notify(TNotifyUI& msg)
	{
		if(msg.sType==_T("click"))
		{
			if( msg.pSender == m_pCloseBtn ) 
			{
				PostQuitMessage(0);
				return; 
			}
			else if( msg.pSender == m_pMinBtn ) 
			{ 
				::ShowWindow(*this,SW_MINIMIZE);
				return;
			}
			else if( msg.pSender->GetName() == _T("changeskinbtn") ) 
			{
				if( CPaintManagerUI::GetResourceZip() == _T("testRes.zip") )
					CPaintManagerUI::SetResourceZip(_T("test1Res.zip"));
				else
					CPaintManagerUI::SetResourceZip(_T("testRes.zip"));
				CPaintManagerUI::ReloadSkin();
			}
		}
		else if( msg.sType == _T("itemselect") ) {
			if( msg.pSender->GetName() == _T("accountcombo") ) {
				CEditUI* pAccountEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("accountedit")));
				if( pAccountEdit ) pAccountEdit->SetText(msg.pSender->GetText());
			}
		}
		else if( msg.sType == _T("windowinit") ) 
			OnPrepare();
	}

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if( uMsg == WM_CREATE ) {

			LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
			styleValue &= ~WS_CAPTION;
			::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

			m_pm.Init(m_hWnd);
			CDialogBuilder builder;
			CControlUI* pRoot = builder.Create(_T("test.xml"), NULL, &m_pm);
			ASSERT(pRoot && "Failed to parse XML");
			m_pm.AttachDialog(pRoot);
			m_pm.AddNotifier(this);
			Init();
			return 0;
		}
		else if( uMsg == WM_DESTROY ) {
			::PostQuitMessage(0L);
		}
		else if( uMsg == WM_ERASEBKGND ) {
			return 1;
		}
		LRESULT lRes = 0;
		if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}

public:
	CPaintManagerUI m_pm;
	CButtonUI* m_pCloseBtn;
	CButtonUI* m_pMaxBtn;
	CButtonUI* m_pRestoreBtn;
	CButtonUI* m_pMinBtn;
};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());
	CPaintManagerUI::SetResourceZip(_T("testRes.zip"));

	HRESULT Hr = ::CoInitializeEx(NULL,COINIT_MULTITHREADED);
	if( FAILED(Hr) ) return 0;

	CFrameWindowWnd* pFrame = new CFrameWindowWnd();
	if( pFrame == NULL ) return 0;
	pFrame->Create(NULL, _T("这是一个最简单的测试用exe，修改test1.xml就可以看到效果"), 0, 0L);
	pFrame->CenterWindow();
	pFrame->ShowWindow(true);
	CPaintManagerUI::MessageLoop();
	::CoUninitialize();
	return 0;
}

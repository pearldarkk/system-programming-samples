
// ServerDlg.h : header file
//

#pragma once

#include "ListCtrlEx.h"

// CServerDlg dialog
class CServerDlg : public CDialogEx
{
// Construction
public:
	CServerDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
    CWinThread* hMasterThread;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
//    afx_msg void OnEnChangeEdit1();
    CButton btnStart;
    CString strIP;
    CString strPort;
    CListCtrlEx lstLog;
    CListCtrlEx lstUser;
    afx_msg void OnClickedButton1();

    typedef struct _MultiCardThreadParam {
        CWnd* wndHandler;
        LPVOID lpParam;
    } MultiCardThreadParam;
    MultiCardThreadParam mctParam;

    static UINT MasterThread(LPVOID lpParam);
    BOOL stop;
//    afx_msg void OnSize(UINT nType, int cx, int cy);
};

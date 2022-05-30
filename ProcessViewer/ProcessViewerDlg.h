
// ProcessViewerDlg.h : header file
//

#pragma once

#include "ListCtrlEx.h"

// CProcessViewerDlg dialog
class CProcessViewerDlg : public CDialogEx
{
// Construction
public:
	CProcessViewerDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROCESSVIEWER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    CButton btnRefresh;
    CListCtrlEx lstResult;

    void ViewProcesses();
    void EnableDebugPrivilege();
    afx_msg void OnClickedButton1();
    CStatic stcStatus;
};

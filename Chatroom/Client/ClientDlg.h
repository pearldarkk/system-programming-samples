
// ClientDlg.h : header file
//

#pragma once

// CClientDlg dialog
class CClientDlg : public CDialogEx
{
// Construction
public:
	CClientDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
    CWinThread* hListenThread;
    //CWinThread* hSendThread;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
//    afx_msg void OnEnChangeEdit2();
    CButton btnConnect;
    CButton btnSend;
    CString strIP;
    CString strPort;

    typedef struct _MultiCardThreadParam {
        CWnd* wndHandler;
        LPVOID lpParam;
    } MultiCardThreadParam;
    MultiCardThreadParam mctParam;

    static UINT ListenThread(LPVOID lpParam);

    BOOL isConnect;
    afx_msg void OnClickedButton1();
    afx_msg void OnClickedButton2();
    CListBox lstMsg;
    CString strSelfMsg;
//    afx_msg void OnEnChangeEdit1();
    CEdit edIP;
    CEdit edPort;
    CEdit edSelfMsg;
};

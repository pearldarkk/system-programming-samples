
// FileScannerDlg.h : header file
//

#pragma once

#include "FileFindEx.h"
#include "ListCtrlEx.h"

// CFileScannerDlg dialog
class CFileScannerDlg : public CDialogEx
{
// Construction
public:
	CFileScannerDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILESCANNER_DIALOG };
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
    BOOL stop;
    CString strtok;
    CImageList img;
    CButton btnSearch;
    CButton btnStop;
    CListCtrlEx lstResult;
    CStatic stcStatus;
    CString strFileLoc;
    CString strFileName;
    BOOL bRecursive;

    void ScanFile();
    void RecursiveScanFile(CString strFolder);
    afx_msg void OnClickedButton1();
    afx_msg void OnClickedButton2();
//    afx_msg void OnBnClickedCheck1();

};

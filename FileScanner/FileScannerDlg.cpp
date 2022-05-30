
// FileScannerDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "FileScanner.h"
#include "FileScannerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFileScannerDlg dialog



CFileScannerDlg::CFileScannerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILESCANNER_DIALOG, pParent)
    , strFileLoc(_T(""))
    , strFileName(_T(""))
    , bRecursive(FALSE)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    strtok = _T("");
}

void CFileScannerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BUTTON1, btnSearch);
    DDX_Control(pDX, IDC_BUTTON2, btnStop);
    DDX_Control(pDX, IDC_LIST1, lstResult);
    DDX_Control(pDX, IDC_STATIC_STATUS, stcStatus);
    DDX_Text(pDX, IDC_EDIT1, strFileLoc);
    DDX_Text(pDX, IDC_EDIT2, strFileName);
    DDX_Check(pDX, IDC_CHECK1, bRecursive);
}

BEGIN_MESSAGE_MAP(CFileScannerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON1, &CFileScannerDlg::OnClickedButton1)
    ON_BN_CLICKED(IDC_BUTTON2, &CFileScannerDlg::OnClickedButton2)
//    ON_BN_CLICKED(IDC_CHECK1, &CFileScannerDlg::OnBnClickedCheck1)
END_MESSAGE_MAP()


// CFileScannerDlg message handlers

BOOL CFileScannerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
    img.Create(IDB_LIST, 16, 2, RGB(255, 255, 255));
    lstResult.SetImageList(&img, LVSIL_SMALL);
    lstResult.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 150);
    lstResult.InsertColumn(1, _T("Location"), LVCFMT_LEFT, 150);
    lstResult.InsertColumn(2, _T("Size in bytes"), LVCFMT_LEFT, 100);
    lstResult.InsertColumn(3, _T("Date modified"), LVCFMT_LEFT, 150);
    lstResult.InsertColumn(4, _T("Date created"), LVCFMT_LEFT, 150);

    /*TCHAR pBuf[256];
    GetCurrentDirectory(256, pBuf);
    strFileLoc.Format(_T("%s"), pBuf);*/
    UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFileScannerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFileScannerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

void CFileScannerDlg::ScanFile() {
    UpdateData(TRUE);

    CString strtmp, strFolder, strName;
    CFileFindEx cFile;
    int i;
    BOOL b;
    MSG msg;

    if (strFileLoc.Right(1) == "\\")
        strtmp = strFileLoc + _T("*.*");
    else {
        strtmp = strFileLoc + _T("\\");
        strtmp += _T("*.*");
    }

    b = cFile.FindFile(strtmp);
    while (b) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (stop)
            return;

        b = cFile.FindNextFile();
        if (cFile.IsDots())
            continue;
        i = lstResult.GetItemCount();
        stcStatus.SetWindowText(cFile.GetFilePath());
        strFolder = cFile.GetFilePath().Left(cFile.GetFilePath().ReverseFind('\\') + 1);
        strName = cFile.GetFileName();
        strName.MakeUpper();
        if (strName.Find(strtok) != -1) {
            if (cFile.IsDirectory()) {
                lstResult.AddItem(cFile.GetFileName(), i, 0, -1, 0);
                lstResult.AddItem(strFolder, i, 1);
                lstResult.AddItem(_T("Folder"), i, 2);
                lstResult.AddItem(cFile.GetLastWriteTimeString(), i, 3);
                lstResult.AddItem(cFile.GetCreationTimeString(), i, 4);
            }
            else {
                lstResult.AddItem(cFile.GetFileName(), i, 0, -1, 1);
                lstResult.AddItem(strFolder, i, 1);
                lstResult.AddItem(cFile.GetLengthString(), i, 2);
                lstResult.AddItem(cFile.GetLastWriteTimeString(), i, 3);
                lstResult.AddItem(cFile.GetCreationTimeString(), i, 4);
            }
        }
    }
}

void CFileScannerDlg::RecursiveScanFile(CString strFolder) {
    UpdateData(TRUE);
    CString strtmp, strSearchingFolder, strName;
    CFileFindEx cFile;
    int i;
    BOOL b;
    MSG msg;

    if (strFolder.Right(1) == _T("\\"))
        strtmp = strFolder + _T("*.*");
    else {
        strtmp = strFolder + _T("\\");
        strtmp += _T("*.*");
    }

    b = cFile.FindFile(strtmp);

    while (b) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (stop)
            return;

        b = cFile.FindNextFile();
        if (cFile.IsDots())
            continue;
        i = lstResult.GetItemCount();
        stcStatus.SetWindowText(cFile.GetFilePath());
        strSearchingFolder = cFile.GetFilePath().Left(cFile.GetFilePath().ReverseFind('\\') + 1);
        strName = cFile.GetFileName();
        strName.MakeUpper();

        if (cFile.IsDirectory()) {
            if (strName.Find(strtok) != -1) {
                lstResult.AddItem(cFile.GetFileName(), i, 0, -1, 0);
                lstResult.AddItem(strSearchingFolder, i, 1);
                lstResult.AddItem(_T("Folder"), i, 2);
                lstResult.AddItem(cFile.GetLastWriteTimeString(), i, 3);
                lstResult.AddItem(cFile.GetCreationTimeString(), i, 4);
            }
            RecursiveScanFile(cFile.GetFilePath());
        }
        else {
            if (strName.Find(strtok) != -1) {
                lstResult.AddItem(cFile.GetFileName(), i, 0, -1, 1);
                lstResult.AddItem(strSearchingFolder, i, 1);
                lstResult.AddItem(cFile.GetLengthString(), i, 2);
                lstResult.AddItem(cFile.GetLastWriteTimeString(), i, 3);
                lstResult.AddItem(cFile.GetCreationTimeString(), i, 4);;
            }
        }
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFileScannerDlg::OnQueryDragIcon() 
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CFileScannerDlg::OnClickedButton1()
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    lstResult.DeleteAllItems();
    stop = FALSE;
    strtok = strFileName;
    strtok.MakeUpper();
    if (bRecursive == TRUE)
        RecursiveScanFile(strFileLoc);
    else ScanFile();
    UpdateData(FALSE);
}


void CFileScannerDlg::OnClickedButton2()
{
    // TODO: Add your control notification handler code here
    stop = TRUE;
}


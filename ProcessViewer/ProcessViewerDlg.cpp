
// ProcessViewerDlg.cpp : implementation file
//

#include <Windows.h>

#include "pch.h"
#include "framework.h"
#include "ProcessViewer.h"
#include "ProcessViewerDlg.h"
#include "afxdialogex.h"
#include <tlhelp32.h>
#include <processthreadsapi.h>
#include <winternl.h>
#include <psapi.h>

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


// CProcessViewerDlg dialog



CProcessViewerDlg::CProcessViewerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROCESSVIEWER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CProcessViewerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BUTTON1, btnRefresh);
    DDX_Control(pDX, IDC_LIST1, lstResult);
    DDX_Control(pDX, IDC_STATIC_STATUS, stcStatus);
}

BEGIN_MESSAGE_MAP(CProcessViewerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON1, &CProcessViewerDlg::OnClickedButton1)
END_MESSAGE_MAP()


// CProcessViewerDlg message handlers

BOOL CProcessViewerDlg::OnInitDialog()
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
    lstResult.InsertColumn(0, _T("PID"), LVCFMT_LEFT, 64);
    lstResult.InsertColumn(1, _T("Name"), LVCFMT_LEFT, 200);
    lstResult.InsertColumn(2, _T("Command line"), LVCFMT_LEFT, 400);
    lstResult.InsertColumn(3, _T("Full path"), LVCFMT_LEFT, 400);
    UpdateData(FALSE);
    EnableDebugPrivilege();
    ViewProcesses();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CProcessViewerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CProcessViewerDlg::OnPaint()
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

void CProcessViewerDlg::EnableDebugPrivilege() {
    HANDLE hToken = 0;
    TOKEN_PRIVILEGES tkp = { 0 };
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid)) {
            tkp.PrivilegeCount = 1;
            tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
            AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
        }
}

typedef NTSTATUS(NTAPI* _NtQueryInformationProcess)(
    HANDLE ProcessHandle,
    DWORD ProcessInformationClass,
    PVOID ProcessInformation,
    DWORD ProcessInformationLength,
    PDWORD ReturnLength
    );

void CProcessViewerDlg::ViewProcesses() {
    int i;
    CString id;
    HANDLE hProcessSnap;
    HANDLE hProcess;
    PROCESSENTRY32 pe32;
    PROCESS_BASIC_INFORMATION pbi;
    PVOID pebAddress;
    PVOID rtlUserProcParamsAddress;
    UNICODE_STRING uCommandline;
    WCHAR* commandline;
    CString cCommandline;
    TCHAR filepath[256];
    CString cFilePath;

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
        return;
    pe32.dwSize = sizeof PROCESSENTRY32;
    if (!Process32First(hProcessSnap, &pe32)) {
        CloseHandle(hProcessSnap);
        return;
    }
    do {
        i = lstResult.GetItemCount();
        id.Format(_T("%08d"), pe32.th32ProcessID);
        lstResult.AddItem(id, i, 0);
        lstResult.AddItem(pe32.szExeFile, i, 1);

        hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
        // get command line from peb of current process
        _NtQueryInformationProcess NtQueryInformationProcess = (_NtQueryInformationProcess)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationProcess");
        NtQueryInformationProcess(hProcess, ProcessBasicInformation, &pbi, sizeof(pbi), NULL);
        if (ReadProcessMemory(hProcess, &(((_PEB*)pbi.PebBaseAddress)->ProcessParameters), &rtlUserProcParamsAddress, sizeof(PVOID), NULL))
            if (ReadProcessMemory(hProcess, &(((_RTL_USER_PROCESS_PARAMETERS*)rtlUserProcParamsAddress)->CommandLine), &uCommandline, sizeof(uCommandline), NULL)) {
                commandline = (WCHAR*)malloc(uCommandline.Length);
                if (ReadProcessMemory(hProcess, uCommandline.Buffer, commandline, uCommandline.Length, NULL)) {
                    cCommandline.Format(_T("%.*s"), uCommandline.Length / 2, commandline);
                    lstResult.AddItem(cCommandline, i, 2);
                }
                free(commandline);
            }
        // get full file path
        cFilePath.Format(_T("%.*s"), GetModuleFileNameEx(hProcess, 0, filepath, 255), filepath);
        lstResult.AddItem(cFilePath, i, 3);
        CloseHandle(hProcess);

    } while (Process32Next(hProcessSnap, &pe32));
    CloseHandle(hProcessSnap);
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CProcessViewerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CProcessViewerDlg::OnClickedButton1()
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    stcStatus.SetWindowText(_T("Loading..."));
    lstResult.DeleteAllItems();
    ViewProcesses();
    UpdateData(FALSE);
    stcStatus.SetWindowText(_T("Completed"));
}


// ClientDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "Client.h"
#include "ClientDlg.h"
#include "afxdialogex.h"

#include <Windows.h>
#include <WS2tcpip.h>
#include <cstring>
#include <string>
#include <winsock2.h>
#include <atlconv.h>

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


// CClientDlg dialog



CClientDlg::CClientDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_CLIENT_DIALOG, pParent)
    , strIP(_T(""))
    , strPort(_T(""))
    , strSelfMsg(_T(""))
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BUTTON1, btnConnect);
    DDX_Control(pDX, IDC_BUTTON2, btnSend);
    DDX_Text(pDX, IDC_EDIT1, strIP);
    DDX_Text(pDX, IDC_EDIT2, strPort);
    DDX_Text(pDX, IDC_EDIT3, strSelfMsg);
    DDX_Control(pDX, IDC_LIST2, lstMsg);
    DDX_Control(pDX, IDC_EDIT1, edIP);
    DDX_Control(pDX, IDC_EDIT2, edPort);
    DDX_Control(pDX, IDC_EDIT3, edSelfMsg);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //    ON_EN_CHANGE(IDC_EDIT2, &CClientDlg::OnEnChangeEdit2)
    //ON_BN_CLICKED(IDC_BUTTON1, &CClientDlg::OnBnClickedButton1)
    //ON_EN_CHANGE(IDC_EDIT1, &CClientDlg::OnEnChangeEdit1)
    ON_BN_CLICKED(IDC_BUTTON1, &CClientDlg::OnClickedButton1)
    ON_BN_CLICKED(IDC_BUTTON2, &CClientDlg::OnClickedButton2)
    //ON_WM_SIZE()
    //ON_EN_CHANGE(IDC_EDIT1, &CClientDlg::OnEnChangeEdit1)
END_MESSAGE_MAP()


// CClientDlg message handlers

BOOL CClientDlg::OnInitDialog()
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
    btnSend.EnableWindow(FALSE);
    UpdateData(FALSE);
    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CClientDlg::OnPaint()
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

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CClientDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

UINT CClientDlg::ListenThread(LPVOID lpParam) {
    MultiCardThreadParam* mctParam = (MultiCardThreadParam*)lpParam;
    CClientDlg* cliDlg = (CClientDlg*)mctParam->wndHandler;
    SOCKET sock_peer = (SOCKET)mctParam->lpParam;
    CString strStatus, strTime, strError;
    CString strMsg;
    CStringA straMsg;
    char read[4096];

    while (cliDlg->isConnect) {
        int bytes_received = recv(sock_peer, read, 4096, 0);
        // peer closed
        if (bytes_received == 0)
            continue;
        if (bytes_received < 0) {
            return FALSE;
        }
        straMsg.Format("%.*s", bytes_received, read);
        strMsg = straMsg;
        cliDlg->lstMsg.InsertString(-1, strMsg);
    }
    return TRUE;
}

SOCKET sock_peer;

void CClientDlg::OnClickedButton1()
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    CString strStatus, strError;

    WSADATA d;
    btnConnect.GetWindowText(strStatus);
    if (strStatus == _T("Connect")) {
        if (WSAStartup(MAKEWORD(2, 2), &d)) {
            lstMsg.InsertString(-1, _T("Winsock intializing failed!"));
            return;
        }

        // config remote address
        addrinfo hints;
        memset(&hints, 0, sizeof hints);
        hints.ai_socktype = SOCK_STREAM;
        addrinfo* peer_address;
        CStringA straIP(strIP), straPort(strPort);
        if (getaddrinfo(straIP, straPort, &hints, &peer_address)) {
            strError.Format(_T("Remote address configuring failed with error code 0x%X"), WSAGetLastError());
            lstMsg.InsertString(-1, strError);
            return;
        }

        // create socket
        sock_peer = INVALID_SOCKET;
        sock_peer = socket(peer_address->ai_family, peer_address->ai_socktype, peer_address->ai_protocol);
        if (sock_peer == INVALID_SOCKET) {
            strError.Format(_T("Socket creating failed with error code 0x%X"), WSAGetLastError());
            lstMsg.InsertString(-1, strError);
            return;
        }

        // establish a connection to remote server
        if (connect(sock_peer, peer_address->ai_addr, peer_address->ai_addrlen)) {
            strError.Format(_T("Remote server connecting failed with error code 0x%X"), WSAGetLastError());
            lstMsg.InsertString(-1, strError);
            return;
        }
        freeaddrinfo(peer_address);
        // connection established

        // create listening thread and sending thread for client
        mctParam.wndHandler = this;
        mctParam.lpParam = (LPVOID)sock_peer;
        isConnect = TRUE;
        hListenThread = AfxBeginThread(ListenThread, (LPVOID)&mctParam);
        //hSendThread = AfxBeginThread(SendThread, (LPVOID)&mctParam);

        // push a notification

        btnConnect.SetWindowText(_T("Disconnect"));
        btnSend.EnableWindow(TRUE);
    }
    else {
        isConnect = FALSE;
        closesocket(sock_peer);
        sock_peer = INVALID_SOCKET;
        CloseHandle(hListenThread->m_hThread);
        hListenThread->m_hThread = NULL;
        lstMsg.ResetContent();
        btnConnect.SetWindowText(_T("Connect"));
        btnSend.EnableWindow(FALSE);
    }
    UpdateData(FALSE);
}


void CClientDlg::OnClickedButton2()
{
    UpdateData(TRUE);
    // TODO: Add your control notification handler code here
    USES_CONVERSION;
    std::string msg = CT2A(strSelfMsg);
    send(sock_peer, msg.c_str(), msg.size(), 0);
    UpdateData(FALSE);
    edSelfMsg.SetWindowText(_T(""));
}


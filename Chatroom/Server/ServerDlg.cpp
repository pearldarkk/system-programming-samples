
// ServerDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "Server.h"
#include "ServerDlg.h"
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


// CServerDlg dialog



CServerDlg::CServerDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_SERVER_DIALOG, pParent)
    , strIP(_T(""))
    , strPort(_T(""))
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BUTTON1, btnStart);
    DDX_Text(pDX, IDC_EDIT1, strIP);
    DDX_Text(pDX, IDC_EDIT2, strPort);
    DDX_Control(pDX, IDC_LIST1, lstLog);
    DDX_Control(pDX, IDC_LIST2, lstUser);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //    ON_EN_CHANGE(IDC_EDIT1, &CServerDlg::OnEnChangeEdit1)
    ON_BN_CLICKED(IDC_BUTTON1, &CServerDlg::OnClickedButton1)
//    ON_WM_SIZE()
END_MESSAGE_MAP()


// CServerDlg message handlers

BOOL CServerDlg::OnInitDialog()
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
    lstLog.InsertColumn(0, _T("Time"), LVCFMT_LEFT, 100);
    lstLog.InsertColumn(1, _T("Event"), LVCFMT_LEFT, 380);
    lstLog.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
    lstUser.InsertColumn(0, _T("IP Address"), LVCFMT_LEFT, 150);
    lstUser.InsertColumn(1, _T("Port"), LVCFMT_LEFT, 100);
    lstUser.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

    UpdateData(FALSE);

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CServerDlg::OnPaint()
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
HCURSOR CServerDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

UINT CServerDlg::MasterThread(LPVOID lpParam) {
    MultiCardThreadParam* mctParam = (MultiCardThreadParam*)lpParam;
    CServerDlg* svDlg = (CServerDlg*)mctParam->wndHandler;
    SOCKET sock_listen = (SOCKET)mctParam->lpParam;
    SYSTEMTIME time;
    CString strStatus, strTime, strError, strIP, strPort, strMsg;
    CStringA straMsg, straStatus;
    int iChat, iUser;
    char cstrIP[INET_ADDRSTRLEN];
    USES_CONVERSION;

    // manage active sockets
    fd_set master;
    FD_ZERO(&master);
    FD_SET(sock_listen, &master);
    SOCKET max_socket = sock_listen;

    while (!svDlg->stop) {
        fd_set reads;
        reads = master;
        if (select(max_socket + 1, &reads, 0, 0, 0) < 0) {
            GetLocalTime(&time);
            strTime.Format(_T("%02d:%02d:%02d"), time.wHour, time.wMinute, time.wSecond);
            iChat = svDlg->lstLog.GetItemCount();
            svDlg->lstLog.AddItem(strTime, iChat, 0);
            strError.Format(_T("Socket selecting failed with error code 0x%X"), WSAGetLastError());
            svDlg->lstLog.AddItem(strError, iChat, 1);
            return FALSE;
        }

        SOCKET iSock;
        for (iSock = 1; iSock <= max_socket; ++iSock)
            if (FD_ISSET(iSock, &reads)) {
                // accept connection, add to master
                if (iSock == sock_listen) {
                    sockaddr_in client_address;
                    socklen_t client_len = sizeof client_address;
                    SOCKET sock_client = accept(sock_listen, (sockaddr*)&client_address, &client_len);
                    if (sock_client == INVALID_SOCKET)
                        if (!svDlg->stop) {
                            GetLocalTime(&time);
                            strTime.Format(_T("%02d:%02d:%02d"), time.wHour, time.wMinute, time.wSecond);
                            iChat = svDlg->lstLog.GetItemCount();
                            svDlg->lstLog.AddItem(strTime, iChat, 0);
                            strError.Format(_T("Socket accepting failed with error code 0x%X"), WSAGetLastError());
                            svDlg->lstLog.AddItem(strError, iChat, 1);
                            return FALSE;
                        }
                        else {
                            //server closed
                            for (SOCKET jSock = 1; jSock <= max_socket; ++jSock)
                                if (FD_ISSET(jSock, &master))
                                    closesocket(jSock);
                            return FALSE;
                            //// push notifications to chatroom
                            //strStatus.Format(_T("Server "), (PCTSTR)strIP);
                            //const char* cstrStatus = (PCSTR)(PCTSTR)strStatus;
                            //for (SOCKET jSock = 1; jSock <= max_socket; ++jSock)
                            //    if (FD_ISSET(jSock, &master))
                            //        send(jSock, cstrStatus, sizeof cstrStatus, 0);
                        }

                    FD_SET(sock_client, &master);
                    if (sock_client > max_socket)
                        max_socket = sock_client;

                    inet_ntop(AF_INET, &client_address.sin_addr, cstrIP, INET_ADDRSTRLEN);
                    strIP = cstrIP;
                    strPort.Format(_T("%d"), ntohs(client_address.sin_port));

                    // update server logs
                    GetLocalTime(&time);
                    strTime.Format(_T("%02d:%02d:%02d"), time.wHour, time.wMinute, time.wSecond);
                    iChat = svDlg->lstLog.GetItemCount();
                    svDlg->lstLog.AddItem(strTime, iChat, 0);
                    strStatus.Format(_T("New connection from IP %s"), (PCTSTR)strIP);
                    svDlg->lstLog.AddItem(strStatus, iChat, 1);

                    iUser = svDlg->lstUser.GetItemCount();
                    svDlg->lstUser.AddItem(strIP, iUser, 0);
                    svDlg->lstUser.AddItem(strPort, iUser, 1);

                    // push notifications to chatroom
                    strStatus.Format(_T("IP %s joined the chat"), (PCTSTR)strIP);
                    //const char* cstrStatus = (PCSTR)(PCTSTR)strStatus;
                    for (SOCKET jSock = 1; jSock <= max_socket; ++jSock)
                        if (FD_ISSET(jSock, &master))
                            if (jSock == sock_listen)
                                continue;
                            else {
                                std::string stt = CT2A(strStatus);
                                send(jSock, stt.c_str(), (int)stt.size(), 0);
                                //send(jSock, straStatus, sizeof straStatus, 0);
                            }
                }
                // read request for an established connection and send data
                else {
                    char read[1024];
                    int bytes_received = recv(iSock, read, 1024, 0);

                    // client disconnected
                    if (bytes_received < 1) {
                        // send new log to server
                        GetLocalTime(&time);
                        strTime.Format(_T("%02d:%02d:%02d"), time.wHour, time.wMinute, time.wSecond);
                        iChat = svDlg->lstLog.GetItemCount();
                        svDlg->lstLog.AddItem(strTime, iChat, 0);
                        strStatus.Format(_T("IP %s disconnected"), (LPCTSTR)strIP);
                        svDlg->lstLog.AddItem(strStatus, iChat, 1);

                        // send notification to chat room
                        strStatus.Format(_T("IP %s left the chat"), (LPCTSTR)strIP);
                        for (SOCKET jSock = 1; jSock <= max_socket; ++jSock)
                            if (FD_ISSET(jSock, &master))
                                if (jSock == sock_listen || jSock == iSock)
                                    continue;
                                else {
                                    std::string stt = CT2A(strStatus);
                                    send(jSock, stt.c_str(), stt.size(), 0);
                                }

                        // delete from user list
                        for (int iItem = 0; iItem < svDlg->lstUser.GetItemCount(); ++iItem) {
                            if (svDlg->lstUser.GetItemText(iItem, 0) == strIP && svDlg->lstUser.GetItemText(iItem, 1) == strPort)
                                svDlg->lstUser.DeleteItem(iItem);
                        }

                        FD_CLR(iSock, &master);
                        closesocket(iSock);
                        continue;
                    }
                    // send messages to chat room
                    for (SOCKET jSock = 1; jSock <= max_socket; ++jSock)
                        if (FD_ISSET(jSock, &master))
                            if (jSock == sock_listen)
                                continue;
                    // self-message
                            else if (jSock == iSock) {
                                straMsg.Format("You: %.*s", bytes_received, read);
                                std::string msg = straMsg;
                                send(jSock, msg.c_str(), msg.size(), 0);
                            }
                    // other ip see
                            else {
                                straMsg.Format("%s: %.*s", cstrIP, bytes_received, read);
                                std::string msg = straMsg;
                                send(jSock, msg.c_str(), msg.size(), 0);
                            }
                }
            }
    }
    return TRUE;
}

SOCKET sock_listen;

void CServerDlg::OnClickedButton1()
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    SYSTEMTIME time;
    WSADATA d;
    CString strStatus, strTime, strError;
    int iLog;
    btnStart.GetWindowText(strStatus);
    // start listening 
    if (strStatus == _T("Start")) {
        memset(&d, 0, sizeof d);
        if (WSAStartup(MAKEWORD(2, 2), &d)) {
            GetLocalTime(&time);
            strTime.Format(_T("%02d:%02d:%02d"), time.wHour, time.wMinute, time.wSecond);
            iLog = lstLog.GetItemCount();
            lstLog.AddItem(strTime, iLog, 0);
            lstLog.AddItem(_T("Winsock intializing failed!"), iLog, 1);
            return;
        }

        // config server address
        addrinfo hints;
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = AI_PASSIVE;

        USES_CONVERSION;
        addrinfo* bind_address;
        std::string ip = CT2A(strIP.GetString()), port = CT2A(strPort.GetString());
        const char* cip = ip.c_str(), * cport = port.c_str();
        getaddrinfo(cip, cport, &hints, &bind_address);

        // create socket
        sock_listen = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
        if (sock_listen == INVALID_SOCKET) {
            GetLocalTime(&time);
            strTime.Format(_T("%02d:%02d:%02d"), time.wHour, time.wMinute, time.wSecond);
            iLog = lstLog.GetItemCount();
            lstLog.AddItem(strTime, iLog, 0);
            strError.Format(_T("Socket creating failed with error code 0x%X"), WSAGetLastError());
            lstLog.AddItem(strError, iLog, 1);
            return;
        }
        // bind socket to IP address
        if (bind(sock_listen, bind_address->ai_addr, bind_address->ai_addrlen)) {
            GetLocalTime(&time);
            strTime.Format(_T("%02d:%02d:%02d"), time.wHour, time.wMinute, time.wSecond);
            iLog = lstLog.GetItemCount();
            lstLog.AddItem(strTime, iLog, 0);
            strError.Format(_T("Socket binding failed with error code 0x%X"), WSAGetLastError());
            lstLog.AddItem(strError, iLog, 1);
            return;
        }
        freeaddrinfo(bind_address);

        // enter listening state
        if (listen(sock_listen, 10) < 0) {
            GetLocalTime(&time);
            strTime.Format(_T("%02d:%02d:%02d"), time.wHour, time.wMinute, time.wSecond);
            iLog = lstLog.GetItemCount();
            lstLog.AddItem(strTime, iLog, 0);
            strError.Format(_T("Listening failed with error code 0x%X"), WSAGetLastError());
            lstLog.AddItem(strError, iLog, 1);
            return;
        }

        // create master thread for server
        mctParam.wndHandler = this;
        mctParam.lpParam = (LPVOID)sock_listen;
        hMasterThread = AfxBeginThread(MasterThread, (LPVOID)&mctParam);

        // waiting for connections
        GetLocalTime(&time);
        strTime.Format(_T("%02d:%02d:%02d"), time.wHour, time.wMinute, time.wSecond);
        iLog = lstLog.GetItemCount();
        lstLog.AddItem(strTime, iLog, 0);
        lstLog.AddItem(_T("Waiting for connections..."), iLog, 1);
        btnStart.SetWindowText(_T("Stop"));
        stop = FALSE;
    }
    else {
        stop = TRUE;
        closesocket(sock_listen);
        sock_listen = INVALID_SOCKET;
        CloseHandle(hMasterThread->m_hThread);
        hMasterThread->m_hThread = NULL;
        lstUser.DeleteAllItems();

        iLog = lstLog.GetItemCount();
        GetLocalTime(&time);
        strTime.Format(_T("%02d:%02d:%02d"), time.wHour, time.wMinute, time.wSecond);
        lstLog.AddItem(strTime, iLog, 0);
        lstLog.AddItem(_T("Server closed"), iLog, 1);

        btnStart.SetWindowText(_T("Start"));
    }
    UpdateData(FALSE);
}


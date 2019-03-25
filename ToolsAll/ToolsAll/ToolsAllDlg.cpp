
// ToolsAllDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "ToolsAll.h"
#include "ToolsAllDlg.h"
#include "afxdialogex.h"
#include "CDialogOne.h"
#include "CDialogTwo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CToolsAllDlg 对话框



CToolsAllDlg::CToolsAllDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TOOLSALL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CToolsAllDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CToolsAllDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CToolsAllDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON2, &CToolsAllDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CToolsAllDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CToolsAllDlg 消息处理程序

BOOL CToolsAllDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CToolsAllDlg::OnSysCommand(UINT nID, LPARAM lParam)
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


void CToolsAllDlg::AddGroupDlg()
{
	DlgOne = new CDialogOne();
	DlgOne->Create(IDD_DIALOG_One, GetDlgItem(IDC_STATIC_GROUP));
	DlgOne->ShowWindow(SW_SHOW);

	DlgTwo = new CDialogTwo();
	DlgTwo->Create(IDD_DIALOG_Two, GetDlgItem(IDC_STATIC_GROUP));
	DlgTwo->ShowWindow(SW_HIDE);

	GetDlgItem(IDC_STATIC_GROUP)->ShowWindow(SW_SHOW);
}

void CToolsAllDlg::Init_GroupMenu() 
{
	/*
GetDlgItem(IDC_GROUP_MENU)->OnDrawClipboard();
	
	if (IDC_STATIC_GROUP == CToolsAllDlg->GetDlgCtrlID())
	{
		pDc->SetTextColor(RGB(255, 255, 255));//字体白色
		hbr = CreatSolidBrush(RGB(0, 0, 0));//背景颜色
	}
	*/
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。
void CToolsAllDlg::OnPaint()
{

	int left;
	int top;
	int right;
	int bottom;

	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;	

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}

	AddGroupDlg();	

	CRect rect;

	GetClientRect(&rect);	

	left = 20;
	top = 10;
	right = 130;
	bottom = rect.bottom - 80;

	//Btn_8583->MoveWindow(left - 130, 10, right, bottom);

	left = 150;
	top = 10;
	right = rect.right - 80;
	bottom = rect.bottom - 80;	

	GetDlgItem(IDC_STATIC_GROUP)->MoveWindow(left, top, right, bottom);

}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CToolsAllDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CToolsAllDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//CDialogEx::OnOK();
}


void CToolsAllDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_STATIC_GROUP)->ShowWindow(SW_SHOW);
	DlgOne->ShowWindow(SW_SHOW);
	DlgTwo->ShowWindow(SW_HIDE);


	


	CRect cr;

	//GetDlgItem(IDD_TOOLSALL_DIALOG)->MoveWindow(150, 0, 1100, 700);
	//GetDlgItem(IDD_TOOLSALL_DIALOG)->GetClientRect(&cr);
	//GetDlgItem(IDD_TOOLSALL_DIALOG)->ShowWindow(SW_HIDE);

	/*
	TRACE("left:%ld \r\n", cr.left);
	TRACE("top:%ld \r\n", cr.top);
	TRACE("right:%ld \r\n", cr.right);
	TRACE("bottom:%ld \r\n", cr.bottom);
	TRACE(" ---------\r\n");

	cr.left += 150;

	//GetDlgItem(IDC_STATIC_GROUP)->MoveWindow(cr.left, cr.top, cr.right, cr.bottom);
	GetDlgItem(IDC_STATIC_GROUP)->GetWindowRect(&cr);
	ClientToScreen(&cr);

	TRACE("left:%ld \r\n", cr.left);
	TRACE("top:%ld \r\n", cr.top);
	TRACE("right:%ld \r\n", cr.right);
	TRACE("bottom:%ld \r\n", cr.bottom);
	*/
}


void CToolsAllDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_STATIC_GROUP)->ShowWindow(SW_SHOW);
	DlgOne->ShowWindow(SW_HIDE);
	DlgTwo->ShowWindow(SW_SHOW);
}

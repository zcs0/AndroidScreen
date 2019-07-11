
// AndroidScreenDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "AndroidScreen.h"
#include "AndroidScreenDlg.h"
#include "afxdialogex.h"
#include "ADBUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
ADBUtils *util;


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
	util = new ADBUtils("D:\\android-sdk\\platform-tools\\adb.exe");
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAndroidScreenDlg 对话框



CAndroidScreenDlg::CAndroidScreenDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ANDROIDSCREEN_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAndroidScreenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_PIC, m_pic);
}

BEGIN_MESSAGE_MAP(CAndroidScreenDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CAndroidScreenDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CAndroidScreenDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CONTENT, &CAndroidScreenDlg::OnBnClickedContent)
END_MESSAGE_MAP()


// CAndroidScreenDlg 消息处理程序

BOOL CAndroidScreenDlg::OnInitDialog()
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
	CBitmap bitmap;  // CBitmap对象，用于加载位图   
	HBITMAP hBmp;    // 保存CBitmap加载的位图的句柄   
	bitmap.LoadBitmap(IDB_PNG1);  // 将位图IDB_BITMAP1加载到bitmap   
	hBmp = (HBITMAP)bitmap.GetSafeHandle();  // 获取bitmap加载位图的句柄   
	m_pic.SetBitmap(hBmp);    // 设置图片控件m_jzmPicture的位图图片

	util = new ADBUtils("D:\\android-sdk\\platform-tools\\adb.exe");
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CAndroidScreenDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAndroidScreenDlg::OnPaint()
{
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
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAndroidScreenDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//绘制图片
void CAndroidScreenDlg::DrowPhoto(CString filePath) {
	//对话框窗体大小及其屏幕坐标
	CRect rectDlg;
	//法1：
	GetClientRect(rectDlg);//获得窗体的大小
	//CFileDialog file(TRUE);
	//int result = file.DoModal();
	CRect rect;
	GetDlgItem(IDC_STATIC_PIC)->GetWindowRect(&rect);           //IDC_WAVE_DRAW为Picture Control的ID  
	ScreenToClient(&rect);

	//GetDlgItem(IDC_STATIC_PIC)->MoveWindow(rect.left, rect.top, rectDlg.Width(), rect.Height(), true);    //固定
	//GetDlgItem(IDC_STATIC_PIC)->MoveWindow(rect.left, rect.top, rectDlg.Width(), rectDlg.Height(), true);    //固定
	int w = rectDlg.Width() - 50;
	GetDlgItem(IDC_STATIC_PIC)->MoveWindow(0, 0, w, rectDlg.Height(), true);    //固定

	int height, width;
	CRect rect1;
	CImage image; //创建图片类
	image.Load(filePath);
	if (image == NULL) {
		return;
	}
	height = image.GetHeight();
	width = image.GetWidth();

	m_pic.GetClientRect(&rect); //获得pictrue控件所在的矩形区域
	CDC *pDc = m_pic.GetDC();//获得pictrue控件的Dc
	SetStretchBltMode(pDc->m_hDC, STRETCH_HALFTONE);


	if (width <= rect.Width() && height <= rect.Width()) //小图片，不缩放
	{
		rect1 = CRect(rect.TopLeft(), CSize(width, height));
		image.StretchBlt(pDc->m_hDC, rect1, SRCCOPY); //将图片画到Picture控件表示的矩形区域
		//return TRUE;
	}
	else
	{
		float xScale = (float)rect.Width() / (float)width;
		float yScale = (float)rect.Height() / (float)height;
		float ScaleIndex = (xScale <= yScale ? xScale : yScale);
		rect1 = CRect(rect.TopLeft(), CSize((int)width*ScaleIndex, (int)height*ScaleIndex));
		//rect1 = CRect(rect.TopLeft(), CSize((int)width, (int)height));
		image.StretchBlt(pDc->m_hDC, rect1, SRCCOPY); //将图片画到Picture控件表示的矩形区域
	}
	ReleaseDC(pDc);//释放picture控件的Dc


}

void CAndroidScreenDlg::OnBnClickedOk()

{
	//对话框窗体大小及其屏幕坐标
	CRect rectDlg;
	//法1：
	GetClientRect(rectDlg);//获得窗体的大小
	CFileDialog file(TRUE);
	int result = file.DoModal();
	CRect rect;
	GetDlgItem(IDC_STATIC_PIC)->GetWindowRect(&rect);           //IDC_WAVE_DRAW为Picture Control的ID  
	ScreenToClient(&rect);

	//GetDlgItem(IDC_STATIC_PIC)->MoveWindow(rect.left, rect.top, rectDlg.Width(), rect.Height(), true);    //固定
	//GetDlgItem(IDC_STATIC_PIC)->MoveWindow(rect.left, rect.top, rectDlg.Width(), rectDlg.Height(), true);    //固定
	int w = rectDlg.Width() - 50;
	GetDlgItem(IDC_STATIC_PIC)->MoveWindow(0, 0, w, rectDlg.Height(), true);    //固定

	if (result == IDOK)
	{

		//CDC* pDC = GetDlgItem(IDC_STATIC_PIC)->GetDC();
		CString filePath = file.GetPathName();


		int height, width;
		CRect rect;//定义矩形类
		CRect rect1;
		CImage image; //创建图片类
		image.Load(filePath);
		height = image.GetHeight();
		width = image.GetWidth();


		m_pic.GetClientRect(&rect); //获得pictrue控件所在的矩形区域
		CDC *pDc = m_pic.GetDC();//获得pictrue控件的Dc
		SetStretchBltMode(pDc->m_hDC, STRETCH_HALFTONE);


		if (width <= rect.Width() && height <= rect.Width()) //小图片，不缩放
		{
			rect1 = CRect(rect.TopLeft(), CSize(width, height));
			image.StretchBlt(pDc->m_hDC, rect1, SRCCOPY); //将图片画到Picture控件表示的矩形区域
			//return TRUE;
		}
		else
		{
			float xScale = (float)rect.Width() / (float)width;
			float yScale = (float)rect.Height() / (float)height;
			float ScaleIndex = (xScale <= yScale ? xScale : yScale);
			rect1 = CRect(rect.TopLeft(), CSize((int)width*ScaleIndex, (int)height*ScaleIndex));
			//rect1 = CRect(rect.TopLeft(), CSize((int)width, (int)height));
			image.StretchBlt(pDc->m_hDC, rect1, SRCCOPY); //将图片画到Picture控件表示的矩形区域
		}
		ReleaseDC(pDc);//释放picture控件的Dc


	}
}




void CAndroidScreenDlg::OnBnClickedCancel()
{
	CRect rect;
	GetDlgItem(IDC_STATIC_PIC)->GetWindowRect(&rect);
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}
void CAndroidScreenDlg::OnTimer(UINT_PTR nIDEvent)
{
	printf("-----");
}
void CAndroidScreenDlg::updateScreen() {
	if (util == NULL) {
		return;
	}
	BOOL b = util->connent();
	//util->Stop();
	CString str = util->GetOutput();
	//util->Stop();
	util->GetFrist(str);//第一个台手机
	CString photo = util->GetPhoto();
	//util->Stop();
	if (!photo.IsEmpty()) {
		util->isOpenInfo = false;
		DrowPhoto(photo);
		printf("-----");
	}
}
UINT ThreadFunc1(LPVOID lpParam)
{
	CAndroidScreenDlg* dlg = (CAndroidScreenDlg*)lpParam;
	printf("-----");
	while (true) {
		Sleep(200);
		dlg->updateScreen();
	}

	return 0;
}


void CAndroidScreenDlg::OnBnClickedContent()
{
	//CAndroidScreenDlg::OnTimeChange
	if (util == NULL) {
		return;
	}
	AfxBeginThread(ThreadFunc1, (LPVOID)this);
	//SetTimer(1, 1000, NULL);
	//SetTimer(1,)
	//setTime
	//while (true) {
		//util->Start();
	//BOOL b = util->connent();
	////util->Stop();
	//CString str = util->GetOutput();
	////util->Stop();
	//util->GetFrist(str);//第一个台手机
	//CString photo = util->GetPhoto();
	////util->Stop();
	//if (!photo.IsEmpty()) {
	//	//(photo);
	//	printf("-----");
	//}
	//}

	//str = util->GetOutput();
	//printf(str);
	//util->connent();



}



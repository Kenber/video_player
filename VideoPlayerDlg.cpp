// VideoPlayerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "VideoPlayer.h"
#include "VideoPlayerDlg.h"

#include <process.h>
#include <string>
#include "CvvImage.h"
#include <opencv2/opencv.hpp>

using namespace cv;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//My Code Begin

CDialog *m_this;
CSliderCtrl *m_myslider;

int frames;
double slider_position = 0.0;


int slider_max=100;
int slider_min=0;

bool openfiles=false;
bool Play = false;

string path;


void myThread(void *param)
{//多线程

	
	VideoCapture myVideo;
	while(1)
	{
		Sleep(1);		//睡眠1毫秒
		int delay=0;

		if(openfiles)
		{
			openfiles=false;
			myVideo.open(path);

			frames=(int)myVideo.get( CV_CAP_PROP_FRAME_COUNT);//获取整个帧数
			slider_max=frames;
			m_myslider->SetRange(slider_min,slider_max,TRUE);
			//m_myslider->SetTicFreq(frames/slider_max);//设置滑动条刻度的频度。默认为一个单位

			//获取帧率
			double rate = myVideo.get(CV_CAP_PROP_FPS);

			//两帧间的间隔时间: 
			delay = int(750/rate);
		}			

		

			Mat image;
			if (Play)
			{

			myVideo.set( CV_CAP_PROP_POS_FRAMES,slider_position++);//以帧为单位设置视频位置
			m_myslider->SetPos(slider_position);
			
			//myVideo.set( CV_CAP_PROP_POS_MSEC,slider_position);//以毫秒为单位设置视频位置
			Sleep(delay);//时延是通过帧率算出来的

				myVideo >> image;

				CDC* pDC = m_this->GetDlgItem( IDC_VIDEO_SHOW )->GetDC();		// 获得显示控件的 DC

				HDC hDC = pDC->GetSafeHdc();				// 获取 HDC(设备句柄) 来进行绘图操作
				CRect rect;
				m_this->GetDlgItem(IDC_VIDEO_SHOW)->GetClientRect( &rect );	// 获取控件尺寸位置
				CvvImage cimg;

				IplImage pimage = image;


				cimg.CopyOf( &pimage );						// 复制图片
				cimg.DrawToHDC( hDC, &rect );				// 将图片绘制到显示控件的指定区域内
				Sleep(delay);								//时延是通过帧率算出来的
				m_this->ReleaseDC( pDC );
			}
		}

		
		
}



//My Code End



// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CVideoPlayerDlg 对话框




CVideoPlayerDlg::CVideoPlayerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVideoPlayerDlg::IDD, pParent)
	
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVideoPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER, m_slider);
	DDX_Control(pDX, IDC_BUTTON_OPENFILES, m_openfiles);
	
}

BEGIN_MESSAGE_MAP(CVideoPlayerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &CVideoPlayerDlg::OnBnClickedButtonPlay)//映射。将PLAY按钮（其ID为IDC_BUTTON_PLAY）与OnBnClickedButtonPlay函数关联
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, &CVideoPlayerDlg::OnBnClickedButtonPause)
	ON_BN_CLICKED(IDCANCEL, &CVideoPlayerDlg::OnBnClickedCancel)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER, &CVideoPlayerDlg::OnNMCustomdrawSlider)
	ON_BN_CLICKED(IDC_BUTTON_OPENFILES, &CVideoPlayerDlg::OnBnClickedButtonOpenfiles)
END_MESSAGE_MAP()


// CVideoPlayerDlg 消息处理程序

BOOL CVideoPlayerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_this = this;  //指针外用

	_beginthread(myThread, 0, 0);//开辟一个线程来处理

	m_myslider = &m_slider;

	m_slider.SetRange(slider_min,slider_max,TRUE);//设置滑块位置的最大值和最小值
	m_slider.SetPos(0);//设置滑块的默认当前位置
	m_slider.SetTicFreq(1);//设置滑动条刻度的频度。默认为一个单位

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CVideoPlayerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CVideoPlayerDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CVideoPlayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CVideoPlayerDlg::OnBnClickedButtonPlay()
{
	// TODO: 在此添加控件通知处理程序代码
	Play=true;
}

void CVideoPlayerDlg::OnBnClickedButtonPause()
{
	// TODO: 在此添加控件通知处理程序代码
	Play=false;
}

void CVideoPlayerDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

void CVideoPlayerDlg::OnNMCustomdrawSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);

	// TODO: 在此添加控件通知处理程序代码
	slider_position=m_slider.GetPos();
	//slider=true;

	*pResult = 0;
}

void CVideoPlayerDlg::OnBnClickedButtonOpenfiles()
{
	// TODO: 在此添加控件通知处理程序代码
	
	CFileDialog dlg(TRUE);
		if(dlg.DoModal()==IDOK)
		{
			CString FilePathName;
			FilePathName=dlg.GetPathName();

			//CString到string的转换
			CStringA stra(FilePathName.GetBuffer(0));
			FilePathName.ReleaseBuffer();
			path=stra.GetBuffer(0);
			stra.ReleaseBuffer();

			openfiles=true;
		}
			
}



// ScommAssistantDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ScommAssistant.h"
#include "ScommAssistantDlg.h"
#include "stdlib.h"
#include "math.h"
#include "assert.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScommAssistantDlg dialog

CScommAssistantDlg::CScommAssistantDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScommAssistantDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScommAssistantDlg)
	m_strRXData = _T("");
	m_strTXData = _T("");
	m_ctrlHexSend = FALSE;
	m_Period = _T("");
	m_Path = _T("");
	m_strEdata = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_fWidthMul=m_fHeightMul=0;
	m_flag=0;
	m_flag_stopdisplay=0;
	m_bAutoSend=FALSE;
	change_flag=FALSE;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CScommAssistantDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScommAssistantDlg)
	DDX_Control(pDX, IDC_BtnOpenLock, m_BtnOpenLock);
	DDX_Control(pDX, IDC_BUTTON_STOP_DISPAL, m_StopDisplay);
	DDX_Control(pDX, IDC_CHECK_HEXDISPLAY, m_ctrlHexDiaplay);
	DDX_Control(pDX, IDC_BUTTON_CLOSE_SERIALPORT, m_SerialPort_CloseStart);
	DDX_Control(pDX, IDC_COMBO_BATE, m_Bate);
	DDX_Control(pDX, IDC_COMBO_STOP, m_Stop);
	DDX_Control(pDX, IDC_COMBO_DATA, m_Data);
	DDX_Control(pDX, IDC_COMBO_CHECKOUT, m_Checkout);
	DDX_Control(pDX, IDC_COMBO_SERIALPORT, m_SerialPort);
	DDX_Control(pDX, IDC_MSCOMM1, m_ctrlComm);
	DDX_Text(pDX, IDC_EDIT_RX, m_strRXData);
	DDX_Text(pDX, IDC_EDIT_TX, m_strTXData);
	DDX_Check(pDX, IDC_CHECK_HEXSEND, m_ctrlHexSend);
	DDX_Text(pDX, IDC_EDIT_PERIOD, m_Period);
	DDX_Text(pDX, IDC_EDIT_PATH, m_Path);
	DDX_Text(pDX, IDC_EditLockAdress, m_strEdata);
	DDV_MinMaxByte(pDX, m_strEdata, 0, 255);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CScommAssistantDlg, CDialog)
	//{{AFX_MSG_MAP(CScommAssistantDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBO_SERIALPORT, OnSelchangeComboSerialport)
	ON_CBN_SELCHANGE(IDC_COMBO_BATE, OnSelchangeComboBate)
	ON_CBN_SELCHANGE(IDC_COMBO_CHECKOUT, OnSelchangeComboCheckout)
	ON_CBN_SELCHANGE(IDC_COMBO_DATA, OnSelchangeComboData)
	ON_CBN_SELCHANGE(IDC_COMBO_STOP, OnSelchangeComboStop)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE_SERIALPORT, OnButtonCloseSerialport)
	ON_BN_CLICKED(IDC_BUTTON_MANUALSEND, OnButtonManualsend)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_AUTOSEND, OnCheckAutosend)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_RX, OnButtonClearRx)
	ON_BN_CLICKED(IDC_BUTTON_STOP_DISPAL, OnButtonStopDispal)
	ON_BN_CLICKED(IDC_BUTTON_CLOSEPROGRAMME, OnButtonCloseprogramme)
	ON_BN_CLICKED(IDC_BUTTON_AFRESH, OnButtonAfresh)
	ON_BN_CLICKED(IDC_BUTTON_CHANGPATH, OnButtonChangpath)
	ON_BN_CLICKED(IDC_BUTTON_SAVEDATA, OnButtonSavedata)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BtnOpenLock, OnBtnOpenLock)
	ON_BN_CLICKED(IDC_BtnEditLockAdress, OnBtnEditLockAdress)
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(IDC_EditLockAdress, OnChangeEditLockAdress)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScommAssistantDlg message handlers

BOOL CScommAssistantDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
    //��ȡδ�Ŵ�ǰ�Ի����С 
    long m_nDlgWidth ,m_nDlgHeight,m_nWidth,m_nHeight;
	
    CRect rect; 
    ::GetWindowRect(m_hWnd,rect); 

	ScreenToClient(rect); 
    m_nDlgWidth = rect.right - rect.left; 
    m_nDlgHeight = rect.bottom - rect.top; 

	//������� 
    // ShowWindow(SW_MAXIMIZE); 	
	//����ֱ���
    m_nWidth = GetSystemMetrics(SM_CXSCREEN); 
    m_nHeight = GetSystemMetrics(SM_CYSCREEN); 
    
	//����Ŵ���
    m_fWidthMul = float(m_nWidth)/float(m_nDlgWidth); 
    m_fHeightMul = float(m_nHeight)/float(m_nDlgHeight);
	change_flag=TRUE;    //����ǳ�Ա����bool�Σ������ж�onsizeִ��ʱoninitdlg�Ƿ��Ѿ�ִ����
    


     m_Path="E:\\���ڵ�����Ϣ\\ScommAssistant.txt";
     m_Period="1000";  //��ʼ���Զ����͵�����Ϊ1000����
	 UpdateData(FALSE);

	if(m_flag==0)  //��һ�γ�ʼ������������ʾ��ʼֵ
    {
		m_SerialPort.SetCurSel(0);//COM1
	    m_Bate.SetCurSel(5); //9600 bit/s
	    m_Checkout.SetCurSel(0);  //��У��λ(NONE)
	    m_Data.SetCurSel(0); //8Ϊ����λ
	    m_Stop.SetCurSel(0); //1Ϊֹͣλ
    }
	CString c;
    int index = m_SerialPort.GetCurSel();
    m_SerialPort.GetLBText(index,c);//��ȡѡ�����ֵ 

	if(m_ctrlComm.GetPortOpen())
		m_ctrlComm.SetPortOpen(FALSE); 

    m_ctrlComm.SetCommPort(index+1);  

    if( !m_ctrlComm.GetPortOpen())
		m_ctrlComm.SetPortOpen(TRUE);//�򿪴���
    else
		AfxMessageBox("cannot open serial port");

	CString str_bate,str_data,str_stop,str_checkout;
	
	index=m_Bate.GetCurSel();
	m_Bate.GetLBText(index,str_bate);

   	index=m_Data.GetCurSel();
	m_Data.GetLBText(index,str_data);
	
	index=m_Stop.GetCurSel();
	m_Stop.GetLBText(index,str_stop);

	index=m_Checkout.GetCurSel();

	int int_bate,int_data,int_stop;
	char char_checkout;
	CString s;
	int_bate=atoi(str_bate);
	int_data=atoi(str_data);
	int_stop=atoi(str_stop);
	switch(index) //��ʱindex=��żλ���������
    {
	   case 0:char_checkout='n';
		   break;
       case 1:char_checkout='O';
		   break;
       default:char_checkout='E';
	}
	s.Format("%d,%c,%d,%d",int_bate,char_checkout,int_data,int_stop);
	m_ctrlComm.SetSettings(s);  //����:������9600����У�飬8������λ��1��ֹͣλ 

    m_ctrlComm.SetInputMode(1); //1����ʾ�Զ����Ʒ�ʽ��ȡ����
    m_ctrlComm.SetRThreshold(1); 
    //����1��ʾÿ�����ڽ��ջ��������ж��ڻ����1���ַ�ʱ������һ���������ݵ�OnComm�¼�
    m_ctrlComm.SetInputLen(0); //���õ�ǰ���������ݳ���Ϊ0
    m_ctrlComm.GetInput();//��Ԥ���������������������

	m_brush.CreateSolidBrush(RGB(192,220,192));//��ʼ��m_brush
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CScommAssistantDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CScommAssistantDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CScommAssistantDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BEGIN_EVENTSINK_MAP(CScommAssistantDlg, CDialog)
    //{{AFX_EVENTSINK_MAP(CScommAssistantDlg)
	ON_EVENT(CScommAssistantDlg, IDC_MSCOMM1, 1 /* OnComm */, OnComm, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CScommAssistantDlg::OnComm() 
{
	// TODO: Add your control notification handler code here
	VARIANT variant_inp;
    COleSafeArray safearray_inp;
    LONG len,k;
    BYTE rxdata[2048]; //����BYTE���� An 8-bit integerthat is not signed.
    CString strtemp;
    if(m_ctrlComm.GetCommEvent()==2) //�¼�ֵΪ2��ʾ���ջ����������ַ�
    {             ////////��������Ը����Լ���ͨ��Э����봦�����
        variant_inp=m_ctrlComm.GetInput(); //��������
        safearray_inp=variant_inp; //VARIANT�ͱ���ת��ΪColeSafeArray�ͱ���
        len=safearray_inp.GetOneDimSize(); //�õ���Ч���ݳ���
        for(k=0;k<len;k++)
            safearray_inp.GetElement(&k,rxdata+k);//ת��ΪBYTE������
        for(k=0;k<len;k++) //������ת��ΪCstring�ͱ���
        {
            BYTE bt=*(char*)(rxdata+k); //�ַ���
			if(IsDlgButtonChecked(IDC_CHECK_HEXDISPLAY))
				strtemp.Format("%02X ",bt); //���ַ���ʮ�����Ʒ�ʽ������ʱ����strtemp��ţ�ע���������һ���ո�
            else 
				strtemp.Format("%c",bt); //���ַ�������ʱ����strtemp���

            m_strRXData+=strtemp; //������ձ༭���Ӧ�ַ��� 
        }
    }
    if(m_flag_stopdisplay==0)
    {
		UpdateData(FALSE); //���±༭������	
    }
}

void CScommAssistantDlg::OnSelchangeComboSerialport() 
{
	// TODO: Add your control notification handler code here
	m_flag=1; //�رյ�һ�γ�ʼ��
	CScommAssistantDlg::OnInitDialog();	
}

void CScommAssistantDlg::OnSelchangeComboBate() 
{
	// TODO: Add your control notification handler code here
	m_flag=1; //�رյ�һ�γ�ʼ��
	CScommAssistantDlg::OnInitDialog();	
}

void CScommAssistantDlg::OnSelchangeComboCheckout() 
{
	// TODO: Add your control notification handler code here
	m_flag=1; //�رյ�һ�γ�ʼ��
	CScommAssistantDlg::OnInitDialog();	
}

void CScommAssistantDlg::OnSelchangeComboData() 
{
	// TODO: Add your control notification handler code here
	m_flag=1; //�رյ�һ�γ�ʼ��
	CScommAssistantDlg::OnInitDialog();	
}

void CScommAssistantDlg::OnSelchangeComboStop() 
{
	// TODO: Add your control notification handler code here
	m_flag=1; //�رյ�һ�γ�ʼ��
	CScommAssistantDlg::OnInitDialog();	
}

void CScommAssistantDlg::OnButtonCloseSerialport() 
{
	// TODO: Add your control notification handler code here
    if(m_ctrlComm.GetPortOpen()) //������ڴ�
    {
		m_ctrlComm.SetPortOpen(FALSE); //�رմ���
		m_SerialPort_CloseStart.SetWindowText("�򿪴���");
	} 	
	else
    {
       	m_ctrlComm.SetPortOpen(TRUE);  //�򿪴���
		m_SerialPort_CloseStart.SetWindowText("�رմ���");
	}
}

void CScommAssistantDlg::OnButtonManualsend() 
{
	// TODO: Add your control notification handler code here


	UpdateData(TRUE); //��ȡ�༭������
    if(IsDlgButtonChecked(IDC_CHECK_HEXSEND))
	{
		CByteArray hexdata;
        int len=String2Hex(m_strTXData,hexdata); //�˴����ص�len�������ڼ��㷢���˶��ٸ�ʮ��������
        m_ctrlComm.SetOutput(COleVariant(hexdata)); //����ʮ����������
	}
    else 
		m_ctrlComm.SetOutput(COleVariant(m_strTXData));//����ASCII�ַ�����
}

//�������ת�������ĸ�ʽ���ƣ��ڷ��Ϳ��е�ʮ�����ַ�Ӧ��ÿ�����ַ�֮�����һ���ո�
//�磺A1 23 45 0B 00 29
//CByteArray��һ����̬�ֽ����飬�ɲο�MSDN����
int CScommAssistantDlg::String2Hex(CString str, CByteArray &senddata)
{
   int hexdata,lowhexdata;
   int hexdatalen=0;
   int len=str.GetLength();
   senddata.SetSize(len/2);
   for(int i=0;i<len;)
   {
	char lstr,hstr=str[i];
    if(hstr==' ')
	{
		i++;
       continue;
	}
    i++;
    if(i>=len)
		break;
    lstr=str[i];
    hexdata=ConvertHexChar(hstr);
    lowhexdata=ConvertHexChar(lstr);
    if((hexdata==16)||(lowhexdata==16))
		break;
    else 
		hexdata=hexdata*16+lowhexdata;
    i++;
    senddata[hexdatalen]=(char)hexdata;
    hexdatalen++;
   }
   senddata.SetSize(hexdatalen);
   return hexdatalen;
}

//����һ�����ַ�ת��Ϊ��Ӧ��ʮ������ֵ�ĺ���
//�ö�C�������϶������ҵ�
//���ܣ�������0-F֮����ַ�����ת��Ϊ��Ӧ��ʮ�������ַ������򷵻�-1
char CScommAssistantDlg::ConvertHexChar(char ch) 
{
if((ch>='0')&&(ch<='9'))
return ch-0x30;
else if((ch>='A')&&(ch<='F'))
return ch-'A'+10;
else if((ch>='a')&&(ch<='f'))
return ch-'a'+10;
else return (-1);
}

void CScommAssistantDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	OnButtonManualsend();
	CDialog::OnTimer(nIDEvent);
}

void CScommAssistantDlg::OnCheckAutosend() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	
	int int_Period;
	int_Period=atoi(m_Period);
	m_bAutoSend=!m_bAutoSend;
   if(m_bAutoSend)
   {
	SetTimer(1,int_Period,NULL);//ʱ��Ϊ��IDC_EDIT_PERIOD�༭�����(����)
   }
   else
   {
	KillTimer(1);  //ȡ����ʱ
   }
}

void CScommAssistantDlg::OnButtonClearRx() 
{
	// TODO: Add your control notification handler code here
    m_strRXData=" ";   //��ս�����
	UpdateData(FALSE);
}

void CScommAssistantDlg::OnButtonStopDispal() 
{
	// TODO: Add your control notification handler code here
	if(m_flag_stopdisplay==0)
    {
		m_flag_stopdisplay=1;
        m_StopDisplay.SetWindowText("������ʾ");
	}
	else
    {
		m_flag_stopdisplay=0;
        m_StopDisplay.SetWindowText("ֹͣ��ʾ");
	}

}

void CScommAssistantDlg::OnButtonCloseprogramme()  //�رճ���
{
	// TODO: Add your control notification handler code here
	EndDialog(IDCANCEL);	
}

void CScommAssistantDlg::OnButtonAfresh() 
{
	// TODO: Add your control notification handler code here
    m_strTXData=" ";  //��շ�����
 	UpdateData(FALSE);
	
}

void CScommAssistantDlg::OnButtonChangpath() 
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE); //TRUEΪOPEN�Ի���FALSEΪSAVE AS�Ի���
	if(dlg.DoModal()==IDOK)
		m_Path=dlg.GetPathName();
    UpdateData(FALSE);	
}

void CScommAssistantDlg::OnButtonSavedata() 
{
	// TODO: Add your control notification handler code here
    FILE *fp = NULL; 
    fp = fopen(m_Path, "w"); 
    if(NULL == fp) 
	{ 
		MessageBox("Can't create output file!"); 
        return; 
	} 

    fwrite(m_strRXData, strlen(m_strRXData), 1, fp); 

    fclose(fp);
}




void CScommAssistantDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (change_flag)//���ȷ��oninitdlg�Ѿ��������.
	{
	/*	ReSize(IDC_COMBO_SERIALPORT);
		ReSize(IDC_COMBO_BATE);
		ReSize(IDC_COMBO_CHECKOUT);
		ReSize(IDC_COMBO_DATA);
		ReSize(IDC_COMBO_STOP);*/
	    ReSize(IDC_EDIT_RX);
      /*  ReSize(IDC_BUTTON_CLOSE_SERIALPORT);
        ReSize(IDC_BUTTON_CLEAR_RX);
        ReSize(IDC_STATIC1);
		ReSize(IDC_STATIC2);
		ReSize(IDC_STATIC3);
		ReSize(IDC_STATIC4);
        ReSize(IDC_STATIC5);*/
		ReSize(IDC_STATIC6);
		ReSize(IDC_STATIC7);
	/*	ReSize(IDC_STATIC8);
		ReSize(IDC_BUTTON_STOP_DISPAL);*/
       // ReSize(IDC_BUTTON_CLOSE_SERIALPORT);
        /*ReSize(IDC_CHECK_AUTOCLEAR);
        ReSize(IDC_CHECK_HEXDISPLAY);
		ReSize(IDC_BUTTON_SAVEDATA);
        ReSize(IDC_BUTTON_CHANGPATH);
        ReSize(IDC_EDIT_PATH);
        ReSize(IDC_BUTTON_AFRESH);
    	ReSize(IDC_CHECK_HEXSEND);
        ReSize(IDC_BUTTON_MANUALSEND);
        ReSize(IDC_CHECK_AUTOSEND);
        ReSize(IDC_EDIT_PERIOD);*/
		ReSize(IDC_EDIT_TX);
        ReSize(IDC_BUTTON_CHOOSEFILE);
        ReSize(IDC_EDIT_FILESTATUS);
        ReSize(IDC_BUTTON_SENDFILE);
		ReSize(IDC_BUTTON_CLOSEPROGRAMME);
        ReSize(IDC_MSCOMM1);
		
        //�ָ��Ŵ����������� (ȷ����ԭʱ���ܹ���ԭ��ԭ���Ĵ�С) 

		m_fWidthMul= float(1)/m_fWidthMul ; 
		m_fHeightMul= float(1)/m_fHeightMul ; 
	}
	
}


void CScommAssistantDlg::ReSize(int nID)
{
	CRect Rect; 
	GetDlgItem(nID)->GetWindowRect(Rect); 
	ScreenToClient(Rect); 
	//����ؼ����Ͻǵ� 
	CPoint OldTLPoint,TLPoint; 
	OldTLPoint = Rect.TopLeft(); 
	TLPoint.x = long(OldTLPoint.x *m_fWidthMul); 
	TLPoint.y = long(OldTLPoint.y *m_fHeightMul); 

	//����ؼ����½ǵ�
	CPoint OldBRPoint,BRPoint; 
	OldBRPoint = Rect.BottomRight(); 
	BRPoint.x = long(OldBRPoint.x *m_fWidthMul); 
	BRPoint.y = long(OldBRPoint.y *m_fHeightMul); 
	//�ƶ��ؼ����¾���

	Rect.SetRect(TLPoint,BRPoint); 
	GetDlgItem(nID)->MoveWindow(Rect,TRUE); 
}

HBRUSH CScommAssistantDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
    //�����һ���Ƿ�Ϊ�Ի�����ж����
	if(nCtlColor ==CTLCOLOR_DLG)
		return m_brush; 
    return hbr;
}

char byteToChar(byte b[])
{ 
    char c = (char) (((b[0] & 0xFF) << 8) | (b[1] & 0xFF)); 
    return c; 
}

int hex_char_value(char c)     
{     
    if(c >= '0' && c <= '9')     
        return c - '0';     
    else if(c >= 'a' && c <= 'f')     
        return (c - 'a' + 10);     
    else if(c >= 'A' && c <= 'F')     
        return (c - 'A' + 10);     
//    assert(0);     
    return 0;     
} 
    
int hex_to_decimal(const char* szHex, int len)     
{     
    int result = 0;     
    for(int i = 0; i < len; i++)     
    {     
        result += (int)pow((float)16, (int)len-i-1) * hex_char_value(szHex[i]);     
    }     
    return result;     
}


void ConvertLongToHexString(int Input,char* output)
{
    char *dest=output;
    assert(output!=NULL);
    while(Input)
    {
        if(Input%16>10)
        {
            *dest++='A'+Input%16-10;
        }
        else
        {
            *dest++=Input%16+'0';
        }
        Input=Input/16;
    }
    *dest++='x';
    *dest++='0';
    *dest='\0';
    int i=0,j=strlen(output)-1;
    while(i<j)
    {
        char temp;
        temp=output[i];
        output[i]=output[j];
        output[j]=temp;
        i++;
        j--;
    }
}


void CScommAssistantDlg::OnBtnOpenLock()//���02 55 01 23 75 ����
{
	// TODO: Add your control notification handler code here
	int i, count;
	CByteArray hexdata;
	BYTE SendBuf[4];  //�����ֽ�����
		
	count = 5;
	hexdata.RemoveAll();//�ֽ��������
	hexdata.SetSize(count);//�趨ά��
		
	SendBuf[0] = 0x01;
    SendBuf[1] = 0x88;
	SendBuf[2] = 0x01;
	SendBuf[3] = 0x23;
	SendBuf[4] = 0xAB;
		
	for(i = 0; i < count; i ++)
	{
		hexdata.SetAt(i, SendBuf[i]);//��hexdata��ֵ
	}
    m_ctrlComm.SetOutput(COleVariant(hexdata)); //����ʮ����������,����SetOutput�����Ĳ���ΪVARIANT�ͣ�����ǿ��ת������ܷ���
}

void CScommAssistantDlg::OnBtnEditLockAdress() 
{
	// TODO: Add your control notification handler code here
	int i, count;
	CByteArray hexdata;
	BYTE SendBuf[5];  //�����ֽ�����
	//BYTE m_byte;
		
	count = 6;
	hexdata.RemoveAll();//�ֽ��������
	hexdata.SetSize(count);//�趨ά��
		
	SendBuf[0] = 0x01;
    SendBuf[1] = 0x88;
	SendBuf[2] = 0x01;
	SendBuf[3] = 0x24;
//	ConvertLongToHexString(m_strEdata,m_byte);
	SendBuf[4] = m_strEdata;
//	SendBuf[4] = byteToChar((BYTE*)m_strEdata.GetBuffer(m_strEdata.GetLength()));
//	SendBuf[4] = c10_c16(m_strEdata);
	SendBuf[5] = 0xAC ^ SendBuf[4];

	for(i = 0; i < count; i ++)
	{
		hexdata.SetAt(i, SendBuf[i]);//��hexdata��ֵ
	}
    m_ctrlComm.SetOutput(COleVariant(hexdata)); //����ʮ����������,����SetOutput�����Ĳ���ΪVARIANT�ͣ�����ǿ��ת������ܷ���
}




void CScommAssistantDlg::OnChangeEditLockAdress() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE); 
//	m_strEdata = GetDlgItem(IDC_EditLockAdress);
	UpdateData(FALSE);
	
}

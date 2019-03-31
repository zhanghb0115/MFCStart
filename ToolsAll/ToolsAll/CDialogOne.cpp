#include "stdafx.h"
#include "CDialogOne.h"
#include "afxdialogex.h"
#include "ToolsAllDlg.h"
#include "Resource.h"
#include "TLVPackage.h"
#include <tchar.h>




CDialogOne::CDialogOne()
{
}


CDialogOne::~CDialogOne()
{
}

void CDialogOne::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogOne, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_CHANGETLV, &CDialogOne::OnBnClickedButton_CHANGETLV)
	ON_BN_CLICKED(IDC_TLV_CLEAR, &CDialogOne::OnBnClickedTlvClear)
END_MESSAGE_MAP()

void CDialogOne::OnBnClickedButton_CHANGETLV()
{
	// TODO: 在此添加控件通知处理程序代码
	unsigned char TempData[1024];
	unsigned char TlvInputData[1024];
	TCHAR IEDIT_CH[1024];
	CString csTring;
	CString strErr;

	//GetDlgItemText(IDC_TLV_INPUTDATA, TlvInputData);
	CString x;
	//GetDlgItem(IDC_TLV_INPUTDATA)->GetWindowText(IEDIT_CH);
	GetDlgItem(IDC_TLV_INPUTDATA)->GetWindowText(IEDIT_CH, 1024);
	csTring = IEDIT_CH;
	memset(TlvInputData, 0x00, sizeof(TlvInputData));
	memset(TempData, 0x00, sizeof(TempData));

	USES_CONVERSION;
	memcpy(TempData, T2A(csTring.GetBuffer(0)), csTring.GetLength());
	csTring.ReleaseBuffer();

	if (csTring.GetLength() == 0)
	{
		strErr.Format(_T("输入数据空\n"));
		MessageBox(strErr, _T("提示"));
		return;
	}

	if (TLVPackage::CONV_AscBcd(TlvInputData, TempData, csTring.GetLength()) == -1)
	{		
		strErr.Format(_T("请输入正确的数据\n"));
		MessageBox(strErr, _T("提示"));
		return;
	 }

	TLVEntity tlvEntity[100];
	TLVEntity tlvEntity_change[100];
	unsigned int tlv_count=100;
	memset(tlvEntity, 0x00, sizeof(tlvEntity));
	memset(tlvEntity_change, 0x00, sizeof(tlvEntity_change));

#if 0
	unsigned char requestBuf[] = {
			0x9F, 0x1C, 0x12, 0x33, 0x33, 0x30, 0x32, 0x32, 0x37, 0x31, 0x39, 0x36, 0x32, 0x30, 0x34, 0x30, 0x34,
			0x32, 0x37, 0x31, 0x38, 0x9F, 0x62, 0x01, 0x01, 0x57, 0x12, 0x62, 0x22, 0x89, 0x00, 0x00, 0x02, 0x91,
			0x01, 0xD0, 0x90, 0x32, 0x01, 0x02, 0x47, 0x17, 0x13, 0x00, 0x0F, 0x5F, 0x20, 0x0A, 0x48, 0x55, 0x47,
			0x55, 0x4F, 0x20, 0x4D, 0x49, 0x4E, 0x47, 0x9F, 0x1F, 0x3C, 0x25, 0x39, 0x39, 0x36, 0x32, 0x32, 0x32,
			0x38, 0x39, 0x30, 0x30, 0x30, 0x30, 0x30, 0x32, 0x39, 0x31, 0x30, 0x31, 0x5E, 0x47, 0x55, 0x4F, 0x20,
			0x4D, 0x49, 0x4E, 0x47, 0x2F, 0x48, 0x55, 0x5E, 0x30, 0x39, 0x30, 0x33, 0x32, 0x30, 0x31, 0x30, 0x32,
			0x34, 0x37, 0x31, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x32, 0x38, 0x39, 0x30,
			0x30, 0x3F
	};
#endif

	TLVPackage::Construct(TlvInputData, csTring.GetLength()/2, tlvEntity, tlv_count, 0);

	int var_i = 0;
	int var_j = 0;
	unsigned char DispChar[1024];
	int CharIndex = 0;
	int TempLeng = 0;
	memset(DispChar, '-', sizeof(DispChar));
	memcpy(&DispChar[CharIndex], "\r\n", 2);
	CharIndex += 2;
	for (var_i = 0; var_i <= tlv_count; var_i++) {
		TLVPackage::CONV_BcdAsc(&DispChar[CharIndex], tlvEntity[var_i].Tag, tlvEntity[var_i].TagSize);
		TempLeng = tlvEntity[var_i].TagSize * 2;
		CharIndex += (TempLeng >10? TempLeng:10);

		TLVPackage::CONV_BcdAsc(&DispChar[CharIndex], tlvEntity[var_i].Value, tlvEntity[var_i].ValueLength);
		CharIndex += tlvEntity[var_i].ValueLength*2;
		memcpy(&DispChar[CharIndex], "\r\n", 2);
		CharIndex += 2;
	}
	DispChar[CharIndex] = 0x00;
	TCHAR chRtn[1024] = {0};
	//strcpy((char *)chRtn, (char *)DispChar);
	MultiByteToWideChar(CP_ACP, 0, (char *)DispChar, -1, chRtn, 1024);
	GetDlgItem(IDC_TLV_OUTPUTDATA)->SetWindowTextW(chRtn);
	TRACE("%s", DispChar);
}

void CDialogOne::OnBnClickedTlvClear()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_TLV_OUTPUTDATA)->SetWindowTextW(NULL);
	GetDlgItem(IDC_TLV_INPUTDATA)->SetWindowTextW(NULL);
}

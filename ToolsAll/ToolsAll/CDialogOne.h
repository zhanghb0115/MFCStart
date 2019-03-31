#pragma once
#include <afxdialogex.h>
class CDialogOne :
	public CDialogEx
{
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV Ö§³Ö

	DECLARE_MESSAGE_MAP();

public:
	CDialogOne();
	~CDialogOne();	
	afx_msg void OnBnClickedButton_CHANGETLV();
	afx_msg void OnBnClickedTlvClear();
};


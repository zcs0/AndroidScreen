#pragma once
#include <WinDef.h>
#include <atlstr.h>
class ADBUtils
{
public:
	ADBUtils(char *adb);
	~ADBUtils();
	virtual BOOL connent();
	BOOL execute(wchar_t adb[],wchar_t parameter[]);

	//����adb shell������ɹ������#״̬������ʹ��RunCmdִ������
	BOOL Start();

	//�������������adb shell״̬�µ��������������ж�������\n�ָ�
	BOOL RunCmd(const CString&strCmdline);

	//�˳�shell����״̬���رս���
	BOOL Stop();

	//��ȡ������,����ǰ����ص���Stop�Ƚ���
	CString GetOutput();
	BOOL Loop();
	CString GetFrist(CString str);//������ӵ��Եı�ʶ��һ��device
	CString phontDevice=NULL;

	CString GetPhoto();//�ֻ���Ļ��ͼ·��

	BOOL isOpenInfo = TRUE;//�رջ�ȡ���


private:
	char* adbPath=NULL;
	HANDLE	hThread;
	CString m_strOutput;
	//HANDLE	m_hProcess;

	//ָʾ�߳�׼���ã����Խ�������������ź���
	HANDLE	m_hEvent;

	HANDLE	m_hReadPipe;
	HANDLE	m_hReadPipe2;
	HANDLE	m_hWritePipe;
	HANDLE	m_hWritePipe2;

};


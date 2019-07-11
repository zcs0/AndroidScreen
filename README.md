# AndroidScreen

项目使用的VS2017 MFC开发
通adb进行调取屏幕截图，由MFC显示(异步)
此方法画面速度
测试需要修改两处
CAndroidScreenDlg.cpp
  util = new ADBUtils("D:\\android-sdk\\platform-tools\\adb.exe");//adb路径

和
CString ADBUtils::GetPhoto()方法
  d:\\2019\\0101.png  //临时图片保存位置

其中
CAndroidScreenDlg.cpp
UINT ThreadFunc1(LPVOID lpParam);
//刷显示的图片线程
while (true) {
		Sleep(200);
		dlg->updateScreen();
	}




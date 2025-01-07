
#include <Windows.h>
#include <iostream>
#include <thread>

#include "GUI/Overlay.h"
#include "background/entry.h"


// Camel case : 變數、檔名
// Upper Camal Case : 物件、function
// Snake case、全大寫 : 全域變數
// 全大寫 : 專有名詞、檔案名稱

int main() {

	// ==========  Overlay  ==========
	std::thread GUI([&]() { OverlayMgr.Setup(); });
	std::thread background([&]() { Background.SetUp(); });	
	GUI.join();
	background.join();

	std::cout << "\n[ All Execution Completed : Safe Exit ]\n";
	system("pause");
}

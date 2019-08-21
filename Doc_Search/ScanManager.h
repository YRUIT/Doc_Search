#pragma once
#include "Common.h"
#include "DataManager.h"

//整个程序只有一个扫描模块，设计成单例模式
class ScanManager {
public:
	
	void Scan(const string& path);

	void StartScan() {
		while (1) {
			Scan("D:");
			std::this_thread::sleep_for(std::chrono::seconds(5));
		}
	}

	static ScanManager* CreateInstance() {
		static ScanManager scanmgr;
		static std::thread thr(&StartScan, &scanmgr);
		thr.detach();
		return &scanmgr;
	}

private:

	ScanManager() {
		//_datamgr.Init();
	}
	ScanManager(const ScanManager&);

	//DataManager _datamgr;

	//vector<string> entrys;
};


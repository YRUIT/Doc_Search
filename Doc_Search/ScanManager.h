#pragma once
#include "Common.h"
#include "DataManager.h"

//��������ֻ��һ��ɨ��ģ�飬��Ƴɵ���ģʽ
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


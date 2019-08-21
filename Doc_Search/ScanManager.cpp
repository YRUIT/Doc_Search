#include "ScanManager.h"

void ScanManager::Scan(const string& path) {
	vector<string> localdirs;
	vector<string> localfiles;
	DirectoryList(path, localdirs, localfiles);
	std::set<string> localset;
	localset.insert(localfiles.begin(), localfiles.end());
	localset.insert(localdirs.begin(), localdirs.end());

	std::set<string> dbset;

	DataManager::GetInstance()->GetDoc(path, dbset);
	auto localit = localset.begin();
	auto dbit = dbset.begin();
	while (localit != localset.end() && dbit != dbset.end()){
		if (*localit < *dbit) {
			//本地有，数据库没有
			DataManager::GetInstance()->InsertDoc(path, *localit);
			++localit;
		}
		else if (*localit > *dbit) {
			//本地没有，数据库有
			DataManager::GetInstance()->DeleteDoc(path, *dbit);
			++dbit;
		}
		else {
			++localit;
			++dbit;
		}
	}
	while (localit != localset.end()) {
		//新增数据
		DataManager::GetInstance()->InsertDoc(path, *localit);
		++localit;
	}
	while (dbit != dbset.end()) {
		//删除数据
		DataManager::GetInstance()->DeleteDoc(path, *dbit);
		++dbit;
	}
	//递归扫描对比子目录数据
	for (const auto& subdirs : localdirs) {
		string subpath = path;
		subpath += '\\';
		subpath += subdirs;
		Scan(subpath);
	}
}
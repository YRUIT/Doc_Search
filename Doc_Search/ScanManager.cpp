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
			//�����У����ݿ�û��
			DataManager::GetInstance()->InsertDoc(path, *localit);
			++localit;
		}
		else if (*localit > *dbit) {
			//����û�У����ݿ���
			DataManager::GetInstance()->DeleteDoc(path, *dbit);
			++dbit;
		}
		else {
			++localit;
			++dbit;
		}
	}
	while (localit != localset.end()) {
		//��������
		DataManager::GetInstance()->InsertDoc(path, *localit);
		++localit;
	}
	while (dbit != dbset.end()) {
		//ɾ������
		DataManager::GetInstance()->DeleteDoc(path, *dbit);
		++dbit;
	}
	//�ݹ�ɨ��Ա���Ŀ¼����
	for (const auto& subdirs : localdirs) {
		string subpath = path;
		subpath += '\\';
		subpath += subdirs;
		Scan(subpath);
	}
}
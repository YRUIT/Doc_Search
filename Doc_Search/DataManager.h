#pragma once
#include "Common.h"

class SqliteManager
{
public:
	SqliteManager()
		:_db(nullptr)
	{}
	~SqliteManager()
	{
		Close();
	}
	void Open(const string& path);
	void Close();
	void ExecuteSql(const string& sql);
	void GetTable(const string& sql, int& row, int& col, char**& ppRet);
	SqliteManager(const SqliteManager&) = delete;
	SqliteManager& operator=(const SqliteManager&) = delete;
private:
	sqlite3* _db; // ���ݿ����
};
//RAII
class AutoGetTable {
public:
	AutoGetTable(SqliteManager& sm, const string& sql, int& row, int& col, char**& ppRet)
	{
		sm.GetTable(sql, row, col, ppRet);
		_ppRet = ppRet;
	}
	~AutoGetTable() {
		sqlite3_free_table(_ppRet);
	}
	AutoGetTable(const AutoGetTable&) = delete;
	AutoGetTable& operator=(const AutoGetTable&) = delete;

private:
	char** _ppRet;
};



////////////////////////////////////////////////
//���ݹ���ģ��
#define DB_NAME "doc.db"
#define TB_NAME "tb_doc"


//Ϊ�˷�����м�������Ƴɵ���ģʽ
class DataManager {
public:
	static DataManager* GetInstance() {
		static DataManager datamgr;
		datamgr.Init();
		return &datamgr;
	}
	//���������ݿ�
	void Init();
	//����path�����е����ĵ�
	void GetDoc(const string& path, std::set<string>& dbset);
	//ɾ��
	void DeleteDoc(const string&  path, const string& name);
	//��������
	void InsertDoc(const string& path, const string& name);
	//����
	void Search(const string& key, vector<std::pair<string, string>>& docinfos);
	//����
	void SplitHighlight(const string& str, const string& key, string& prefix, string& highlight, string& suffix);
private:
	DataManager() {}
	SqliteManager _dbmgr;

	std::mutex _mtx;
};
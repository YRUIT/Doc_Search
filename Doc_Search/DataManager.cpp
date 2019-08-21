#pragma once
#include "DataManager.h"
void SqliteManager::Open(const string& path) {
	int ret = sqlite3_open(path.c_str(), &_db);
	if (ret != SQLITE_OK) {
		ERROR_LOG("sqlite3_open\n");
	}
	else {
		TRACE_LOG("sqlite3_open success\n");
	}
}
void SqliteManager::Close() {
	int ret = sqlite3_close(_db);
	if (ret != SQLITE_OK) {
		ERROR_LOG("sqlite3_close\n");
	}
	else {
		TRACE_LOG("sqlite3_close success\n");
	}
}
void SqliteManager::ExecuteSql(const string& sql) {
	char* errmsg;
	int ret = sqlite3_exec(_db, sql.c_str(), nullptr, nullptr, &errmsg);
	if (ret != SQLITE_OK) {
		ERROR_LOG("sqlite3_exec(%s) errmsg: %s\n", sql.c_str(), errmsg);
		sqlite3_free(errmsg);
	}
	else {
		TRACE_LOG("sqlite3_exec(%s) success\n", sql.c_str());
	}
}
void SqliteManager::GetTable(const string& sql, int& row, int& col, char**& ppRet) {
	char* errmsg;
	int ret = sqlite3_get_table(_db, sql.c_str(), &ppRet, &row,  &col, &errmsg);
	if (ret != SQLITE_OK) {
		ERROR_LOG("sqlite3_get_table(%s) errmsg: %s\n", sql.c_str(), errmsg);
		sqlite3_free(errmsg);
	}
	else {
		TRACE_LOG("sqlite3_get_table(%s) success\n", sql.c_str());
	}
}
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
void DataManager::Init() {
	std::unique_lock<std::mutex> lock(_mtx);
	_dbmgr.Open(DB_NAME);
	char sql[256];
	sprintf(sql, "create table if not exists %s(id integer primary key autoincrement, \
				 path text not null, \
				 doc_name text not null, \
				 doc_pinyin text, \
				 doc_name_initials text)", TB_NAME);
	_dbmgr.ExecuteSql(sql);
}
void DataManager::GetDoc(const string& _path, std::set<string>& dbset) {
	char sql[256];
	sprintf(sql, "select doc_name from %s where path='%s';", TB_NAME, _path.c_str());
	int row;
	int col;
	char** ppRet;

	std::unique_lock<std::mutex> lock(_mtx);
	AutoGetTable agt(_dbmgr, sql, row, col, ppRet);
	lock.unlock();

	for (int i = 1; i <= row; ++i) {
		for (int j = 0; j < col; ++j) {
			dbset.insert(ppRet[i * col + j]);
		}
	}
}
void DataManager::InsertDoc(const string& path, const string& name) {
	char sql[4096];
	string pinyin = ChineseConvertPinYinAllSpell(name);
	string initials = ChineseConvertPinYinInitials(name);
	sprintf(sql, "insert into %s(path, doc_name, doc_pinyin, doc_name_initials) values('%s', '%s', '%s', '%s')", TB_NAME, path.c_str(), name.c_str(), pinyin.c_str(), initials.c_str());
	std::unique_lock<std::mutex> lock(_mtx);
	_dbmgr.ExecuteSql(sql);
}
void DataManager::DeleteDoc(const string&  path, const string& name) {
	char sql[256];
	sprintf(sql, "delete from %s where doc_name like '%%s%'", TB_NAME, name.c_str());
	_dbmgr.ExecuteSql(sql);


	string path_ = path;
	path_ += '\\';
	path_ += name;
	sprintf(sql, "delete from %s where path='%s'", TB_NAME, path_.c_str());
	std::unique_lock<std::mutex> lock(_mtx);
	_dbmgr.ExecuteSql(sql);
}

void DataManager::Search(const string& key, vector<std::pair<string, string>>& docinfos) {
	//{
	//	char sql[256];
	//	sprintf(sql, "select doc_name, path from %s where doc_name like '%%%s%%'", TB_NAME, key.c_str());
	//	int row;
	//	int col;
	//	char** ppRet;
	//	AutoGetTable agt(_dbmgr, sql, row, col, ppRet);
	//	int i = 1;
	//	if (i <= row) {
	//		for (; i <= row; ++i) {
	//			docinfos.push_back(std::make_pair(ppRet[i * col + 0], ppRet[i * col + 1]));
	//		}
	//	}
	//}
	{
		char sql[256];
		string pinyin = ChineseConvertPinYinAllSpell(key);
		string initials = ChineseConvertPinYinInitials(key);
		sprintf(sql, "select doc_name, path from %s where doc_pinyin like '%%%s%%' or doc_name_initials like '%%%s%%'", TB_NAME, pinyin.c_str(), initials.c_str());

		//sprintf(sql, "select doc_name, path from %s where doc_pinyin like '%%%s%%'", TB_NAME, pinyin.c_str());
		int row;
		int col;
		char** ppRet;
		std::unique_lock<std::mutex> lock(_mtx);
		AutoGetTable agt(_dbmgr, sql, row, col, ppRet);
		lock.unlock();

		int i = 1;
		if (i <= row) {
			for (; i <= row; ++i) {
				docinfos.push_back(std::make_pair(ppRet[i * col + 0], ppRet[i * col + 1]));
			}
		}
		return;
	}
}

void DataManager::SplitHighlight(const string& str, const string& key, \
	string& prefix, string& highlight, string& suffix) {
	//key直接匹配
	{
		size_t highlight_start = str.find(key);
		if (highlight_start != string::npos) {
			prefix = str.substr(0, highlight_start);
			highlight = key;
			suffix = str.substr(highlight_start + key.size(), string::npos);
			return;
		}
	}
	//key是拼音全拼
	{
		string str_allspell = ChineseConvertPinYinAllSpell(str);
		string key_allspell = ChineseConvertPinYinAllSpell(key);
		size_t highlight_index = 0;
		size_t allspell_index = 0;
		size_t highlight_len = 0;
		size_t highlight_start = 0;
		size_t allspell_start = str_allspell.find(key_allspell);
		if (allspell_start != string::npos) {
			size_t allspell_end = allspell_start + key_allspell.size();

			while (allspell_index < allspell_end) {

				if (allspell_index == allspell_start) {
					highlight_start = highlight_index;
				}
				//如果是ascii字符，就直接走
				if (str[highlight_index] >= 0 && str[highlight_index] <= 127) {
					++highlight_index;
					++allspell_index;
				}
				else {
					char chinese[3] = { '\0' };
					chinese[0] = str[highlight_index];
					chinese[1] = str[highlight_index + 1];
					string   ap_str = ChineseConvertPinYinAllSpell(chinese);
					highlight_index += 2; //gbk汉字是两个字符
					//全拼则跳过这个汉字的拼音的长度
					allspell_index += ap_str.size();
				}
			}
			highlight_len = highlight_index - highlight_start;

			prefix = str.substr(0, highlight_start);
			highlight = str.substr(highlight_start, highlight_len);
			suffix = str.substr(highlight_start + highlight_len, string::npos);
			return;
		}
	}
	//首字母
	{
		string initials_str = ChineseConvertPinYinInitials(str);
		string initials_key = ChineseConvertPinYinInitials(key);
		size_t initials_start = initials_str.find(initials_key);
		if (initials_start != string::npos) {
			size_t initials_end = initials_start + initials_key.size();
			size_t initials_index = 0;
			size_t highlight_index = 0;
			size_t highlight_start = 0;
			size_t highlight_len = 0;
			while (initials_index < initials_end) {
				if (initials_index == initials_start) {
					highlight_start = highlight_index;
				}
				//ASCII
				if (str[highlight_index] >= 0 && str[highlight_index] <= 127) {
					++highlight_index;
					++initials_index;
				}
				//汉字
				else {
					highlight_index += 2;
					++initials_index;
				}
			}
			highlight_len = highlight_index - highlight_start;
			prefix = str.substr(0, highlight_start);
			highlight = str.substr(highlight_start, highlight_len);
			suffix = str.substr(highlight_start + highlight_len, string::npos);
			return;
		}
	}

	TRACE_LOG("splite highlight no match. str : %s  key : %s", str.c_str(), key.c_str());
	prefix = str;
}


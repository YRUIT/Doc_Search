#include "Common.h"
#include "DataManager.h"
#include "ScanManager.h"

void TestDirectoryList() {
	vector<string> dirs;
	vector<string> files;
	DirectoryList("D:\\宇睿", dirs, files);
	for (const auto& e : dirs) {
		cout << e << endl;
	}
	for (const auto& e : files) {
		cout << e << endl;
	}
}

void TestSqlite() {
	SqliteManager sq;
	sq.Open("test.db");

	string createtb_sql = "create table if not exists tb_doc(id integer primary key autoincrement, doc_path text, doc_name text);";
	sq.ExecuteSql(createtb_sql);
	
	string insert_sql = "insert into tb_doc(doc_path, doc_name) values('sdadqwe' , 'leihoua');";
	sq.ExecuteSql(insert_sql);
	string query_sql = "select * from tb_doc where doc_name = 'nihao';";
	int row;
	int col;
	char** ppRet;
	sq.GetTable(query_sql, row, col, ppRet);
	/*for (int i = 0; i < row; ++i) {
		for (int j = 0; j < col; ++j) {
			cout << ppRet[i * col + j] << " ";
		}
		cout << endl;
	}
	sqlite3_free_table(ppRet);*/
	
	AutoGetTable agt(sq, query_sql, row, col, ppRet);
	for (int i = 0; i < row; ++i) {
		for (int j = 0; j < col; ++j) {
			cout << ppRet[i * col + j] << " ";
		}
		cout << endl;
	}
/*	string delete_sql = "DELETE FROM tb_doc where doc_path = 'yr'";
	sq.ExecuteSql(delete_sql)*/;
}

void TestScanManager() {
	//ScanManager scanmgr;
	//scanmgr.Scan("D:");
}

void TestSearch() {
	ScanManager::CreateInstance();
	//DataManager datamgr;
	DataManager::GetInstance()->Init();
	vector<std::pair<string, string>> docinfos;
	string key;
	cout << "==================================================================================================================================================" << endl;
	cout << "请输入关键字进行搜索：";
	while (std::cin >> key) {
		docinfos.clear();
		//DataManager::GetInstance()->Search(key, docinfos);
		DataManager::GetInstance()->Search(key, docinfos);
		printf("%-100s %-100s\n", "名称", "路径");
		for (const auto& e : docinfos) {
			//cout << e.first << "  " << e.second << endl;
			//printf("%-50s %-50s\n", e.first.c_str(), e.second.c_str());
			string prefix, highlight, suffix;
			const string& name = e.first;
			const string& path = e.second;
			DataManager::GetInstance()->SplitHighlight(e.first, key, prefix, highlight, suffix);
			cout << prefix;
			ColourPrintf(highlight.c_str());
			cout << suffix;
			//补齐100空格
			for (size_t i = name.size(); i <= 100; ++i) {
				cout << " ";
			}
			printf("%-100s\n", e.second.c_str());

		}
		cout << "==================================================================================================================================================" << endl;
		cout << "请输入关键字进行搜索：";
	}
}

void TestColourPrintf() {
	ColourPrintf("leihoua\n");
}
void TestChineseConvertPinYinAllSpell() {
	string str("宇睿");
	cout << ChineseConvertPinYinAllSpell(str) << endl;
}
void TestChineseConvertPinYinInitials() {
	string str("中国nihao");
	cout << ChineseConvertPinYinInitials(str) << endl;
}
void TestHighLight() {
	//key是什么，高亮什么
	{
		string key = "宇睿";
		string str = "ad666ffafanihaoas宇睿";
		size_t pos = str.find(key);
		string prefix, suffix;
		prefix = str.substr(0, pos);
		suffix = str.substr(pos + key.size(), string::npos);
		cout << prefix;
		ColourPrintf(key.c_str());
		cout << suffix << endl;
	}
	//key是拼音，高亮对应的汉字
	{
	string key = "nihao";
	string str = "ad6626f你好fafas";
	string prefix, suffix;
	string str_py = ChineseConvertPinYinAllSpell(str);
	string key_py = ChineseConvertPinYinAllSpell(key);
	size_t pos = str_py.find(key_py);
	if (pos == string::npos) {
		cout << "拼音不匹配" << endl;
	}
	else {
		size_t key_start = pos;
		size_t key_end = pos + key_py.size();
		size_t str_i = 0, py_i = 0;
		while (py_i < key_start) {
			if (str[str_i] >= 0 && str[str_i] <= 127) {
				++str_i;
				++py_i;
			}
			else {
				char chinese[3] = { '\0' };
				chinese[0] = str[str_i];
				chinese[1] = str[str_i + 1];
				str_i += 2;
				string py_str = ChineseConvertPinYinAllSpell(chinese);
				py_i += py_str.size();
			}
		}
		prefix = str.substr(0, str_i);
		size_t str_j = str_i, py_j = py_i;
		while (py_j < key_end) {
			if (str[str_j] >= 0 && str[str_j] <= 127) {
				++str_j;
				++py_j;
			}
			else{
				char chinese[3] = { '\0' };
				chinese[0] = str[str_j];
				chinese[1] = str[str_j + 1];
				str_j += 2;
				string py_str = ChineseConvertPinYinAllSpell(chinese);
				py_j += py_str.size();
			}
		}
		key = str.substr(str_i, str_j - str_i);
		suffix = str.substr(str_j, string::npos);
		cout << prefix;
		ColourPrintf(key.c_str());
		cout << suffix << endl;
	}
}
	//key是拼音首字母，高亮对应的汉字
	{
		string key = "nh";
		string str = "asdafqinnv你好airsvbg";
		string prefix, suffix;

	}
}
int main()
{
	//TestDirectoryList();
	//TestSqlite();
	//TestScanManager();
	TestSearch();
	//TestColourPrintf();
	//TestChineseConvertPinYinAllSpell();
	//TestChineseConvertPinYinInitials();
	//TestHighLight();
	return 0;
}
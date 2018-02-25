#include <windows.h>
#include <sstream>
#include <fstream>
#include <string>
#include <regex>
#include "UTAU.h"

class Tool{
public:
	template <typename T>
	static void error(T value){
		
		std::string message;
		std::stringstream(value) >> message;

		// http://msdn.microsoft.com/ja-jp/library/cc410914.aspx
		MessageBox(0, message.c_str(), "エラーが発生しました", MB_OK | MB_ICONERROR);
		exit(0);
	}
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd){
	
	// 引数の数が2つ以外のときは終了
	if(__argc != 2) Tool::error("引数の数が合いません");
	
	// 一時ファイルが開く
	UTAU::SectionVector tempFile;
	if(!UTAU::UstReader::open(__argv[1], tempFile)) Tool::error("プラグインに渡された一時ファイルが開けません。");
	
	// テーブルファイルのパスを作る
	std::string argv(__argv[0]);
	auto tableFilePath = argv.substr(0, argv.rfind("\\")+1) + "table.txt";
	
	// テーブルファイルを開く
	std::ifstream	tableFile(tableFilePath);
	if(!tableFile) Tool::error("テーブルファイルが開けません。");

	// テーブルを作成
	std::string buffer;
	std::map<std::string, std::string> table;
	while(tableFile && std::getline(tableFile, buffer)){
		
		// テーブルの正規表現
		const std::regex tableRegex("\\s*(.+?)\\s*,\\s*(.+?)\\s*");

		// マッチオブジェクト
		std::match_results<std::string::const_iterator> matchObject;
		if(std::regex_match(buffer, matchObject, tableRegex)){
			table.insert(std::pair<std::string, std::string>(matchObject[1], matchObject[2]));
		}
	}
	// テーブルが空なら警告
	if(table.empty()) Tool::error("テーブルが空です。");

	// 歌詞を置換する
	for(auto i=tempFile.begin(); i!=tempFile.end(); ++i){
		for(auto t=table.begin(); t!=table.end(); ++t){
			if(i->lyric == t->first) i->lyric = t->second;
		}
	}

	// 一時ファイルへの出力を開いて書き出し
	std::ofstream tempFileOut(__argv[1]);
	if(tempFileOut) tempFileOut << tempFile;
    return 0;
}
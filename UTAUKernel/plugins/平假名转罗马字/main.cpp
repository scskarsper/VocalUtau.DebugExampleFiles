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
		MessageBox(0, message.c_str(), "�G���[���������܂���", MB_OK | MB_ICONERROR);
		exit(0);
	}
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd){
	
	// �����̐���2�ȊO�̂Ƃ��͏I��
	if(__argc != 2) Tool::error("�����̐��������܂���");
	
	// �ꎞ�t�@�C�����J��
	UTAU::SectionVector tempFile;
	if(!UTAU::UstReader::open(__argv[1], tempFile)) Tool::error("�v���O�C���ɓn���ꂽ�ꎞ�t�@�C�����J���܂���B");
	
	// �e�[�u���t�@�C���̃p�X�����
	std::string argv(__argv[0]);
	auto tableFilePath = argv.substr(0, argv.rfind("\\")+1) + "table.txt";
	
	// �e�[�u���t�@�C�����J��
	std::ifstream	tableFile(tableFilePath);
	if(!tableFile) Tool::error("�e�[�u���t�@�C�����J���܂���B");

	// �e�[�u�����쐬
	std::string buffer;
	std::map<std::string, std::string> table;
	while(tableFile && std::getline(tableFile, buffer)){
		
		// �e�[�u���̐��K�\��
		const std::regex tableRegex("\\s*(.+?)\\s*,\\s*(.+?)\\s*");

		// �}�b�`�I�u�W�F�N�g
		std::match_results<std::string::const_iterator> matchObject;
		if(std::regex_match(buffer, matchObject, tableRegex)){
			table.insert(std::pair<std::string, std::string>(matchObject[1], matchObject[2]));
		}
	}
	// �e�[�u������Ȃ�x��
	if(table.empty()) Tool::error("�e�[�u������ł��B");

	// �̎���u������
	for(auto i=tempFile.begin(); i!=tempFile.end(); ++i){
		for(auto t=table.begin(); t!=table.end(); ++t){
			if(i->lyric == t->first) i->lyric = t->second;
		}
	}

	// �ꎞ�t�@�C���ւ̏o�͂��J���ď����o��
	std::ofstream tempFileOut(__argv[1]);
	if(tempFileOut) tempFileOut << tempFile;
    return 0;
}
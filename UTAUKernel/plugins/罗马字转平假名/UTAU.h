#pragma once
#include <sstream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <map>
#include <utility>

namespace UTAU{

	///////////////////////////////////////////////////////////////////////////
	// �萔
	///////////////////////////////////////////////////////////////////////////
	/// �p�[�X����ۂ̍s�̎�ނ�\���񋓌^
	enum LineType{
		ParameterLine	= 0x0100,	///< �Z�N�V�����ł͂Ȃ��p�����[�^��\���s�i�r�b�g���Z�q&�Ŏg�p�j
		UnknownLine,				///< ���m�̃p�����[�^
		LengthLine,					///< Length=
		LyricLine,					///< Lyric=
		NoteNumLine,				///< NoteNum=
		TempoLine,					///< Tempo=
		CacheLine,					///< CacheDir=
		SectionLine		= 0x1000,	///< �Z�N�V�����J�n�s��\���i�r�b�g���Z�q&�Ŏg�p�j
		UnknownSection,				///< ���m�Z�N�V����
		NormalSection,				///< [#0000] �ʏ�̃Z�N�V����
		PrevSection,				///< [#PREV]
		NextSection,				///< [#NEXT]
		SettingSection,				///< [#SETTING]
		TrackEndSection,			///< [#TRACKEND]
	};

	/// UTAU::Section.number �ŁA���l���g�p���Ȃ��ꍇ�Ɏg���萔
	const int NoNumber = 10000;

	/*************************************************************************/
	/// UTAU�֘A�t�@�C���̃Z�N�V������\���N���X.
	/// UTAU::SectionVector �͂��̃N���X�̔z��
	/// @author	����
	/// @date	2012/01/20
	/// @sa		UTAU::SectionVector
	/*************************************************************************/
	class Section{
	public:
		
		///////////////////////////////////////////////////////////////////////////
		// �萔
		///////////////////////////////////////////////////////////////////////////
		
		/// �Z�N�V�����̎�ނ�\���񋓌^
		enum SectionType{
			SectionUnknown,		///< ��ʕs��
			SectionNormal,		///< [#0000] �ʏ�̔ԍ��t���Z�N�V����
			SectionSetting,		///< [#SETTING]
			SectionPrev,		///< [#PREV]
			SectionNext,		///< [#NEXT]
			SectionDelete,		///< [#DELETE]
			SectionInsert,		///< [#INSERT]
			SectionTrackEnd		///< [#TRACKEND]
		};

		///////////////////////////////////////////////////////////////////////////
		// public �ϐ�
		///////////////////////////////////////////////////////////////////////////
		
		SectionType	type;		///< �Z�N�V�����̎�� SectionUnknown �̂Ƃ��ɂ� name ���g�p
		int			number;		///< �Z�N�V�����ԍ� �K�v�Ȃ��^�C�v�̎��� UTAU::NoNumber
		std::string	name;		///< �Z�N�V������ �Z�N�V�����̎�ނ��s���Ȏ��ɂ̂ݎg�p
		std::string lyric;		///< �̎�
		int			note;		///< �m�[�g�i���o�[
		int			length;		///< �m�[�g�̒���
		float		tempo;		///< �e���|
		std::map<std::string, std::string>
					unknownParameters;	///< ���m�̃p�����[�^�̂��߂� std::map
		

		///////////////////////////////////////////////////////////////////////////
		/// �R���X�g���N�^.
		/// @param type		�Z�N�V�����̎�� UTAU::Section::SectionType
		/// @param lyric	�̎�
		/// @param note		�m�[�g�i���o�[
		/// @param length	���̒���
		/// @param number	�Z�N�V�����ԍ�
		///////////////////////////////////////////////////////////////////////////
		Section(
			const SectionType type=SectionNormal,
			const std::string lyric="",
			const int note=0,
			const int length=0,
			const int number=NoNumber
		)
			:	type(type), number(number), name(""), lyric(lyric),
				note(note), length(length), tempo(0.0)
		{

		}


		///////////////////////////////////////////////////////////////////////////
		/// ��r���Z�q == �̃I�[�o�[���[�h.
		/// <algorithm> �� std::search() �ɑΉ����邽�߂̕�
		/// �̎��Ɖ��̒����ƃe���|���݂̂��r����悤�ɂȂ��Ă���
		///////////////////////////////////////////////////////////////////////////
		bool operator==(const Section& section) const{
			if(lyric == section.lyric && note == section.note &&
				length == section.length && tempo == section.tempo){
					return true;
			}
			return false;
		}
	};

	/*************************************************************************/
	/// UTAU::Section �̔z��.
	/// std::vector<UTAU::Section> ���p���������̂ŁA�X�g���[���ւ̏����o����A
	/// <algorithm> �� search() ���g�p�����A�m�[�g���� find() �Ȃǂ����B
	/// �܂��A�e���|����L���b�V���p�X�ȂǁA[#SETTING]�̓��e���ێ�����B
	/// @author	����
	/// @date	2012/01/20
	/// @sa		UTAU::Section
	/*************************************************************************/
	class SectionVector: public std::vector<Section>{
	public:
		
		///////////////////////////////////////////////////////////////////////////
		// public �ϐ�
		///////////////////////////////////////////////////////////////////////////
		float		tempo;		///< �v���W�F�N�g�S�̂̃e���|
		std::string cachePath;	///< �L���b�V���p�X
		std::string ustPath;	///< ust�t�@�C���̃p�X
		std::map<std::string, std::string>
			unknownParameters;	///< ���m�̃p�����[�^�̂��߂� std::map
		
		///////////////////////////////////////////////////////////////////////////
		/// <algorithm> std::search() ���g�p�����v�f�T��.
		/// @param	pattern	�T������ UTAU::SectionVector
		/// @bug	���̃��\�b�h�͌��݂̎����ł� #PREV �ւ̑Ή������Ă��Ȃ����ߐ��퓮�삵�Ȃ�
		/// @sa		UTAU::Section::operator==()
		///////////////////////////////////////////////////////////////////////////
		std::vector<Section>::const_iterator find(const SectionVector& pattern) const{
			return std::search(begin(), end(), pattern.begin(), pattern.end());
		}

		///////////////////////////////////////////////////////////////////////////
		/// �w�肳�ꂽ�Z�N�V�����ԍ������m�[�g�ւ̃C�e���[�^��Ԃ�.
		/// @param	sectionNumber	�T���Z�N�V�����ԍ�
		/// @return	�w�肳�ꂽ�ԍ��̃m�[�g�� std::vector<UTAU::Section>::const_iterator
		///////////////////////////////////////////////////////////////////////////
		std::vector<Section>::const_iterator getNote(int sectionNumber) const{
			for(std::vector<Section>::const_iterator i = this->begin(); i!=this->end(); ++i){
				if(i->number == sectionNumber) return i;
			}
			return this->end();
		}
	};

	///////////////////////////////////////////////////////////////////////////
	/// �X�g���[�����Z�q << ���g�p���������o��.
	/// �����_�ł͊��S�ȑΉ��ƂȂ��Ă��Ȃ����߁A���̂܂܏����o���Ă��A
	/// �ꎞ�t�@�C����ust�t�@�C���Ƃ��Ă̎g�p�͏o���Ȃ��B
	/// @todo	�ꎞ�t�@�C���ւ̊��S�Ή�
	/// @memo	ust�t�@�C���ւ̑Ή��͕ʂ̌^�ւ̃L���X�g�Ȃǂ��g���ׂ����E�E�E
	///////////////////////////////////////////////////////////////////////////
	std::ostream& operator <<(std::ostream& stream, const SectionVector& section){
		// SETTING �������o��
		stream << "[#SETTING]"			<< std::endl;
		stream << section.cachePath		<< std::endl;
		stream << section.ustPath		<< std::endl;
		stream << section.tempo			<< std::endl;

		// ���g�������o���i�K���j
		for(std::vector<Section>::const_iterator i=section.begin(); i<section.end(); ++i){
			
			switch(i->type){
				case Section::SectionPrev:
					stream << "[#PREV]"<< std::endl;
					break;
				case Section::SectionNext:
					stream << "[#NEXT]"<< std::endl;
					break;
				case Section::SectionDelete:
					stream << "[#DELETE]"<< std::endl;
					break;
				case Section::SectionNormal:
					stream << "[#" << std::setw(4) << std::setfill('0') << i->number << "]"<< std::endl;
					stream << "Length="		<< i->length	<< std::endl;
					stream << "NoteNum="	<< i->note		<< std::endl;
					stream << "Lyric="		<< i->lyric		<< std::endl;
					if(i->tempo > 0)
						stream << "Tempo=" << i->tempo		<< std::endl;
					// p:unknownParameters iterator i:section iterator
					for(auto p=i->unknownParameters.begin(); p!=i->unknownParameters.end(); ++p){
						stream << p->first << "=" << p->second << std::endl;
					}
					break;
				case Section::SectionInsert:
					stream << "[#INSERT]"<< std::endl;
					stream << "Length="		<< i->length	<< std::endl;
					stream << "NoteNum="	<< i->note		<< std::endl;
					stream << "Lyric="		<< i->lyric		<< std::endl;
					if(i->tempo > 0)
						stream << "Tempo=" << i->tempo		<< std::endl;
					// p:unknownParameters iterator i:section iterator
					for(auto p=i->unknownParameters.begin(); p!=i->unknownParameters.end(); ++p){
						stream << p->first << "=" << p->second << std::endl;
					}
					break;
			}
		}
		
		return stream;
	}


	/*************************************************************************/
	/// ust�t�@�C����ꎞ�t�@�C���̓ǂݍ���.
	/// static �ȃ��\�b�h�̋l�ߍ��킹�Ȃ̂ŁA
	/// �N���X���\�b�h�Ƃ��ČĂяo���Ďg�p���Ă��������B
	/// @author	����
	/// @date	2012/01/20
	/// @sa		UTAU::Section UTAU::SectionVector
	/*************************************************************************/
	class UstReader{

	private:
		///////////////////////////////////////////////////////////////////////////
		// private �N���X���N���X ���{���͐؂蕪�����������݈ˑ��������߂�ǂ�
		///////////////////////////////////////////////////////////////////////////
		
		/*************************************************************************/
		/// �Z�N�V�����̃p�[�T�[�̊��N���X.
		/// �s�̎�ނ�Ԃ� UTAU::UstReader::Parser::whatIsLine() �ƁA
		/// �p���̂��߂̏������z�֐� parse() �����B
		/// @author	����
		/// @date	2012/01/20
		/// @sa		UTAU::UstReader
		/*************************************************************************/
		class Parser{
		public:
			///////////////////////////////////////////////////////////////////////////
			// public �ϐ�
			///////////////////////////////////////////////////////////////////////////
			
			///////////////////////////////////////////////////////////////////////////
			/// �f�X�g���N�^.
			/// �p�����邽�ߔO�̂��� virtual ���g�p
			///////////////////////////////////////////////////////////////////////////
			virtual ~Parser(){};
			
			///////////////////////////////////////////////////////////////////////////
			/// �p�[�X���s���������z�֐�.
			/// �p�����ɂ��ꂼ��̃p�[�T�����̃��\�b�h�̒��g����������
			/// @param	buffer	�t�@�C����1�s std::string
			/// @param	vector	�Z�N�V������ۑ����� UTAU::SectionVector
			/// @param	file	�ǂݍ��ݒ��̃t�@�C�� std::ifstream
			///////////////////////////////////////////////////////////////////////////
			virtual void parse(std::string& buffer, SectionVector& vector, std::ifstream& file) = 0;

			///////////////////////////////////////////////////////////////////////////
			/// �^����ꂽ������ust��ꎞ�t�@�C���łȂ�̍s�Ȃ̂�����肷��.
			/// @param	buffer	�t�@�C����1�s std::string
			/// @return	�s�̎�ނ� UTAU::LineType �ŕԂ�
			///////////////////////////////////////////////////////////////////////////
			static LineType whatIsThisLine(const std::string& buffer){
				const std::string settingMarker	= "[#SETTING]";
				const std::string prevMarker		= "[#PREV]";
				const std::string nextMarker		= "[#NEXT]";
				const std::string trackEndMarker	= "[#TRACKEND]";
				const std::string sectionMarker	= "[#";
				const std::string lyricMarker		= "Lyric=";
				const std::string lengthMarker		= "Length=";
				const std::string noteNumMarker	= "NoteNum=";
				const std::string tempoMarker		= "Tempo=";
				const std::string cacheMarker		= "CacheDir=";

				// �Z�b�e�B���O
				if(buffer.compare(0, settingMarker.length(), settingMarker) == 0)		return SettingSection;
				// �I��͈͂̑O�̃m�[�g
				else if(buffer.compare(0, prevMarker.length(), prevMarker) == 0)		return PrevSection;
				// �I��͈͂̎��̃m�[�g
				else if(buffer.compare(0, nextMarker.length(), nextMarker) == 0)		return NextSection;
				// �g���b�N�̏I��
				else if(buffer.compare(0, trackEndMarker.length(), trackEndMarker) == 0)return TrackEndSection;
				// �ʏ�̃Z�N�V����
				else if(buffer.compare(0,sectionMarker.length(), sectionMarker) == 0 &&
					buffer[sectionMarker.length()] >= '0' && buffer[sectionMarker.length()] <= '9')	return NormalSection;
				// ���̑��̃Z�N�V����
				else if(buffer.compare(0,sectionMarker.length(), sectionMarker) == 0)	return UnknownSection;
				// Length
				else if(buffer.compare(0,lengthMarker.length(), lengthMarker) == 0)		return LengthLine;
				// Lyric
				else if(buffer.compare(0,lyricMarker.length(), lyricMarker) == 0)		return LyricLine;
				// NoteNum
				else if(buffer.compare(0,noteNumMarker.length(), noteNumMarker) == 0)	return NoteNumLine;
				// Tempo
				else if(buffer.compare(0,tempoMarker.length(), tempoMarker) == 0)		return TempoLine;
				// CacheDir
				else if(buffer.compare(0,cacheMarker.length(), cacheMarker) == 0)		return CacheLine;
				// Other
				else return UnknownLine;
			}
		};

		/*************************************************************************/
		/// [#SETTING]��ǂނ��߂̃p�[�T.
		/// UTAU::UstReader::Parser ���p������
		/// @author	����
		/// @date	2012/01/20
		/// @sa		UTAU::UstReader::Parser
		/*************************************************************************/
		class SettingParser : public Parser {
		public:
			void parse(std::string& buffer, SectionVector& vector, std::ifstream& file){
				while(std::getline(file, buffer)){
					
					LineType type = whatIsThisLine(buffer);
					
					// ���̃Z�N�V�����̓���������߂�
					if(type & SectionLine) return;

					switch(type){
					case TempoLine:
						std::stringstream(buffer.substr(buffer.find("=")+1)) >> vector.tempo;
						break;
					case CacheLine:
						vector.cachePath	= buffer.substr(buffer.find("=")+1);
						vector.ustPath		= vector.cachePath.substr(0, vector.cachePath.rfind(".")) + ".ust";
						break;
					case UnknownLine:
						// = �O�ƌ�ɕ����Ė��m�̃p�����[�^�Ƃ��ĕۑ�
						vector.unknownParameters.insert(
							std::pair<std::string,std::string>(
								buffer.substr(0, buffer.find("=")),
								buffer.substr(buffer.find("=")+1)
							)
						); break;
					}
				}
			}
		};

		/*************************************************************************/
		/// �ǂݔ�΂����߂̃p�[�T.
		/// UTAU::UstReader::Parser ���p������B�������A�R���X�g���N�^�ŁA
		/// ���̃Z�N�V������ǂݔ�΂��̂��� UTAU::Section::SectionType �Ŏw�肷��B
		/// @author	����
		/// @date	2012/01/20
		/// @sa		UTAU::UstReader::Parser
		/*************************************************************************/
		class SkipParser : public Parser {
			Section::SectionType type;
		public:
			
			void parse(std::string& buffer, SectionVector& vector, std::ifstream& file){
				
				Section section;
				section.number = NoNumber;
				section.type = type;
				// �m��Ȃ��Z�N�V������������Z�N�V��������ێ� �i���������݂͖��m�̃Z�N�V������parse()�ł͂����Ă���j
				if(section.type == Section::SectionUnknown){
					// # ����I���܂ł𔲂��o�����Ė��O�ɓ����
					section.name = buffer.substr(buffer.find("#")+1, buffer.length()-1-buffer.find("#"));
				}

				vector.push_back(section);

				while(std::getline(file, buffer)){
					if(whatIsThisLine(buffer) & SectionLine) return;
				}

			}

			SkipParser(const Section::SectionType type)
				: type(type){
			}

		};

		/*************************************************************************/
		/// �ʏ�̃Z�N�V������ǂނ��߂̃p�[�T.
		/// UTAU::UstReader::Parser ���p������B�ʏ�̃Z�N�V������[#����]�ŕ\�����B
		/// @author	����
		/// @date	2012/01/20
		/// @sa		UTAU::UstReader::Parser
		/*************************************************************************/
		class NormalParser : public Parser {
		public:
			void parse(std::string& buffer, SectionVector& vector, std::ifstream& file){
				
				Section section;

				// �Z�N�V�����i���o�[��4���Œ�
				std::stringstream(buffer.substr(buffer.find("#")+1 , 4)) >> section.number;
				// �Z�N�V�����^�C�v��ݒ�
				section.type	= Section::SectionNormal;

				while(std::getline(file, buffer)){

					LineType type = whatIsThisLine(buffer);
					if(type & SectionLine){
						vector.push_back(section);
						return;
					}
					switch(type){
					case LengthLine:
						std::stringstream(buffer.substr(buffer.find("=")+1)) >> section.length;
						break;
					case TempoLine:
						std::stringstream(buffer.substr(buffer.find("=")+1)) >> section.tempo;
						break;
					case NoteNumLine:
						std::stringstream(buffer.substr(buffer.find("=")+1)) >> section.note;
						break;
					case LyricLine:
						section.lyric = buffer.substr(buffer.find("=")+1);
						break;
					case UnknownLine:
						// = �O�ƌ�ɕ����Ė��m�̃p�����[�^�Ƃ��ĕۑ�
						section.unknownParameters.insert(
							std::pair<std::string,std::string>(
							buffer.substr(0, buffer.find("=")),
							buffer.substr(buffer.find("=")+1)
							)
						); break;
					}
				}
				vector.push_back(section);
			}
		};
		///////////////////////////////////////////////////////////////////////////
		// �N���X���N���X�����܂�
		///////////////////////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////////////////////
		/// ust�t�@�C����ꎞ�t�@�C����1�s��ǂݍ��݃p�[�X���s��.
		/// ���ۂ̃p�[�X�� UTAU::UstReader::Parser ���p�������A�e��p�[�T�[���s���B
		/// @param	vector	�Z�N�V������ۑ����� UTAU::SectionVector
		/// @param	file	�ǂݍ��ݒ��̃t�@�C�� std::ifstream
		/// @sa		UTAU::UstReader::Parser	
		///////////////////////////////////////////////////////////////////////////
		static void parse(std::ifstream& file, SectionVector& vector){

			// �o�b�t�@
			std::string		buffer;

			// �܂�1�s�ǂݍ���
			if(!std::getline(file, buffer)) return;

			// �t�@�C���̏I���܂Ń��[�v
			while(file){

				// �p�[�T�[
				std::unique_ptr<Parser> parser;

				// �Z�N�V�����̃^�C�v�ɂ���ď����𕪂���
				switch(Parser::whatIsThisLine(buffer)){
				case SettingSection:
					parser.reset(new SettingParser());break;
				case PrevSection:
					parser.reset(new SkipParser(Section::SectionPrev));break;
				case UnknownSection:
					//parser.reset(new SkipParser(Section::SectionUnknown));break;
					return; // ���m�̃Z�N�V�����͈���Ȃ�
				case NormalSection:
					parser.reset(new NormalParser());break;
				case NextSection: return;
					parser.reset(new SkipParser(Section::SectionNext));break;
				case TrackEndSection: return;
				}

				// �w�肵���p�[�T�[�Ńp�[�X����
				parser->parse(buffer, vector, file);
			}
		}

	public:

		///////////////////////////////////////////////////////////////////////////
		/// �w�肳�ꂽ�p�X���J���ăp�[�X���Ăяo��.
		/// @param	path	�t�@�C���p�X��\�� std::string(�I�[�o�[���[�h�� char* ��)
		/// @param	vector	�Z�N�V������ۑ����邽�߂� UTAU::SectionVector
		/// @return	�t�@�C�����J������ true �J���Ȃ������� false
		/// @sa		UTAU::UstReader::parse()
		/// @note	���̎����ł̓p�[�X���Ăяo�����ʒu����\�z���ď����o�����@���Ȃ�
		///////////////////////////////////////////////////////////////////////////
		static bool open(const std::string& path, SectionVector& vector){
			std::ifstream	file(path);
			if(!file){
				return false;
			}
			else{
				parse(file, vector);
				return true;
			}
			
		}

		///////////////////////////////////////////////////////////////////////////
		/// �w�肳�ꂽ�p�X���J���ăp�[�X���Ăяo��.
		/// @param	path	�t�@�C���p�X��\�� char* (�I�[�o�[���[�h�� std::string ��)
		/// @param	vector	�Z�N�V������ۑ����邽�߂� UTAU::SectionVector
		/// @return	�t�@�C�����J������ true �J���Ȃ������� false
		/// @sa		UTAU::UstReader::parse()
		/// @note	���̎����ł̓p�[�X���Ăяo�����ʒu����\�z���ď����o�����@���Ȃ�
		///////////////////////////////////////////////////////////////////////////
		static bool open(const char* path, SectionVector& vector){
			return open(std::string(path), vector);
		}
	};
}
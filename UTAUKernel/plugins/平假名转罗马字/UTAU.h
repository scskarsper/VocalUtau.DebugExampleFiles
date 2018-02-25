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
	// 定数
	///////////////////////////////////////////////////////////////////////////
	/// パースする際の行の種類を表す列挙型
	enum LineType{
		ParameterLine	= 0x0100,	///< セクションではなくパラメータを表す行（ビット演算子&で使用）
		UnknownLine,				///< 未知のパラメータ
		LengthLine,					///< Length=
		LyricLine,					///< Lyric=
		NoteNumLine,				///< NoteNum=
		TempoLine,					///< Tempo=
		CacheLine,					///< CacheDir=
		SectionLine		= 0x1000,	///< セクション開始行を表す（ビット演算子&で使用）
		UnknownSection,				///< 未知セクション
		NormalSection,				///< [#0000] 通常のセクション
		PrevSection,				///< [#PREV]
		NextSection,				///< [#NEXT]
		SettingSection,				///< [#SETTING]
		TrackEndSection,			///< [#TRACKEND]
	};

	/// UTAU::Section.number で、数値を使用しない場合に使う定数
	const int NoNumber = 10000;

	/*************************************************************************/
	/// UTAU関連ファイルのセクションを表すクラス.
	/// UTAU::SectionVector はこのクラスの配列
	/// @author	ちえ
	/// @date	2012/01/20
	/// @sa		UTAU::SectionVector
	/*************************************************************************/
	class Section{
	public:
		
		///////////////////////////////////////////////////////////////////////////
		// 定数
		///////////////////////////////////////////////////////////////////////////
		
		/// セクションの種類を表す列挙型
		enum SectionType{
			SectionUnknown,		///< 種別不明
			SectionNormal,		///< [#0000] 通常の番号付きセクション
			SectionSetting,		///< [#SETTING]
			SectionPrev,		///< [#PREV]
			SectionNext,		///< [#NEXT]
			SectionDelete,		///< [#DELETE]
			SectionInsert,		///< [#INSERT]
			SectionTrackEnd		///< [#TRACKEND]
		};

		///////////////////////////////////////////////////////////////////////////
		// public 変数
		///////////////////////////////////////////////////////////////////////////
		
		SectionType	type;		///< セクションの種類 SectionUnknown のときには name も使用
		int			number;		///< セクション番号 必要ないタイプの時は UTAU::NoNumber
		std::string	name;		///< セクション名 セクションの種類が不明な時にのみ使用
		std::string lyric;		///< 歌詞
		int			note;		///< ノートナンバー
		int			length;		///< ノートの長さ
		float		tempo;		///< テンポ
		std::map<std::string, std::string>
					unknownParameters;	///< 未知のパラメータのための std::map
		

		///////////////////////////////////////////////////////////////////////////
		/// コンストラクタ.
		/// @param type		セクションの種類 UTAU::Section::SectionType
		/// @param lyric	歌詞
		/// @param note		ノートナンバー
		/// @param length	音の長さ
		/// @param number	セクション番号
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
		/// 比較演算子 == のオーバーロード.
		/// <algorithm> の std::search() に対応するための物
		/// 歌詞と音の長さとテンポ情報のみを比較するようになっている
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
	/// UTAU::Section の配列.
	/// std::vector<UTAU::Section> を継承したもので、ストリームへの書き出しや、
	/// <algorithm> の search() を使用した、ノート検索 find() などを持つ。
	/// また、テンポ情報やキャッシュパスなど、[#SETTING]の内容も保持する。
	/// @author	ちえ
	/// @date	2012/01/20
	/// @sa		UTAU::Section
	/*************************************************************************/
	class SectionVector: public std::vector<Section>{
	public:
		
		///////////////////////////////////////////////////////////////////////////
		// public 変数
		///////////////////////////////////////////////////////////////////////////
		float		tempo;		///< プロジェクト全体のテンポ
		std::string cachePath;	///< キャッシュパス
		std::string ustPath;	///< ustファイルのパス
		std::map<std::string, std::string>
			unknownParameters;	///< 未知のパラメータのための std::map
		
		///////////////////////////////////////////////////////////////////////////
		/// <algorithm> std::search() を使用した要素探索.
		/// @param	pattern	探索する UTAU::SectionVector
		/// @bug	このメソッドは現在の実装では #PREV への対応をしていないため正常動作しない
		/// @sa		UTAU::Section::operator==()
		///////////////////////////////////////////////////////////////////////////
		std::vector<Section>::const_iterator find(const SectionVector& pattern) const{
			return std::search(begin(), end(), pattern.begin(), pattern.end());
		}

		///////////////////////////////////////////////////////////////////////////
		/// 指定されたセクション番号を持つノートへのイテレータを返す.
		/// @param	sectionNumber	探すセクション番号
		/// @return	指定された番号のノートの std::vector<UTAU::Section>::const_iterator
		///////////////////////////////////////////////////////////////////////////
		std::vector<Section>::const_iterator getNote(int sectionNumber) const{
			for(std::vector<Section>::const_iterator i = this->begin(); i!=this->end(); ++i){
				if(i->number == sectionNumber) return i;
			}
			return this->end();
		}
	};

	///////////////////////////////////////////////////////////////////////////
	/// ストリーム演算子 << を使用した書き出し.
	/// 現時点では完全な対応となっていないため、そのまま書き出しても、
	/// 一時ファイルやustファイルとしての使用は出来ない。
	/// @todo	一時ファイルへの完全対応
	/// @memo	ustファイルへの対応は別の型へのキャストなどを使うべきか・・・
	///////////////////////////////////////////////////////////////////////////
	std::ostream& operator <<(std::ostream& stream, const SectionVector& section){
		// SETTING を書き出す
		stream << "[#SETTING]"			<< std::endl;
		stream << section.cachePath		<< std::endl;
		stream << section.ustPath		<< std::endl;
		stream << section.tempo			<< std::endl;

		// 中身を書き出す（適当）
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
	/// ustファイルや一時ファイルの読み込み.
	/// static なメソッドの詰め合わせなので、
	/// クラスメソッドとして呼び出して使用してください。
	/// @author	ちえ
	/// @date	2012/01/20
	/// @sa		UTAU::Section UTAU::SectionVector
	/*************************************************************************/
	class UstReader{

	private:
		///////////////////////////////////////////////////////////////////////////
		// private クラス内クラス ※本当は切り分けたいが相互依存が強くめんどう
		///////////////////////////////////////////////////////////////////////////
		
		/*************************************************************************/
		/// セクションのパーサーの基底クラス.
		/// 行の種類を返す UTAU::UstReader::Parser::whatIsLine() と、
		/// 継承のための純粋仮想関数 parse() を持つ。
		/// @author	ちえ
		/// @date	2012/01/20
		/// @sa		UTAU::UstReader
		/*************************************************************************/
		class Parser{
		public:
			///////////////////////////////////////////////////////////////////////////
			// public 変数
			///////////////////////////////////////////////////////////////////////////
			
			///////////////////////////////////////////////////////////////////////////
			/// デストラクタ.
			/// 継承するため念のため virtual を使用
			///////////////////////////////////////////////////////////////////////////
			virtual ~Parser(){};
			
			///////////////////////////////////////////////////////////////////////////
			/// パースを行う純粋仮想関数.
			/// 継承時にそれぞれのパーサがこのメソッドの中身を実装する
			/// @param	buffer	ファイルの1行 std::string
			/// @param	vector	セクションを保存する UTAU::SectionVector
			/// @param	file	読み込み中のファイル std::ifstream
			///////////////////////////////////////////////////////////////////////////
			virtual void parse(std::string& buffer, SectionVector& vector, std::ifstream& file) = 0;

			///////////////////////////////////////////////////////////////////////////
			/// 与えられた文字列がustや一時ファイルでなんの行なのかを特定する.
			/// @param	buffer	ファイルの1行 std::string
			/// @return	行の種類を UTAU::LineType で返す
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

				// セッティング
				if(buffer.compare(0, settingMarker.length(), settingMarker) == 0)		return SettingSection;
				// 選択範囲の前のノート
				else if(buffer.compare(0, prevMarker.length(), prevMarker) == 0)		return PrevSection;
				// 選択範囲の次のノート
				else if(buffer.compare(0, nextMarker.length(), nextMarker) == 0)		return NextSection;
				// トラックの終了
				else if(buffer.compare(0, trackEndMarker.length(), trackEndMarker) == 0)return TrackEndSection;
				// 通常のセクション
				else if(buffer.compare(0,sectionMarker.length(), sectionMarker) == 0 &&
					buffer[sectionMarker.length()] >= '0' && buffer[sectionMarker.length()] <= '9')	return NormalSection;
				// その他のセクション
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
		/// [#SETTING]を読むためのパーサ.
		/// UTAU::UstReader::Parser を継承する
		/// @author	ちえ
		/// @date	2012/01/20
		/// @sa		UTAU::UstReader::Parser
		/*************************************************************************/
		class SettingParser : public Parser {
		public:
			void parse(std::string& buffer, SectionVector& vector, std::ifstream& file){
				while(std::getline(file, buffer)){
					
					LineType type = whatIsThisLine(buffer);
					
					// 次のセクションの頭だったら戻る
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
						// = 前と後に分けて未知のパラメータとして保存
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
		/// 読み飛ばすためのパーサ.
		/// UTAU::UstReader::Parser を継承する。ただし、コンストラクタで、
		/// 何のセクションを読み飛ばすのかを UTAU::Section::SectionType で指定する。
		/// @author	ちえ
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
				// 知らないセクションだったらセクション名を保持 （ただし現在は未知のセクションはparse()ではじいている）
				if(section.type == Section::SectionUnknown){
					// # から終わりまでを抜き出すして名前に入れる
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
		/// 通常のセクションを読むためのパーサ.
		/// UTAU::UstReader::Parser を継承する。通常のセクションは[#数字]で表される。
		/// @author	ちえ
		/// @date	2012/01/20
		/// @sa		UTAU::UstReader::Parser
		/*************************************************************************/
		class NormalParser : public Parser {
		public:
			void parse(std::string& buffer, SectionVector& vector, std::ifstream& file){
				
				Section section;

				// セクションナンバーは4桁固定
				std::stringstream(buffer.substr(buffer.find("#")+1 , 4)) >> section.number;
				// セクションタイプを設定
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
						// = 前と後に分けて未知のパラメータとして保存
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
		// クラス内クラスここまで
		///////////////////////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////////////////////
		/// ustファイルや一時ファイルの1行を読み込みパースを行う.
		/// 実際のパースは UTAU::UstReader::Parser を継承した、各種パーサーが行う。
		/// @param	vector	セクションを保存する UTAU::SectionVector
		/// @param	file	読み込み中のファイル std::ifstream
		/// @sa		UTAU::UstReader::Parser	
		///////////////////////////////////////////////////////////////////////////
		static void parse(std::ifstream& file, SectionVector& vector){

			// バッファ
			std::string		buffer;

			// まず1行読み込む
			if(!std::getline(file, buffer)) return;

			// ファイルの終わりまでループ
			while(file){

				// パーサー
				std::unique_ptr<Parser> parser;

				// セクションのタイプによって処理を分ける
				switch(Parser::whatIsThisLine(buffer)){
				case SettingSection:
					parser.reset(new SettingParser());break;
				case PrevSection:
					parser.reset(new SkipParser(Section::SectionPrev));break;
				case UnknownSection:
					//parser.reset(new SkipParser(Section::SectionUnknown));break;
					return; // 未知のセクションは扱わない
				case NormalSection:
					parser.reset(new NormalParser());break;
				case NextSection: return;
					parser.reset(new SkipParser(Section::SectionNext));break;
				case TrackEndSection: return;
				}

				// 指定したパーサーでパースする
				parser->parse(buffer, vector, file);
			}
		}

	public:

		///////////////////////////////////////////////////////////////////////////
		/// 指定されたパスを開いてパースを呼び出す.
		/// @param	path	ファイルパスを表す std::string(オーバーロードで char* も)
		/// @param	vector	セクションを保存するための UTAU::SectionVector
		/// @return	ファイルを開けたら true 開けなかったら false
		/// @sa		UTAU::UstReader::parse()
		/// @note	この実装ではパースを呼び出さず位置から構築して書き出す方法がない
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
		/// 指定されたパスを開いてパースを呼び出す.
		/// @param	path	ファイルパスを表す char* (オーバーロードで std::string も)
		/// @param	vector	セクションを保存するための UTAU::SectionVector
		/// @return	ファイルを開けたら true 開けなかったら false
		/// @sa		UTAU::UstReader::parse()
		/// @note	この実装ではパースを呼び出さず位置から構築して書き出す方法がない
		///////////////////////////////////////////////////////////////////////////
		static bool open(const char* path, SectionVector& vector){
			return open(std::string(path), vector);
		}
	};
}
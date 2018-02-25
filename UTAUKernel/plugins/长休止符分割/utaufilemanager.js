// utaufilemanager モジュール
// UTAUプラグインが行うファイル入出力に便宜を提供する。

var fso = new ActiveXObject("Scripting.FileSystemObject");

//	oto.iniを読み込むオブジェクト
var OtoIni = function() {
	this.Path = "";
	this.Prefix = "";
	this.Table = new Array();
};

OtoIni.prototype.load = function(path, prefix) {
	if(!fso.FileExists(path)) return null;
	var table = new Array();
	var fr = fso.OpenTextFile(path, 1);
	while(!fr.AtEndOfStream) {
		var value = fr.readLine().replace(/[\r\n]/g, "").split(/,/);
		var name = value[0].split(/=/);
		table.push({
			Name: name[0] ? name[0] : "",
			Alias: name[1] ? name[1] : "",
			Offset: value[1] ? (value[1] - 0) : 0.0,
			Consonant: value[2] ? (value[2] - 0) : 0.0,
			Blank: value[3] ? (value[3] - 0) : 0.0,
			PreUtterance: value[4] ? (value[4] - 0) : 0.0,
			VoiceOverlap: value[5] ? (value[5] - 0) : 0.0
		});
	}
	fr.close();
	this.Path = path;
	this.Table = table;
	this.Prefix = prefix;
};
// OtoIni.prototype.save 未実装

//	prefix.mapを読み込むオブジェクト
var PrefixMap = function() {
	this.Path = "";
	this.Table = new Array();
};

PrefixMap.prototype.load = function(path) {
	if(!path.match(/prefix\.map$/i)) path = fso.BuildPath(path, "prefix.map");
	if(!fso.FileExists(path)) return false;
	var table = new Array();
	var fr = fso.OpenTextFile(path, 1);
	while(!fr.AtEndOfStream) {
		fr.readLine().match(/^(.*?)\t(.*?)\t(.*?)[\r\n]*$/);
		table.push({
			Note: RegExp.$1 ? RegExp.$1 : "",
			Prefix: RegExp.$2 ? RegExp.$2 : "",
			Suffix: RegExp.$3 ? RegExp.$3 : ""
		});
	}
	fr.close();
	this.Path = path;
	this.Table = table;
	return true;
};
// PrefixMap.prototype.save 未実装

//	ライブラリ内の複数のoto.iniを読み込むオブジェクト
var OtoIniManager = function() {
	this.Path = "";
	this.Table = new Array();
	this.PrefixMap = new PrefixMap();
}

OtoIniManager.prototype.loadOtoIni = function(path) {
	if(!fso.FolderExists(path)) return false;
	var root = fso.GetFolder(path);
	var subs = new Enumerator(root.SubFolders);
	for(; !subs.atEnd(); subs.moveNext()) {
		var subpath = fso.BuildPath(subs.item().Path, "oto.ini");
		if(fso.FileExists(subpath)) {
			var oto = new OtoIni();
			oto.load(subpath, subs.item().Name);
			this.Table.push(oto);
		}
	}
	var subpath = fso.BuildPath(root.Path, "oto.ini");
	if(fso.FileExists(subpath)) {
		var oto = new OtoIni();
		oto.load(subpath, "");
		this.Table.push(oto);
	}
	this.Path = path;
	return true;
}

OtoIniManager.prototype.loadPrefixMap = function(path) {
	return this.PrefixMap.load(path);
}

OtoIniManager.prototype.load = function(path) {
	if(this.loadOtoIni(path)) {
		this.loadPrefixMap(path);
		this.Path = path;
		return true;
	}
	return false;
}

//	USTファイルを読み込むオブジェクト
var USTFile = function() {
	this.Path = "";
	this.Table = new Array();
}

USTFile.prototype.load = function(path) {
	if(!fso.FileExists(path)) return false;
	var table = new Array();
	var fr = fso.OpenTextFile(path, 1);
	var section = {
		Header: undefined,
		Content: {}
	};
	while(!fr.AtEndOfStream) {
		var line = fr.readLine().replace(/[\r\n]/g, "");
		if(line.match(/^\[#(.*?)\]$/)) {
			if(section.Header != undefined) {
				table.push(section);
				section = {
					Header: undefined,
					Content: {}
				};
			}
			section.Header = RegExp.$1;
		} else if(line.match(/=/)) {
			var name = RegExp.leftContext;
			var value = RegExp.rightContext;
			section.Content[name] = value;
		}
	}
	if(section.Header != undefined) {
		table.push(section);
	}
	fr.close();
	this.Path = path;
	this.Table = table;
	return true;
};

USTFile.prototype.save = function(path) {
	if(path == undefined) path = this.Path;
	var fw = fso.OpenTextFile(path, 2, true);
	for(var i = 0; i < this.Table.length; i++) {
		fw.writeLine("[#" + this.Table[i].Header + "]");
		for(var name in this.Table[i].Content) {
			var value = this.Table[i].Content[name];
			fw.writeLine(name + "=" + value);
		}
	}
	fw.close();
	this.Path = path;
	return true;
};
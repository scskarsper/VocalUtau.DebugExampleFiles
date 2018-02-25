//	htahelperモジュール
//	HTAのシステム上の不便を補完するモジュール。

//	Arrayの拡張
//	多重配列のコピー
Array.prototype.recursiveClone = function() {
 var tmp = Array.apply(null, this);
 for(var i = 0; i < tmp.length; i++) {
  if(tmp[i].recursiveClone)
   tmp[i] = tmp[i].recursiveClone();
  }
return tmp;
}

//	コマンドライン引数の解析
var HTAHelper = {
	parseCommandLine: function() {
		var cl = HTAObject.commandLine;
		var mode = 0;
		var stp = 0;
		var result = new Array();

		for(var i = 0; i < cl.length; i++) {
			switch(cl.charAt(i)) {
				case " ":
					if(mode == 1) {
						result.push(cl.substring(stp, i));
						mode = 0;
					}
				break;
				case "\"":
					if(mode == 0) {
						stp = i + 1;
						mode = 2;
					} else if(mode == 2) {
						result.push(cl.substring(stp, i));
						mode = 0;
					}
				break;
				default:
					if(mode == 0) {
						stp = i;
						mode = 1;
					}
				break;
			}
		}
		if(mode != 0) {
			result.push(cl.substring(stp, i));
		}
		return result;
	}
};

// オブジェクトのメンバの一覧を得る（デバッグ用）
function explore(obj, space) {
	switch(typeof obj) {
		case "function" :
			return "(function)\n";
		case "object":
			var r = "\n";
			for(var n in obj) {
				r += space + n + ": " + explore(obj[n], space + " "); 
			}
			return r;
		default:
			return obj + "\n";
	}
}

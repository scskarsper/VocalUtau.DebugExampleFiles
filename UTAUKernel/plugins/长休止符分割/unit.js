//	unitモジュール
//	単位/記法を変換する関数のモジュール。

//	KeyNoteコレクション
// 	KeyNote.toNum(expr) exprの文字列をキー数に変換する
// 	KeyNote.toExpr(num) numの数値をキー文字列に変換する
var KeyNote = {
	toNum: function(expr) {
		if(typeof expr != "string") return undefined;
		var table = "C#D#EF#G#A#B";
		var sharp = (expr.charAt(1) == "#") ? 1 : 0;
		var key = table.indexOf(expr.substr(0, 1 + sharp).toUpperCase());
		var octave = expr.substr(1 + sharp) - 0 + 1;
		return (key >= 0) ? (octave * 12 + key + sharp) : undefined;
	},
	toExpr: function(num) {
		if(typeof num != "number") return undefined;
		var table = "C#D#EF#G#A#B";
		var octave = Math.floor(num / 12) - 1;
		var keynum = num % 12;
		var sharp = (table.charAt(keynum) == "#") ? 1 : 0;
		return (num >= 0) ? (table.substr(keynum - sharp, 1 + sharp) + octave) : undefined;
	}
};

//	TimeScaleコレクション
//	TimeScale.getTick(msec, tempo)	Tickに変換する
//	TimeScale.getMsec(tick, tempo)	ミリ秒に変換する
//	どちらも浮動小数を返すので、必要に応じてMath.floor()等を施すこと
var TimeScale = {
	toTick: function(msec, tempo) {
		return msec * tempo / 125.0;
	},
	toMsec: function(tick, tempo) {
		return 125.0 * tick / tempo;
	}
};
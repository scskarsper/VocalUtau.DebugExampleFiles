//	unit���W���[��
//	�P��/�L�@��ϊ�����֐��̃��W���[���B

//	KeyNote�R���N�V����
// 	KeyNote.toNum(expr) expr�̕�������L�[���ɕϊ�����
// 	KeyNote.toExpr(num) num�̐��l���L�[������ɕϊ�����
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

//	TimeScale�R���N�V����
//	TimeScale.getTick(msec, tempo)	Tick�ɕϊ�����
//	TimeScale.getMsec(tick, tempo)	�~���b�ɕϊ�����
//	�ǂ��������������Ԃ��̂ŁA�K�v�ɉ�����Math.floor()�����{������
var TimeScale = {
	toTick: function(msec, tempo) {
		return msec * tempo / 125.0;
	},
	toMsec: function(tick, tempo) {
		return 125.0 * tick / tempo;
	}
};
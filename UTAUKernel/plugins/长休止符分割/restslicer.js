// restslicerƒ‚ƒWƒ…[ƒ‹

var ust;

function setup() {
	ust = new USTFile();
	ust.load(HTAHelper.parseCommandLine()[1]);
}

function start() {
	var duration = document.getElementById("duration").value;
	var durationinto = document.getElementById("durationinto").value;
	if(durationinto <= 0) {
		alert("•ªŠ„Œã‚Ì’·‚³‚É‚Í³‚Ì’l‚ğİ’è‚µ‚Ä‚­‚¾‚³‚¢");
		return false;
	}
	for(var i = ust.Table.length - 1; i >= 0; i--) {
		if(ust.Table[i].Header.match(/\D/)) {
			continue;
		}
		if(ust.Table[i].Content["Lyric"] == "R" && ust.Table[i].Content["Length"] - duration >= 0) {
			var number = Math.floor(ust.Table[i].Content["Length"] / durationinto);
			var excess = ust.Table[i].Content["Length"] % durationinto;
			var addlist = new Array();
			for(var j = 0; j < number; j++) {
				addlist.push({
					Header: "INSERT",
					Content: {
						Length: durationinto,
						NoteNum: ust.Table[i].Content["NoteNum"],
						Lyric: "R"
					}
				});
			}
			switch(document.getElementById("excess").value) {
				case "addfirst":
					addlist[0].Content["Length"] = excess + (addlist[0].Content["Length"] - 0);
					break;
				case "addlast":
					addlist[addlist.length - 1].Content["Length"] = excess + (addlist[addlist.length - 1].Content["Length"] - 0);
					break;
				case "first":
					if(excess == 0) break;
					addlist.unshift({
						Header: "INSERT",
						Content: {
							Length: excess,
							NoteNum: ust.Table[i].Content["NoteNum"],
							Lyric: "R"
						}
					});
					break;
				case "last":
					if(excess == 0) break;
					addlist.push({
						Header: "INSERT",
						Content: {
							Length: excess,
							NoteNum: ust.Table[i].Content["NoteNum"],
							Lyric: "R"
						}
					});
					break;
				case "floor":
					break;
				case "ceil":
					if(excess == 0) break;
					addlist.push({
						Header: "INSERT",
						Content: {
							Length: durationinto,
							NoteNum: ust.Table[i].Content["NoteNum"],
							Lyric: "R"
						}
					});
					break;
			}
			ust.Table[i].Header = "DELETE";
			for(var j = addlist.length - 1; j >= 0 ; j--) {
				ust.Table.splice(i, 0, addlist[j]);
			}
		}
	}
	ust.save();
	this.close();
	return true;
}

function cancel() {
	this.close();
}
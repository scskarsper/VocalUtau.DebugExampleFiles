var kanatable = new Array();
var romantable = new Array();

kanatable[0] = "あかがさざただなはばぱまやらわぁゃゎアカガサザタダナハバパマヤラワァャヮ"; romantable[0] = "a";
kanatable[1] = "いきぎしじちぢにひびぴみりゐぃイキギシジチヂニヒビピミリヰィ"; romantable[1] = "i";
kanatable[2] = "うくぐすずつづぬふぶぷむゆるぅゅウクグスズツヅヌフブプムユルゥュヴ"; romantable[2] = "u";
kanatable[3] = "えけげせぜてでねへべぺめれゑぇエケゲセゼテデネヘベペメレヱェ"; romantable[3] = "e";
kanatable[4] = "おこごそぞとどのほぼぽもよろをぉょオコゴソゾトドノホボポモヨロヲぉょ"; romantable[4] = "o";
kanatable[5] = "んン"; romantable[5] = "n";
kanatable[6] = "R"; romantable[6] = "-";

var fso = new ActiveXObject("Scripting.FileSystemObject");

if(!WScript.Arguments.Unnamed(0)) {
 WScript.Echo("本スクリプトはUTAUプラグインです。UTAUからご利用ください。");
 WScript.Quit();
}

var fr = fso.OpenTextFile(WScript.Arguments.Unnamed(0), 1);
var fw = fso.OpenTextFile("diphonize_temp.txt", 2, true);

var mode = -1;
var prom = 6;

while(!fr.AtEndOfStream) {

 var s = fr.ReadLine();

 if(s.charAt(0) == "[") {
  if(s.indexOf("[#SETTING]") >= 0) mode = 0;
  else if(s.indexOf("[#PREV]") >= 0) mode = 1;
  else if(s.indexOf("[#NEXT]") >= 0) mode = 3;
  else mode = 2;
 }

 if(s.match(/Lyric=(.+)/)) {
  var k = s.charAt(s.length - 1);
  for(var i = 0; i < 6; i++) {
   if(kanatable[i].indexOf(k) >= 0) break;
  }
  if(mode == 2 && i < 6) {
   s = "Lyric=" + romantable[prom] + " " + RegExp.$1;
  }
  prom = i;
 }

 fw.WriteLine(s);
}

fr.Close();
fw.Close();

fso.GetFile("diphonize_temp.txt").Copy(WScript.Arguments.Unnamed(0), true);
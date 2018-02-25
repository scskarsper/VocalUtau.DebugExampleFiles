var fso = new ActiveXObject("Scripting.FileSystemObject");

if(!WScript.Arguments.Unnamed(0)) {
 WScript.Echo("本スクリプトはUTAUプラグインです。UTAUからご利用ください。");
 WScript.Quit();
}

var fr = fso.OpenTextFile(WScript.Arguments.Unnamed(0), 1);
var fw = fso.OpenTextFile("back2cv_temp.txt", 2, true);

while(!fr.AtEndOfStream) {

 var s = fr.ReadLine();

 if(s.charAt(0) == "[") {
  if(s.indexOf("[#SETTING]") >= 0) mode = 0;
  else if(s.indexOf("[#PREV]") >= 0) mode = 1;
  else if(s.indexOf("[#NEXT]") >= 0) mode = 3;
  else mode = 2;
 }

 if(mode == 2 && s.match(/Lyric=[aeioun\-] (\D+)\d?/)) {
  s = "Lyric=" + RegExp.$1;
 }

 fw.WriteLine(s);
}

fr.Close();
fw.Close();

fso.GetFile("back2cv_temp.txt").Copy(WScript.Arguments.Unnamed(0), true);
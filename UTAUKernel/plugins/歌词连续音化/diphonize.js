var kanatable = new Array();
var romantable = new Array();

kanatable[0] = "���������������Ȃ͂΂ς܂��킟���A�J�K�T�U�^�_�i�n�o�p�}�������@����"; romantable[0] = "a";
kanatable[1] = "���������������ɂЂт҂݂��C�L�M�V�W�`�a�j�q�r�s�~�����B"; romantable[1] = "i";
kanatable[2] = "�����������ÂʂӂԂՂނ�那��E�N�O�X�Y�c�d�k�t�u�v�������D����"; romantable[2] = "u";
kanatable[3] = "�����������Ăł˂ւׂ؂߂��G�P�Q�Z�[�e�f�l�w�x�y�������F"; romantable[3] = "e";
kanatable[4] = "�����������Ƃǂ̂قڂۂ���������I�R�S�\�]�g�h�m�z�{�|������������"; romantable[4] = "o";
kanatable[5] = "��"; romantable[5] = "n";
kanatable[6] = "R"; romantable[6] = "-";

var fso = new ActiveXObject("Scripting.FileSystemObject");

if(!WScript.Arguments.Unnamed(0)) {
 WScript.Echo("�{�X�N���v�g��UTAU�v���O�C���ł��BUTAU���炲���p���������B");
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
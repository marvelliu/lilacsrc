<?php
require("wapfuncs.php");
if(loginok())
{
  if ($userid == "guest")
    {
      authpanic("�����޷���������");
    }
  else
    {
      $target = $_GET['to'];
      @$origmsg = $_GET['m'];
      $msg = smarticonv($origmsg);
      @$origtitle = $_GET['t'];
      $title = smarticonv($origtitle);
      waphead(0);
      echo "<card id=\"main\" title=\"".BBS_WAP_NAME."\">";
      echo "<p>";
      if(empty($msg) || empty($title) || empty($target)){
	showmailform($target);
	}
      else {
	bbswapmail($target, $title, $msg);
      }
      echo "</p>";
    }
}
?>
</card>
</wml>

<?php
function bbswapmail($target, $title, $content)
{
  $retn = bbs_postmail($target, preg_replace("/\\\(['|\"|\\\])/","$1",$title), preg_replace("/\\\(['|\"|\\\])/","$1",$content), 0, 0);
  switch ($retn) {
  case -1:
    echo "����ʧ��:�޷������ļ�<br/>";
    break;
  case -2:
    echo "����ʧ��:�Է���������ʼ�<br/>";
    break;
  case -3:
    echo "����ʧ��:�Է�������<br/>";
    break;
  default:
    echo "�ɹ��ĳ�<br/>";
  }
  echo "<do type=\"prev\" label=\"����\"><prev/></do>";
  return;
}

function showmailform($target)
{
   echo "������<input emptyok=\"true\" name=\"to\" size=\"20\""
     .((empty($target))?"":" value=\"".$target."\"")."/><br/>";
  echo "����<input emptyok=\"true\" name=\"t\" size=\"20\"/><br/>";
  echo "����<input emptyok=\"true\" name=\"m\" size=\"80\"/><br/>";
  showlink(urlstr('writemail',array('m'=>'$(m)','t'=>'$(t)','to'=>'$(to)')),"����");
  echo "<br/>";
  showlink(urlstr("menu"),"���˵�");
}
?>
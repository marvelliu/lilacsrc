<?php
require("wapfuncs.php");
if(loginok())
{
  setlocale(LC_ALL, "zh_CN");
  $time = strftime("<br/> %T <br/> %A %D <br/>");
  $uolnum = bbs_getonlinenumber();
  $webnum = bbs_getwwwguestnumber();
  waphead(0);
  checkmm();
  echo "<card id=\"main\" title=\"".BBS_WAP_NAME."\">";
  echo "<p>";
  echo "ϵͳ��׼ʱ��$time";
  echo "Ŀǰ���� $uolnum ��<br/>($webnum WWW GUEST)<br/>";
  echo "</p>";
}
?>
</card>
</wml>

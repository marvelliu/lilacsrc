<?php
require("wapfuncs.php");

//************************

if(loginok())
{
  waphead(0);
  if ($userid != 'guest') checkmm();
  echo "<card id=\"main\" title=\"".BBS_WAP_NAME."\">";
  echo "<p align=\"center\">���˵�</p>";
  echo "<p>";
  showlink(urlstr('top10'),"ʮ����");echo "<br/>";
  if($userid != 'guest')
    {
      showlink(urlstr('favboard'),"���˶���");echo "<br/>";
      showlink(urlstr('onlinefriend'),"���ߺ���");echo "<br/>";
      showlink(urlstr('mailbox'),"�����ʼ�");echo "<br/>";
      showlink(urlstr('sendmsg'),"����ѶϢ");echo "<br/>";
    }
  echo "<input emptyok=\"true\" type=\"text\" name=\"a\" size=\"10\"/><br/>";
  showlink(urlstr('showuser',array('n'=>'$(a)')),"��ѯ����");echo "<br/>";
  showlink(urlstr('searchboard',array('n'=>'$(a)')),"��ѯ����");echo "<br/>";
  showlink(urlstr('sysinfo'), "ϵͳ��Ϣ"); echo "<br/>";
  showlink(urlstr('logout'),"�˳�");echo "<br/>";
  echo "</p>";
}
?>
</card>
</wml>

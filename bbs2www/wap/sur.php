<?php
require("wapfuncs.php");
if(loginok())
{
  $id = $_GET["n"];
  $lookupuser = array ();
  waphead(0);
  echo "<card id=\"main\" title=\"".BBS_WAP_NAME."\">";
  
  if ($id=="" || (bbs_getuser($id, $lookupuser) == 0))
    {
      echo "<p align=\"center\">";
      echo "���û�������<br/>";
      showlink(urlstr("menu"),���˵�);
      echo "</p>";
    }
  else
    {
      $usermodestr = bbs_getusermode($id);
      echo "<p>";
      echo $lookupuser["userid"].'('
	.htmlspecialchars(stripansi($lookupuser["username"])).')';
      echo "��վ".$lookupuser["numlogins"]."�Σ�����"
	.$lookupuser["numposts"]."ƪ���¡�<br/>";
      echo "�ϴ���".date("D M j H:i:s Y",$lookupuser["lastlogin"]);
      echo "<br/>��[".substr($lookupuser["lasthost"], 0, 
			strrpos($lookupuser["lasthost"],"."))
	."***]��վ��<br/>";
      echo "����ʱ��";
      if( $usermodestr!="" && $usermodestr{0}=="1" )
	{
	  echo date("[D M j H:i:s Y]", $lookupuser["lastlogin"]+60+( $lookupuser["numlogins"]+$lookupuser["numposts"] )%100 );
	} else if($lookupuser["exittime"] < $lookupuser["lastlogin"])
	  echo "[����]";
      else
	echo date("[D M j H:i:s Y]", $lookupuser["exittime"]);
      echo "<br/>";
      if( bbs_checknewmail($lookupuser["userid"]) ) echo "������<br/>";
      echo '������['.bbs_compute_user_value($lookupuser["userid"]).'],<br/>';
      echo '���['.bbs_user_level_char($lookupuser["userid"]).']<br/>';
      if ($usermodestr!="" && $usermodestr{1} != "")
	echo ereg_replace("\n","<br/>",stripansi(strip_tags(substr($usermodestr, 1))));
      echo "<br/>";
      showlink(urlstr("menu"),���˵�);
      echo "</p>";
    }
}
?>
</card>
</wml>

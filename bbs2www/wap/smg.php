<?php
require("wapfuncs.php");
if(loginok())
{
  if ($userid == "guest")
    {
      authpanic("�����޷�����ѶϢ");
    }
  else
    {
      @$origmsg = $_GET['m'];
      $msg = smarticonv($origmsg);
      @$destid = $_GET['t'];
      @$destutmpstr = $_GET['i'];

      if (!empty($destid))
	{
	  if (empty($destumpstr))
	    {
	      $destutmp=0;
	    }
	  else
	    {
	      $destutmp = decodesession($destutmpstr);
	    }
	  bbs_sendwebmsg($destid, $msg, $destutmp, $errmsg);
	  waphead(0);
	  echo "<card id=\"main\" title=\"".BBS_WAP_NAME."\">";
	  echo "<p>$errmsg</p>";
	  echo "<do type=\"prev\" label=\"����\"><prev/></do>";
	}
      else
	{
	  waphead(0);
	  checkmm();
          echo "<card id=\"main\" title=\"".BBS_WAP_NAME."\">";
	  echo "<p>";
	  echo "��<input emptyok=\"true\" name=\"t\" size=\"7\"/><br/>";
	  echo "˵<input emptyok=\"true\" name=\"m\" size=\"20\"/><br/>";
          echo "<a href=\"".urlstr('sendmsg',array('m'=>'$(m)','t'=>'$(t)'))."\">����</a>";
	  echo "</p>";
	  echo "<p>";
	  showlink(urlstr('menu'),"���˵�");
	  echo "</p>";
	}
    }
}
?>
</card>
</wml>

<?php
require("wapfuncs.php");
if(loginok())
{
  $brdstr = $_GET[n];
  $argv = decarg("searchboard",$_GET['n']);
  $page = intval($argv["page"]);
  waphead(0);
  echo "<card id=\"main\" title=\"".BBS_WAP_NAME."\">";  
  echo "<p/>";
  echo "δ���<br/>";
  showlink(urlstr("menu"),"���˵�");
}
?>
</card>
</wml>

<?php
require("wapfuncs.php");
if(loginok())
{
  if ($userid == "guest")
    {
      authpanic("guestû������");
    }
  else
    {
      $argv = decarg("listmail",$_GET['x']);
      $mboxnum = intval($argv["mbox"]);
      $page = intval($argv["page"]);
      $mailboxes = loadmboxes($userid);
      waphead(0);
      echo "<card id=\"main\" title=\"".BBS_WAP_NAME."\">";
      echo "<p>";
      if ($mboxnum >= count($mailboxes["path"])) {
	echo "����������<br/>";
	}
      else {
	$pagesize = 10; ////assume 60bytes/entry,so 60*10=600bytes~1kbytes
	$mail_fullpath = bbs_setmailfile($userid,$mailboxes["path"][$mboxnum]);
	$total = bbs_getmailnum2($mail_fullpath);
	$start = ($page - 1) * $pagesize + 1;
	if(( $page == 0 )||( $start > $total)) {
	  $start = max(1,$total-$pagesize+1);
	  $page = intval(($start+$pagesize-2) / $pagesize) + 1;
	}
	$maildata = bbs_getmails($mail_fullpath,$start, min($pagesize,$total-
$start+1));
	
	if ($maildata == FALSE) echo "��ȡ�ʼ��б�ʧ��";
	else {
	  $count = 0;
	  foreach ($maildata as $eachmail){
	    if(stristr($maildata[$count]["FLAGS"],"N")){
	      echo "*";
	    }
	    else {
	      echo ".";
	    }
	    $from = $maildata[$count]["OWNER"];
	    $title = $maildata[$count]["TITLE"];
	    if (strncmp($title, "Re: ", 4) != 0)
	      $title = "��  " . $title;
	    echo "[".$from."]";
	    showlink(urlstr("readmail",array(),encarg("readmail",array("mbox"=>$mboxnum,"mailid" => ($start+$count)))),htmlspecialchars($title));
	    echo "<br/>";
	    $count ++;
	  }
	  if($page > 1)showlink(urlstr("listmail",array(),encarg("listmail",array("mbox"=>$mboxnum,"page"=>($page - 1)))),"��һҳ");
	  if(($start + $count) < $total) showlink(urlstr("listmail",array(),encarg("listmail",array("mbox"=>$mboxnum,"page"=>($page + 1)))),"��һҳ");
	  echo "<br/>";
	}
	
      }
      showlink(urlstr("menu"),"���˵�");
      echo "</p>";      
    }
}
?>
</card>
</wml>

<?php
require("wapfuncs.php");
if(loginok())
{
  if ($userid == "guest")
    {
      authpanic("����û������");
    }
  else
    {
      $argv = decarg("readmail",$_GET['x']);
      $mboxnum = intval($argv["mbox"]);
      $mailid = intval($argv["mailid"]);
      $page = intval($argv["page"]);
      $mailboxes = loadmboxes($userid);
      waphead(0);
      echo "<card id=\"main\" title=\"".BBS_WAP_NAME."\">";
      echo "<p>";
      if ($mboxnum >= count($mailboxes["path"])) {
        echo "����������<br/>";
      }
      else {
	$dir = "mail/".strtoupper($userid{0})."/".$userid."/".$mailboxes["path"][$mboxnum];
	$articles = array ();
        if( bbs_get_records_from_num($dir, $mailid, $articles) ) {
	  $file = $articles[0]["FILENAME"];
	  $mailfrom = $articles[0]["OWNER"];
	  $filename = "mail/".strtoupper($userid{0})."/".$userid."/".$file ;
	  if(! file_exists($filename)){
	    echo "�ż�������...<br/>";
	  }
	  else{
	    wapshowmail($filename, $page, $mboxnum, $mailid, $mailfrom);
	    bbs_setmailreaded($dir, $mailid);
	  }
        }else{
	  echo "����Ĳ���<br/>";
        }

      }
      showlink(urlstr("menu"),"���˵�");
      echo "</p>";
    }
}
?>
</card>
</wml>

<?php
function wapshowmail($fname, $page, $mbox, $maild, $target)
{
  $pagesize = 800;
  $fhandle = @fopen($fname, "rb");
  $fdata = "";
  $fbuf = "";
  while (!feof($fhandle)) $fdata .= fread($fhandle, 1024);
  fclose($fhandle);

  $fbuf1 = preg_split('/\x0/', $fdata, -1, PREG_SPLIT_NO_EMPTY);
  $fbuf2 = stripansi($fbuf1[0]);
  $fbuf1 = preg_split('/\n/', $fbuf2, 5, PREG_SPLIT_NO_EMPTY);
  $fbody = "";
  if(preg_match('/������: (.*)/', $fbuf1[0], $farray)){
    $fwho = $farray[1];
  }
  else {
    $fwho = "";
    $fbody = $fbuf1[0];
  }
  if(preg_match('/��(\s)*��: (.*)/', $fbuf1[1], $farray)){
    $ftitle = $farray[2];
  }
  else {
    $ftitle = "";
    $fbody = $fbody.$fbuf1[1];
  }

  if(preg_match('/����վ: (.*)\((.*)\)(.*)/', $fbuf1[2], $farray)){
    $ftime = $farray[2];
  }
  else {
    $fsite = "";
    $fbody = $fbody.$fbuf1[2];
  }
  if(preg_match('/��(\s)*Դ: (.*)/', $fbuf1[3], $farray)){
    $fwhere = $farray[2];
  }
  else {
    $where = "";
    $fbody = $fbody.$fbuf1[3];
  }

  if(strncmp($ftitle, "Re: ", 4) != 0)
    $ftitle = "��".$ftitle;
  $ftime = preg_replace("/[\t\ ]+/", '.', $ftime);
  $fbody = $fwho."[".$ftime."][".$fwhere."]\n".$ftitle."\n".$fbody."\n".$fbuf1[4];
  $startnum = $page * $pagesize;
  $endnum = min($startnum+$pagesize,strlen($fbody));
  if($startnum < strlen($fbody)){
    $fbuf1 = ch_substr($fbody, $startnum, $pagesize);
    $fbuf2 = preg_replace("/[\t\ ]*/m", "", $fbuf1);
    $fbuf = preg_replace("/(\n)+/s","\n",$fbuf2);
    echo preg_replace("/\n/","<br/>",htmlspecialchars($fbuf));
    echo "<br/>";
    if($page > 0)
      showlink(urlstr("readmail",array(),encarg("readmail",array("mbox"=>$mbox, "mailid"=>$mailid, "page"=>($page -1)))),"��һҳ");
    if($endnum < strlen($fbody)) {
      showlink(urlstr("readmail",array(),encarg("readmail",array("mbox"=>$mbox, "mailid"=>$mailid, "page"=>($page+1)))),"��һҳ");
    }
    else {
      showlink(urlstr("writemail",array("to"=>$target)),"�ظ�");
    }
    echo "<br/>";
  }
  else echo "�����ҳ��<br/>";
}
?>
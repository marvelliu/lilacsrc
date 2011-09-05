<?php
require("wapfuncs.php");
if(loginok())
{
  $argv = decarg("showboard",$_GET['x']);
  $brdnum = intval($argv["board"]);
  $page = intval($argv["page"]);
  $mode = intval($argv["mode"]);

  waphead(0);
  echo "<card id=\"main\" title=\"".BBS_WAP_NAME."\">";
  echo "<p>";
  $brdarr = array();
  if ($brdnum != 0){
    $brdname = bbs_getbname($brdnum);
    if ($brdname){
      if (bbs_checkreadperm($currentuser_num, $brdnum)){
	bbs_getboard($brdname, $brdarr);
	if ($brdarr["FLAG"]&BBS_BOARD_GROUP){
	  $secboards = bbs_getboards($brdarr["SECNUM"], $brdnum,0);
	  if ($secboards == FALSE) echo "��ȡ���б�ʧ��";
	  else {
	    $page = intval($argv["secpage"]);
	    $pagesize = 12; //assume 50bytes/entry,so 50*12=600bytes~1kbytes
	    $startnum = $page * $pagesize;
	    $brd_name = $secboards["NAME"]; // Ӣ����
	    $brd_desc = $secboards["DESC"]; // ��������
	    $brd_class = $secboards["CLASS"]; // �������
	    $brd_bm = $secboards["BM"]; // ����
	    $brd_artcnt = $secboards["ARTCNT"]; // ������
	    $brd_unread = $secboards["UNREAD"]; // δ�����
	    $brd_zapped = $secboards["ZAPPED"]; // �Ƿ� z ��
	    $brd_flag= $secboards["FLAG"];//Ŀ¼��ʶ
	    $brd_bid= $secboards["BID"];//Ŀ¼��ʶ
	    $num = count($brd_name);
	    $endnum = min($num,$startnum + $pagesize);
	    for($i = $startnum; $i < $endnum; $i++)
	      {
		if( $brd_unread[$i] ==-1 && $brd_artcnt[$i] ==-1)
		  continue;
		$brdnum = bbs_getboard($brd_name[$i], $brdarr);
		showlink(urlstr("showboard",array(),encarg("showboard",array("board" => $brdnum))),$brd_name[$i]);
		echo "<br/>";
	      }
	    if ($page >0)
	      showlink(urlstr("showboard",array(),encarg("showboard",array("board"=> $brdnum,"page" => ($page - 1)))),"��һҳ");
	    if ($num > $endnum) {
	      showlink(urlstr("showboard",array(),encarg("showboard",array("board"=> $brdnum,"page" => ($page + 1)))),"��һҳ");
	      echo "<br/>";
	    }
	  }
	}
	else {
	  $pagesize = 10; ////assume 60bytes/entry,so 60*10=600bytes~1kbytes
	  $total = bbs_countarticles($brdnum, $mode);
	  $start = ($page - 1) * $pagesize + 1;
	  if(( $page == 0 )||( $start > $total)) {
	    $start = max(1,$total-$pagesize+1);
	    $page = intval(($start+$pagesize-2) / $pagesize) + 1;
	  }
	  $articles = bbs_getarticles($brdname, $start, min($pagesize,$total-$start+1),$mode);
	  if ($articles == FALSE) echo "��ȡ�����б�ʧ��";
	  else {
	    $count = 0;
	    foreach ($articles as $article){
	      $title = $article["TITLE"];
	      if (strncmp($title, "Re: ", 4) != 0)
		$title = "�� " . $title;
	      echo $article["OWNER"];
	      showlink(urlstr("readpost",array(),encarg("readpost",array("board"=>$brdnum,"id" => $article["ID"]))),htmlspecialchars($title));
	      echo "<br/>";
	      $count ++;
	    }
	    echo "λ��:[".intval(($start+$count-1)*100/$total)."%]<br/>";
	    if($page > 1)showlink(urlstr("showboard",array(),encarg("showboard",array("board"=>$brdnum,"page"=>($page - 1),"mode"=>$mode))),"��һҳ");
	    if(($start + $count) < $total) showlink(urlstr("showboard",array(),encarg("showboard",array("board"=>$brdnum,"page"=>($page + 1),"mode"=>$mode))),"��һҳ");
	    echo "<br/>";
	    $modestr = array(
			     "NORMAL" => "��ͨ��",
			     "DIGEST" => "g����",
			     "MARK" => "m����",
			     "ZHIDING" => "�ö�"
			     );
	    while( list ($key, $mdname) = each ($modestr))
	      {
		if($dir_modes[$key] != $mode) {
		  showlink(urlstr("showboard",array(),encarg("showboard",array("board"=>$brdnum,"mode"=>$dir_modes[$key]))),$modestr[$key]);
		  echo "<br/>";
		}
	      }
	    showlink(urlstr("writepost",array(),encarg("writepost",array("board"=>$brdnum))),"��������");
	    echo "<br/>";
	  }
	}
      }
      else{echo "�����������<br/>";}
    }
    else{echo "�����������<br/>";}
  }
  else{echo "�����������<br/>";}
  showlink(urlstr("menu"),���˵�);
  echo "</p>";  
}
?>
</card>
</wml>


<?php
require("wapfuncs.php");
if(loginok())
{
  if ($userid == "guest")
    {
      authpanic("guestû�����ߺ���");
    }
  else
    {
      $argv = decarg("favboard", $_GET['x']);
      if(!isset($argv["select"])) $select = -1;
      else $select = intval($argv["select"]);
      if($select > 818) $select = 0;
      if(!isset($argv["up"])) $father = -1;
      else $father = intval($argv["up"]);
      if($father > 818) $father = 0;

      $page = intval($argv["page"]);
      $pagesize = 12; //assume 50bytes/entry,so 50*12=600bytes~1kbytes
      $startnum = $page * $pagesize;
      waphead(0);
      checkmm();
      echo "<card id=\"main\" title=\"".BBS_WAP_NAME."\">";
      echo "<p align=\"center\">";
      if (($select < -1) || (bbs_load_favboard($select)==-1))
	{
	  echo "����Ĳ���";
	}
      else
	{
	  $boards = bbs_fav_boards($select, 1);
	  if ($boards == FALSE) echo "��ȡ���б�ʧ��";
	  else {
	    $brd_name = $boards["NAME"]; // Ӣ����
	    $brd_desc = $boards["DESC"]; // ��������
	    $brd_class = $boards["CLASS"]; // �������
	    $brd_bm = $boards["BM"]; // ����
	    $brd_artcnt = $boards["ARTCNT"]; // ������
	    $brd_unread = $boards["UNREAD"]; // δ�����
	    $brd_zapped = $boards["ZAPPED"]; // �Ƿ� z ��
	    $brd_position= $boards["POSITION"];//λ��
	    $brd_flag= $boards["FLAG"];//Ŀ¼��ʶ
	    $brd_bid= $boards["BID"];//Ŀ¼��ʶ
	    $num = count($brd_name);
	    $endnum = min($num,$startnum + $pagesize);
	    for($i = $startnum; $i < $endnum; $i++)
	      {
		if( $brd_unread[$i] ==-1 && $brd_artcnt[$i] ==-1)
		  continue;
		echo ($i+1).".";
		if ($brd_flag[$i] ==-1 )
		  {
		    echo "+";
		    showlink(urlstr("favboard",array(),encarg("favboard",array("select" => transelect($brd_bid[$i]), "up" => transelect($select)))),$brd_desc[$i]);
		    echo "<br/>";
		    continue;
		  }
		$brdarr = array();
                $brdnum = bbs_getboard($brd_name[$i], $brdarr);
		showlink(urlstr("showboard",array(),encarg("showboard",array("board" => $brdnum))),$brd_name[$i]);
		echo "<br/>";
	      }
	    if ($select != -1) {
	      showlink(urlstr("favboard",array(),encarg("favboard",array("select" => transelect($father)))),"��һ��");
	      echo "<br/>";
	    }
	    if ($page >0)
	    showlink(urlstr("favboard",array(),encarg("favboard",array("select" => transelect($select),"page" => ($page - 1)))),"��һҳ");
	    if ($num > $endnum) {
	      showlink(urlstr("favboard",array(),encarg("favboard",array("select" => transelect($select),"page" => ($page + 1)))),"��һҳ");
	      echo "<br/>";
	    }
	  }
	}
      showlink(urlstr("menu"),���˵�);
      echo "</p>";      
      bbs_release_favboard();
    }
}
?>
</card>
</wml>

<?php
function transelect($num)
{
  if($num == 0) return 904;
  if($num == -1) return 0;
  return $num;
}
?>
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
      $argv = decarg("writepost",$_GET['x']);
      $brdnum = intval($argv["board"]);
      $reid = intval($argv["reid"]);
      @$origmsg = $_GET['m'];
      $msg = smarticonv($origmsg);
      @$origtitle = $_GET['t'];
      $title = smarticonv($origtitle);
      waphead(0);
      echo "<card id=\"main\" title=\"".BBS_WAP_NAME."\">";
      echo "<p>";
      
      if( $brdnum != 0 ){
	$brdname = bbs_getbname($brdnum);
	if ($brdname){
	  $brdarr = array();
	  bbs_getboard($brdname, $brdarr);
	  if (bbs_checkreadperm($currentuser_num, $brdnum) &&(!bbs_is_readonly_board($brdarr)) && bbs_checkpostperm($currentuser_num, $brdnum)){
	    $articles = array();
	    if(empty($msg)){//no content,show out post forms
	      if ($reid > 0) {//reply
		$num = bbs_get_records_from_id($brdname, $reid, $dir_modes["NORMAL"], $articles);
		if ($num == 0) {
		  echo "�ظ���ԭ���ºŴ���<br/>";
		}
		else {
		  if(strncmp($articles[1]["TITLE"],"Re: ", 4)!=0)
		    $title = "Re: ".$articles[1]["TITLE"];
		  showpostform($brdnum, $title, $reid);
		}
	      }
	      else showpostform($brdnum, "", 0);
	    }
	    else{ // have content
	      if ($reid > 0) {//reply
		$num = bbs_get_records_from_id($brdname, $reid, $dir_modes["NORMAL"], $articles);
		if ($num == 0) {
		  echo "�ظ���ԭ���ºŴ���<br/>";
		}
		else {
		  if ($articles[1]["FLAGS"][2] == 'y') {
		    echo "���Ĳ��ɻظ�<br/>";
		  }
		  else {
		    if(empty($title)) {
		      if(strncmp($articles[1]["TITLE"],"Re: ", 4)!=0)
			$title = "Re: ".$articles[1]["TITLE"];
		      else $title = $articles[1]["TITLE"];
		    }
		    bbswappost($brdname, $title, $msg, $reid);
		  }
		}
	      }
	      else {
		if(empty($title)) {
		  echo "����ķ�������<br/>";
		}
		else {
		  bbswappost($brdname, $title, $msg, 0);
		}
	      }
	    }
	  }
	  else {
	    echo "û���ڱ��淢���µ�Ȩ��<br/>";
	  }
	}
	else {
	  echo "����İ����<br/>";
	}
      }
      else{
	echo "�����ڴ˰����<br/>";
      }
      echo "</p>";
    }
}
?>
</card>
</wml>

<?php
function bbswappost($board, $title, $content, $reid)
{
  $retn = bbs_postarticle($board, preg_replace("/\\\(['|\"|\\\])/","$1",$title), preg_replace("/\\\(['|\"|\\\])/","$1",$content), 0, $reid, 0, 0);
  switch ($retn) {
  case -1:
    echo "���������������!<br/>";
    break;
  case -2:
    echo "����Ϊ����Ŀ¼��!<br/>";
    break;
  case -3:
    echo "����Ϊ��!<br/>";
    break;
  case -4:
    echo "����������Ψ����, ����������Ȩ���ڴ˷�������!<br/>";
    break;
  case -5:
    echo "�ܱ�Ǹ, �㱻������Աֹͣ�˱����postȨ��!<br/>";
    break;
  case -6:
    echo "���η��ļ������,����Ϣ��������!<br/>";
    break;
  case -7:
    echo "�޷���ȡ�����ļ�! ��֪ͨվ����Ա, лл! <br/>";
    break;
  case -8:
    echo "���Ĳ��ɻظ�!<br/>";
    break;
  case -9:
    echo "ϵͳ�ڲ�����, ��Ѹ��֪ͨվ����Ա, лл!<br/>";
    break;
  case -21:
    echo "���Ļ��ֲ����ϵ�ǰ���������趨, ��ʱ�޷��ڵ�ǰ��������������!<br/>";
    break;
  default:
    echo "���ĳɹ�<br/>";
  }
  echo "<do type=\"prev\" label=\"����\"><prev/></do>";
  return;
}

function showpostform($boardnum,$title,$reid)
{
  echo "����<input emptyok=\"true\" name=\"t\" size=\"20\""
    .((empty($title))?"":" value=\"".$title."\"")."/><br/>";
  echo "����<input emptyok=\"true\" name=\"m\" size=\"80\"/><br/>";
  showlink(urlstr('writepost',array('m'=>'$(m)','t'=>'$(t)'), encarg('writepost',array("board"=>$boardnum,"reid"=>$reid))),"����");
  echo "<br/>";
  showlink(urlstr("menu"),"���˵�");

}
?>

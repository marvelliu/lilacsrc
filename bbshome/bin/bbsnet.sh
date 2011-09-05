#!/bin/sh
#
#	define the standout() and standend() escape sequences...
#
SO="[7m"
SE="[0m"
stty pass8

#biff n

while true
do
clear
echo "---------------------------------------------"
echo "          ${SO} ��j��u�t BBS �A�ȶ��� ${SE}"
echo "---------------------------------------------"
echo "
   ${SO}[0]${SE} �������G��

   ${SO} �ϮѬd�ߨt�� ${SE}
   [1] ��q�j��		[5] ��ߤ����Ϯ��] (����Τ�, �]���ܽ���)
   [2] �M�ؤj��  	[6] �x�_���߹Ϯ��]
   [3] �x�W�j��         [7] ������s�|              
   [4] ���\�j��              

   ${SO}[97]${SE} �]�w 1Discuss �ҥΤ��s�边
   ${SO}[98]${SE} login �H�Ʋέp�Ϫ���v��
   ${SO}[99]${SE} ���� login �H�Ʋέp�Ϫ�
    ${SO}[S]${SE} �N 1Discuss �����Q�װϱƧ�

   ${SO}[Q]${SE} Exit.    
"
#   ${SO}[99]${SE} �i�����ƱƦ�] 

echo -n "�п�� [Q]: " 

        if read CHOICE
           then
	    clear
            time_stamp=`sdate`
            case "${CHOICE}"
              in
                '')
                  break
                  ;;
                'Q')
                  break
                  ;;
		'q')
		  break
		  ;;
                0)
		  LC_CTYPE=iso_8859_1
		  export LC_CTYPE
		  echo "$time_stamp faqview $USER" >> bbslog
		  cd 0Announce
		  faqview '    ��q�j�Ǹ�T�u�{�Ǩt BBS ���G�� (perl ��)'
		  cd ..
                  ;;
                1)
                  echo "$time_stamp bbsnet $USER ��j�Ϯ��]" >> bbslog
                  echo "${SO} �s����j�Ϯ��] ${SE}"
                  rlogin lib1.nctu.edu.tw -l library
                  #expect /expect/nctu_lib
                  ;;
                2)
                  echo "$time_stamp bbsnet $USER �M�j�Ϯ��]" >> bbslog
                  echo "${SO} �s���M�j�Ϯ��] ${SE}"
                  echo "�Х� ${SO}search${SE} login"
                  telnet 140.114.72.2
                  #expect /expect/nthu_lib
                  ;;
                3)
                  echo "$time_stamp bbsnet $USER �x�j�Ϯ��]" >> bbslog
                  echo "${SO} �s���x�j�Ϯ��] ${SE}"
                  echo "�Х� ${SO}library${SE} login"
                  #rlogin asleep.ntu.edu.tw -l reader
                  telnet 140.112.196.20 
                  ;;
                4)
                  echo "$time_stamp bbsnet $USER ���j�Ϯ��]" >> bbslog
                  echo "${SO} �s�����j�Ϯ��] ${SE}"
                  echo "�Х� ${SO}OPAC${SE} login"
                  telnet 140.116.207.1
                  #expect /expect/ncku_lib
                  ;;
                5)
                  echo "$time_stamp bbsnet $USER �����Ϯ��]" >> bbslog
                  echo "${SO} �s����ߤ����Ϯ��] ${SE}"
                  echo "\
�p�G�n�h�X�����Ϯ��]���t��, �Цb�u�˯���v���ܲŸ��U��J ${SO}.X${SE} 
�M��b �ucommand�v ���ܲŸ��U��J ${SO} logoff ${SE}"
                  echo -n "�Ы� ENTER �~��"
                  read junk
                  #telnet 192.83.186.1
                  expect -f /expect/clib
                  ;;
                6)
                  echo "$time_stamp bbsnet $USER �x�_���߹Ϯ��]" >> bbslog
                  echo "${SO} �s���x�_���߹Ϯ��] ${SE}"
                  echo "�Х� ${SO}opacnet${SE} login"
                  #telnet 192.83.187.1
                  expect -f /expect/tmlib
                  ;;
                7)
                  echo "$time_stamp bbsnet $USER ����|�Ϯ��]" >> bbslog
                  echo "${SO} �s������|�Ϯ��] ${SE}"
                  rlogin las.sinica.edu.tw -l chinese
                  ;;
		97) echo "          >>>   �s�边�]�w�e��   <<<     " 
                    echo "============================================="
                    echo "     (1) ${SO}ve${SE}   ---- BBS ���ҥΪ��s�边"
		    echo "     (2) ${SO}cvi${SE}  ---- vi"
                    echo "     (3) ${SO}cjoe${SE} ---- joe"
                    echo "============================================="
                    echo -n "�п�� 1 - 3 ���ƥ� (�Э@�ߵ���) ==> " 
                    if read EDIT 
	            then
	                qEDIT=`cat /home/$USER/editor`
                        case ${EDIT} in
                        1) qEDIT="/bin/ve" ;;
                        2) qEDIT="/bin/cvi" ;;
                        3) qEDIT="/bin/cjoe" ;;
                        *) echo  "�u���� 1 - 3 ���ƥ�"  ;;
                        esac

                        echo  "�ثe�ϥΤ��s�边�� ${SO} $qEDIT ${SE}"
                    fi

		    echo $qEDIT > /home/$USER/editor
                    echo -n "�Ы� <enter> �~��" 
                    read junk
                    echo "$time_stamp bbsnet $USER set_editor" >> bbslog
		    ;;
                
                98) echo -n "��� (1..12) ==>"
		    read mon
		    echo -n "��� (1..30) ==>"
		    read day
		    rmore stat/login.$mon.$day 
                    echo "$time_stamp bbsnet $USER �έp��" >> bbslog
                    echo -n "�Ы� ${SO}ENTER${SE} �~�� "
                    read junk  
                  ;;

                99) rmore stat/login 
                    echo "$time_stamp bbsnet $USER �έp��" >> bbslog
                    echo -n "�Ы� ${SO}ENTER${SE} �~��d�߬Q��έp�� "
                    read junk  
		    clear
		    rmore stat/login.yesterday
                    echo -n "�Ы� ${SO}ENTER${SE} �~�� "
                    read junk  
                  ;;
		s|S) 
		    if [ -f home/$USER/.newsrc ] ; then
			cp home/$USER/.newsrc tmp/$USER
			sort tmp/$USER > home/$USER/.newsrc
			rm tmp/$USER
		    fi
		    ;;
                *)
          	  echo ""
		  echo "${SO} �ݤ����z�諸 ${CHOICE}, �A�դ@���a .${SE}"
		  read junk
		  ;;
            esac
          else
            exit
        fi
done

clear


#!/usr/bin/bash
#author: 孙昊航
#2020-11-24-20-04

#print.sh             print the whole screen
#print.sh -s          print the selected area
#print.sh -w          print the selected window
#print.sh --help/-h   print this help
if [ "${1}" == "" ]; then
	import -window root $HOME/Screenshot-$(date +%Y%m%d_%H%M%S).png
#TODO 现在设置了双屏，这个命令有点问题
#     粗略看了看man，是不是改成
#     "import -screen" 会更好呢？
#     再说吧
#     2021-11-21-19-05-00
elif [ "${1}" == "-s" ]; then
	import $HOME/Screenshot-$(date +%Y%m%d_%H%M%S).png
elif [ "${1}" == "-w" ]; then
	import -screen $HOME/Screenshot-$(date +%Y%m%d_%H%M%S).png
elif [ "${1}" == "-h" ] || [ "${1}" == "--help" ]; then
	echo "print.sh             print the whole screen"
        echo "print.sh -s          print the selected area"
        echo "print.sh -w          print the selected window"
        echo "print.sh --help/-h   print this help"
fi

#    我原来写的是将图片存在了 /home/sunhh/Pictures/ 里面
#    但是如果没有这个文件夹的话，就不会起作用
#    所以改成 $HOME/
#    截图之后的图片及时处理就行了，
#    放在Pictures里面干嘛嗷！
#    新增 -w 选项
#    2022-03-27-20-45

iflyttstool
===========
使用讯飞语音技术开发的一个简单的TTS工具。
作者：占进冬
时间：2013-05-19

## 目录结构：
doc目录：讯飞语音云相关文档。
include目录：第三代库头文件。
lib目录：第三方库。
log:日志目录。
res：第三方资源目录:间隔音音频，开头、结尾音频的合成文本。
synth:默认的合成结果保存路径。
text:默认的合成文本路径。

## 使用方式：
-s 音频合成
命令行跟参数-s和要合成的文本路径以及合成结果的路径。
注意：路径是携带文件名的完整路径，合成的音频结果必须保存为.wav文件。
示例：
"C:\Documents and Settings\jdzhan\桌面\手机腾讯网音频转换工具\bin\tecentttstool.exe" -s ../text/早晚报.txt ../synth/synth.wav
-t 音频转换
命令行跟参数-t和要转换的.wav音频文件路径以及转换结果.mp3的文件路径。
注意：路径是携带文件名的完整路径，目前支持从.wav格式转换为.mp3格式。
示例：
"C:\Documents and Settings\jdzhan\桌面\手机腾讯网音频转换工具\bin\tecentttstool.exe" -t ../synth/synth.wav ../synth/synth.mp3
-st 音频合成后同时完成.wav到.mp3格式的转换
命令行跟参数-st，要合成的文本路径，合成的.wav路径以及转换后的.mp3文件路径。如果没有提供.mp3文件路径则和.wav路径相同。
示例：
"C:\Documents and Settings\jdzhan\桌面\手机腾讯网音频转换工具\bin\tecentttstool.exe" -st ../text/早晚报.txt ../synth/synth.wav
或
"C:\Documents and Settings\jdzhan\桌面\手机腾讯网音频转换工具\bin\tecentttstool.exe" -st ../text/早晚报.txt ../synth/synth.wav E:\synth.mp3

## 合成文本格式：
要合成的文本格式如下：
示例：
这是新闻1

这是新闻2

这是新闻3

……
多条新闻之间以空格分割，合成时两条新闻之间补上间隔音，并在开头结尾添加上固定文本合成的音频。

##配置文件：
配置文件中m_configs和szParam等参数属于tts音频合成的配置，具体参见doc目录下相关文档或者直接访问：http://open.voicecloud.cn/。
注意：配置日志路径前需手动创建文件夹。

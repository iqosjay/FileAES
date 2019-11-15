# FileAES-Windows
AES文件加密解密


运行指令：

FileAES.exe mode=Encrypt fin=B:\Test.txt fout=B:\TestEncrypt.txt key=IuOWiOgr6uQyLZXL

说明：

mode=Encrypt	加密
mode=Decrypt	解密 

fin=xxxxxxxxxx	输入文件（必须存在！）
fout=xxxxxxxx	输出文件（可以不存在、存在也会覆盖）

key=xxxxxxxxx	加（解）密所使用的密钥（对称加密、所以加密和解密的Key应当是完全一样的）

由于这个工具主要是为了解决“微信机器人”在“Windows”下的加密编译、所以最后一个参数可以不传
会使用默认的硬编码“ABCDEFGH”进行加（解）密
可以在“main.cpp”中修改

# RunAs
Cpp实现类似cmd Runas 命令，提升用户身份执行程序。
# 用法:
命令行
RunAsExe.exe [username] [domain] [password] ["Program"]
### example:
  RunAsExe.exe administrator . xxxx cmd
  
  注意上面 "." 表示本地域名。

# 原理
调用 CreateProcessWithLogonW函数
https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-createprocesswithlogonw

# RunAs
Cpp实现类似cmd Runas 命令，提升用户身份执行程序。
# 用法:
RunAsExe.exe username domain password "Program"

# 原理
调用 CreateProcessWithLogonW函数
https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-createprocesswithlogonw

# I am the loader ~~without melon~~
## How do I work
`loader.cpp` is a dll injector based on `QueueUserAPC` call and some Nt calls to "lightly" avoid EDR beep beep...  
`evildll.cpp` is a dll file uses cache.dat as a encrypted blob, decrypt and phew on the memory.  
`enc.py` is a pe shellcode encryptor.  
`testdll.cpp` is a test dll ???  
## How to use
```cmd
.\loader.exe <PID/EXE PATH> <FULL DLL PATH> <ANYTHING IF U WANT TO LEVEL UP UP>[OPTIONAL]
```
And we can turn PE file into shellcode by [hasherezade/pe_to_shellcode](https://github.com/hasherezade/pe_to_shellcode) then enc.py!    
Just remember that the architect(the x86/x64 wtf stuff) ~~is~~ are the same for dll and the process pe you want to inject.  
I compiled them by visual studio code `cl.exe` on my cute tiny computer and tada~  
Some notes!  
```cmd
cl.exe /EHsc /GS- /O1 stager.cpp /link /OUT:stager.exe
cl /LD test_dll.cpp /Fe:test.dll
```
How to change arch
```cmd
"C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvarsall.bat" x64
```
Do you know how to put a whale into a fridge?  
Yep, my hint is that chocolate stuffs are requirement!  

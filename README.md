
# Proyek UAS - Well Puskesmas

Simple C++ Healthcare Management using sqlite3 for database and socket boost::asio

## Installation

Download and install Msys2 from [msys2.org](https://www.msys2.org/). Then, open an Msys2 shell and install all build tools and libraries needed: 
```shell
pacman -Syu
pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-boost msys2-w32api-headers
```
To complete the setup, you also need to add the Msys2 installation directory to your system's PATH variable. Here's how you can do it:

```
1. Right-click on the "This PC" or "My Computer" icon on your desktop or in the File Explorer and select "Properties."
2. In the System Properties window, click on the "Advanced system settings" link.
3. In the System Properties dialog box, click on the "Environment Variables" button.
4. In the Environment Variables window, locate the "Path" variable under the "System variables" section and click on the "Edit" button.
5. In the Edit Environment Variable window, click on the "New" button and add the path to your Msys2 installation directory. 
   For example, if Msys2 is installed in "C:\msys64," add "C:\msys64\usr\bin" to the list of paths.
6. Click "OK" in each window to save the changes and close the dialogs.
```
After adding the Msys2 directory to the PATH variable, you should be able to access the Msys2 tools and libraries from any command prompt or terminal window without specifying the full path.

Next, open a Terminal and run the following commands:

```bash
  git clone https://github.com/Visqy/Well-Puskesmas.git
  cd Well-Puskesmas
  g++ server.cpp -o server.exe -lboost_program_options-mt -lws2_32 -lwsock32 -lsqlite3 
  cd client
  g++ client.cpp -o client -lboost_program_options-mt -lws2_32
```
    
## Authors

- [@Visqy](https://github.com/Visqy) a.k.a. Sabiq
- [@Paneva](https://github.com/Paneva) a.k.a. Zidan
- [@ciprut1304](https://github.com/ciprut1304) a.k.a. Catur
- [@woozaxu](https://github.com/woozaxu) a.k.a. Yenita
- [@Nthnla](https://github.com/Nthnla) a.k.a. Nathaniela



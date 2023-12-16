
# Proyek UAS - CLI Point Of Sale

Simple C++ Point Of Sale using sqlite3 for database


## Demo

To run the demo, follow the steps below:

- Download the "main.exe" and "database.db" files from demo folder
- Save the files to a specific location on your computer.
- Double-click on the "main.exe" file to execute it.
- Ensure that the "database.db" file is placed in the appropriate directory as instructed by the application's documentation or developer.
- The demo will now run, utilizing the "database.db" file for its intended functionality.

Default login account
```
user password
admin 123
supplier 123
kasir 123
```

## Installation

Download and install Msys2 from [msys2.org](https://www.msys2.org/). Then, open an Msys2 shell and install all build tools and libraries needed: 
```shell
pacman -Syu
pacman -S mingw-w64-x86_64-toolchain
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
  git clone https://github.com/Visqy/Proyek-UAS-POS.git
  cd Proyek-UAS-POS
  g++ main.cpp -o main.exe -lsqlite3
  main.exe
```
    
## Authors

- [@Visqy](https://github.com/Visqy) a.k.a. Sabiq
- [@juans20](https://github.com/juans20) a.k.a. Najwan
- [@vaioretto22](https://github.com/vaioretto22) a.k.a. Fathan
- [@firlyyuzmitaa](https://github.com/firlyyuzmitaa) a.k.a. Firly
- [@asrieeputri](https://github.com/asrieeputri) a.k.a. Asrie
- [@rsndaaa](https://github.com/rsndaaa) a.k.a. Risanda



# GTK3 Number Converter App (Windows)

## **Introduction**
This guide will help you set up, build, and run a GTK3-based Number Converter application on Windows using MSYS2.

---

## **Prerequisites**
1. **Install MSYS2**  
   - Download: [https://www.msys2.org/](https://www.msys2.org/)
   - Run the installer and use the default path (`C:\msys64`).
   - Keep "Run MSYS2 now" checked after installation.

2. **Update MSYS2**
   ```sh
   pacman -Syu
   ```
   - Restart the terminal when updates complete.
   - Open **MSYS2 MinGW 64-bit** and run:
     ```sh
     pacman -Su
     ```

3. **Install Required Packages**
   ```sh
   pacman -S mingw-w64-x86_64-gtk3 mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-pkg-config
   ```

---

## **Project Setup**
1. **Create Project Structure**
   ```sh
   mkdir -p /c/Projects/NumberConverter/{src,bin,build}
   ```

2. **Save Source Code**
   - Create `C:\Projects\NumberConverter\src\main.c`
   - Paste your source code and save.

3. **Create a Makefile**
   - Create `C:\Projects\NumberConverter\Makefile`
   - Add the following content:
     ```make
     CC = gcc
     CFLAGS = `pkg-config --cflags gtk+-3.0`
     LDFLAGS = `pkg-config --libs gtk+-3.0`
     TARGET = bin/number_converter.exe
     SRC = src/main.c

     all:
      	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

     clean:
      	rm -f $(TARGET)
     ```
   - Save the file.

---

## **Building and Running**
1. **Navigate to the Project Directory**
   ```sh
   cd /c/Projects/NumberConverter
   ```

2. **Build the Application**
   ```sh
   mingw32-make
   ```

3. **Run the Application**
   ```sh
   ./bin/number_converter
   ```

---

## **Preparing for Distribution**
1. **Identify Required DLLs**
   ```sh
   ldd ./bin/number_converter
   ```

2. **Copy Required Files**
   ```sh
   mkdir -p /c/Projects/NumberConverter/dist
   cp ./bin/number_converter.exe ./dist/
   ```
   - Copy required DLLs from `C:\msys64\mingw64\bin` to `dist`.
   - Copy GTK themes/icons from:
     - `C:\msys64\mingw64\share\themes`
     - `C:\msys64\mingw64\share\icons`
     - `C:\msys64\mingw64\lib\gdk-pixbuf-2.0`

3. **Create a Launcher Script**
   - Create `C:\Projects\NumberConverter\dist\NumberConverter.bat`:
     ```bat
     @echo off
     set PATH=%~dp0;%PATH%
     start "" "%~dp0number_converter.exe"
     ```
   - Save the file.

---

## **Troubleshooting**
1. **Missing Libraries**
   - Copy all DLLs listed by `ldd ./bin/number_converter` into your `dist` directory.
   - Update the batch file to set the GTK environment variables:
     ```bat
     @echo off
     set GTK_BASEPATH=%~dp0
     set PATH=%~dp0;%PATH%
     start "" "%~dp0number_converter.exe"
     ```

2. **Compilation Errors**
   - Ensure all required packages are installed.
   - Test `pkg-config`:
     ```sh
     pkg-config --cflags --libs gtk+-3.0
     ```
   - If not found, install it:
     ```sh
     pacman -S mingw-w64-x86_64-pkg-config
     ```

3. **Runtime Errors**
   - Run from the MSYS2 terminal to check error messages.
   - Verify all required theme and icon files are in the correct locations.

---

## **Alternative Compilation (Without Makefile)**
If you prefer direct compilation:
```sh
cd /c/Projects/NumberConverter/src
```
```sh
gcc -o ../bin/number_converter.exe main.c `pkg-config --cflags --libs gtk+-3.0`
```
Or, if backticks don’t work:
```sh
gcc -o ../bin/number_converter.exe main.c $(pkg-config --cflags --libs gtk+-3.0)
```

---

This guide ensures a smooth setup, build, and distribution process for your GTK3 Number Converter app on Windows.


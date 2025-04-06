<div align="center">
	<img src="./resources/icon/icon_mac.png" width="200" height="200">
	<h1>FileWatson</h1>
	<p>
		<b>Find your files quickly and effortlessly</b>
	</p>
	<br>
	<br>
	<br>
</div>

FileWatson is a cross-platform and open-source, blazing fast file search tool designed for quick results.
It can be used to quickly find any file based on extension, keywords or to simply list every file inside a folder.

You can allocate any number of cores, the more you allocate the quicker the search.

You can double click on a row to open the file in your default file manager, copy the path by selecting a row and pressing `CRTL+C` or zoom the ui by pressing `CTRL+SCROLL`

You can write your own language files to use any desired language in the program. 

FileWatson is still in development, new features will be added soon but as of now only basic tools are available, however the program is stable and efficient.

## **Search Bar Usage Guide**

FileWatson allows you to perform precise searches using the search bar on the top, you can also choose if include folders in the search. Here are some examples of how to use it:

- **List All Files**  
  To list all files in a specific folder, use the following query:  
  `C:/path/to/folder/*.*` or `C:/path/to/folder/`<br>
  This will display every file within the folder.

- **List Files by Extension**  
  To search for files with a specific extension, like `.txt`, enter:  
  `C:/path/to/folder/*.txt`  
  This will list only the files with the `.txt` extension in the specified folder.

- **Search by Keywords Using Levenshtein Distance**  
  To find files containing a specific keyword (e.g., "homework"), use the following:  
  `C:/path/to/folder/homework`  

## **Features**

### 🔍 **Blazing Fast File Search**
- **Quickly Locate Files**: Search for files using extensions, keywords.
- **Optimized Speed**: Designed for minimal delay, ensuring results appear almost instantly.

### 🖥️ **Cross-Platform Compatibility**
- **Runs Anywhere**: Supports major operating systems, allowing seamless functionality on Windows, macOS, and Linux.

### 🏎️ **Core Allocation**
- **Multi-Core Utilization**: Assign as many cores as you like for faster searches. 

### 📋 **Table Sorting**
- **Clickable Columns**: Click on any column header in the results table to sort the files alphabetically by that column.
- **Toggle Sorting**: Clicking the same header again reverses the sorting order.

### 🖱️ **Interactive File Actions**
- **Open Files Directly**: Double-click a result to open the file in your default file manager.
- **Copy File Paths**: Select a file row and press `CTRL+C` to copy its path instantly.
- **Zoom UI**: Adjust the UI scale by pressing `CTRL+SCROLL` for a better view.

### 🌍 **Custom Language Support**
- **Localization Ready**: Create and use custom language files to translate the program into your preferred language.
- **Easy Implementation**: Simply write a new JSON language file, and FileWatson will adapt.

# Build Instructions
⚠️ Tested on:
 - Windows 11 x64
 - Linux Mint 22 Cinnamon (Kernel 6.8.0-38-generic)
 - Debian WSL (Kernel 5.15.167.4-1)

Follow the steps below to build the FileWatson project on your platform.

## Prerequisites
- **CMake**
- **vcpkg** (Optional)
- **wxWidgets** (Install and ensure `wx-config` is available on non-Windows platforms)
- **nlohmann_json** (Ensure it's installed or available via your package manager)
- **DBus** (Required only on Linux)

## Windows
1. Clone the project
2. Open it in Visual Studio
3. Install wxWidgets and nlohmann_json, both can be downloaded using [vcpkg](https://vcpkg.io/)

## macOS
1. Install [Homebrew](https://brew.sh/) if not already installed.
2. Install dependencies:
    ```bash
    brew install cmake wxwidgets nlohmann-json
    ```
3. Export homebrew include path:
      - For Apple Silicon Macs:
        ```bash
        export C_INCLUDE_PATH=/opt/homebrew/include:$C_INCLUDE_PATH
        export CPLUS_INCLUDE_PATH=/opt/homebrew/include:$CPLUS_INCLUDE_PATH
        ```
      - For Intel Macs:
        ```bash
        export C_INCLUDE_PATH=/usr/local/include:$C_INCLUDE_PATH
        export CPLUS_INCLUDE_PATH=/usr/local/include:$CPLUS_INCLUDE_PATH
        ```
4. Build the project:
    ```bash
    mkdir build && cd build
    cmake .. -G "Unix Makefiles"
    make
    ```

## Linux

1. Install required packages with your package manager:
   - On Ubuntu/Debian:
     ```bash
     sudo apt update
     sudo apt install cmake g++ libwxgtk3.2-dev nlohmann-json3-dev libdbus-1-dev
     ```
   - On Arch:
     ```bash
     sudo pacman -Syu
     sudo pacman -S cmake g++ wxwidgets-gtk3 nlohmann-json dbus
     ```

2. Build the project:
   ```bash
   mkdir build && cd build
   cmake .. -G "Unix Makefiles"
   make
   ```

## Install Instructions
The CMake files includes commands to install FileWatson automatically on Linux and Windows.

### Linux
1. `sudo make install` (This will also create a symlink called `fw`)

### Windows
In the Visual Studio GUI go to CMake -> Build -> Install. This will execute the `install()` command.

## Language Files

FileWatson supports custom language files, allowing users to translate the program into their desired language. Here’s a simple guide to creating and modifying these files.

### **Structure of a Language File**
Language files are JSON files that define the text displayed in the program. These files are located in the `lang/` folder on Windows and Linux and in `Resources/lang/` on macOS in the app bundle.

Each entry consists of a key and a value:
- **Key**: Represents the internal identifier used by the program (should not be changed).
- **Value**: The actual text displayed in the program, which you can translate or modify.

### **Example**
```json
{
  "lang_def": "Language Name",

  "app_name": "FileWatson %s <-(VERSION_NUMBER)",

  "browse": "Browse",
  "search": "Search",
  "directory": "Directory",
  "file_folder_name": "File/Folder Name",
  "size": "Size",
  "last_mod": "Last Modified",

  "error": "Error",
  "alg_warn": "Algorithm Warning",
  "info": "Info",

  "exit": "Exit",

  "clipboard_open_fail": "Failed to open the clipboard.",
  "search_already_in_prog": "A search is already in progress.",
  "path_search_missing": "Please specify a path to search in.",
  "invalid_core_count": "Please specify a valid number of cores to use.",
  "core_less_or_equal": "Please specify a number of cores less or equal than %d <-(MAX_CORE_COUNT).",
  "file_picker_window_name": "Select a path to recursively search in",
  "show_file_fm_error": "Failed to show file in the default file manager.\n\nPath: %s <-(PATH_OF_SELECTED_FILE)",
  "search_done": "Search completed.",
  "more_than_x_in_list": "More than %d <-(MAX_ENTRIES) entries present in the table (%zu <-(PRESENT_ENTRIES), sorting might take a while. Continue?"
}
```
If everything has been done correctly, the language should appear in the combo box on the bottom left.


ℹ️ Most of the default languages (except English and Italian) are AI generated, if you find any grammatical errors or some improvements you can add, pull requests are open.

⚠️ If any key is missing from the language file, the UI will display the default message **"String not found"**.

## TODO
Contributing is really appreciated. To suggest new features or improvements, please open an issue.
- [ ] Windows installer
- [ ] Stop button
- [ ] Control of fuzzy factor
- [ ] Show progress bar for heavy searches with a cancel button

## Dependencies
FileWatson only depends on two libraries:
- [wxWidgets](https://github.com/ImageOptim/gifski) - GUI, Config, and cross-platform Unicode support.
- [nlohmann-json](https://github.com/sindresorhus/Defaults) - Quick and easy JSON library, used in the language loader.

## License
FileWatson is licensed under [MIT](https://opensource.org/license/mit).  
# imdbparser

Convert the IMDB alternate interfaces text files to an SQLite database or csv file

Data Source: http://www.imdb.com/interfaces

## How to build

This uses CMake and requires SQLite[optional]

If you are on linux you'll need to install sqlite3 headers and library

Example:

    sudo apt-get install libsqlite3-dev

For ther platforms you'll need to either download it from here or build it manually froum source (https://www.sqlite.org/download.html)

To use SQLite the cli make command would look like

    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE:STRING=Release ..
    make

OR use cmake GUI or cmake to provide options interactively


## Usage:

The source files are encoded in ISO-8859-1 but sqlite supports utf8 and can be converted to utf with any script similar to

    for i in *.list; do iconv -f ISO-8859-1 -t UTF8 $i >"u8${i}"; done

### Using the command
    
    ./imdbparser.exe ./imdbout.sqlite ./u8ratings.list [./u8language.list] [./u8certificates.list] [./u8genres.list] [./u8running-times.list]

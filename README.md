# imdbparser

Convert the IMDB alternate interfaces text files to an SQLite database or csv file

Data Source: http://www.imdb.com/interfaces


## usage:

The source files are encoded in ISO-8859-1 but sqlite supports utf8 and can be converted to utf with any script similar to

    for i in *.list; do iconv -f ISO-8859-1 -t UTF8 $i >"u8${i}"; done

### Using the command
    
    ./imdbparser.exe ./imdbout.sqlite ./u8ratings.list [./u8language.list] [./certificates.list] [./genres.list] [./running-times.list]
    
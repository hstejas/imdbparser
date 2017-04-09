#!/bin/bash 

set -euo pipefail

mkdir -p data
cd data

FILES="ratings language certificates genres running-times"

for FILE in $FILES; do
	wget "ftp://ftp.fu-berlin.de/pub/misc/movies/database/$FILE.list.gz"
done

gunzip ./*.gz

for i in *.list; 
do 
 iconv -f ISO-8859-1 -t UTF8 "$i" >"u8${i}"; 
done

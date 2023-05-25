#!/bin/bash
filePath="demo.txt"
echo "File Name : " $filePath > out.txt
#here I used ">" cause it overwrites on the old data of the outpute file

#No of lines
awk  'BEGIN{count=0} 
    //{count++} 
    END{print "Number of lines in the text file : ",count}' >> out.txt $filePath
#here and later on I used ">>" cause it appends to the output file.

#No of words 
awk  'BEGIN{count=0} 
    //{count++} 
    END{print "Number of words in the text file : ",count}' RS="[[:space:]]" >> out.txt $filePath
#RS means we increase count after every space.

#file size
fileSize=$(stat -c%s "$filePath")
echo "Number of bytes : " `wc -c < $file_path` >> out.txt
echo "Script executed Successfully !!! Check out.txt for output."
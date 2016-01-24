import os 
import sys

out = open("sampleDB.dat" , "w" )  
f = open(sys.argv[1])
for ch in tuple(f) : 
    out.write("<s> " + ch.replace(' ' , '@ ').replace('\n' , '')  + " <\s>\n")
    out.write("<s> " + ch.replace(' ' , '@ ').replace('\n' , '')  + "@ <\s>\n")

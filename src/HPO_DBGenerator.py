#!/usr/bin/env python

import os 

out = open("sampleDB.dat" , "w" )  
for _file in os.listdir("data/text") :
    f = open("data/text/" + _file)
    ch = f.read().strip()
    out.write("<s> " + ch  + " </s>\n")
    out.write("<s> " + ch  + "@ </s>\n")
    if len(ch.split(' ')) > 1 : 
        for c in ch.split(' ') :
            out.write("<s> " + c.strip() + " </s>\n")
            out.write("<s> " + c.strip() + "@ </s>\n")
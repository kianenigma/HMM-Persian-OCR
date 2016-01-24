#!/usr/bin/env python

###########################################################################
# Modification and Distribution by Kian Peymani, Artificial Intelligence Group
# Iran University of Science and Thechnology
#
# Permission to use, copy, modify, and distribute this software and its
# documentation for any purpose and without fee is hereby granted,
# with appearance of this license on top of each file.
# This software is provided "as is" without express or implied warranty.
# ##########################################################################


import os

out = open("sampleDB.dat" , "w" )
for _file in os.listdir("data/text") :
    f = open("data/text/" + _file)
    ch = f.read().strip()
    out.write("<s> " + ch  + " <\s>\n")
    #out.write("<s> " + ch  + "@ </s>\n")
    if len(ch.split(' ')) > 1 :
        for c in ch.split(' ') :
            out.write("<s> " + c.strip() + " <\s>\n")
            out.write("<s> " + c.strip() + "@ <\s>\n")

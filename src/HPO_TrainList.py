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
import sys
import random


for fea in os.listdir('feaDir') :
    if os.stat('feaDir/' + fea).st_size == 12 :
	#Must be checked
        os.remove('feaDir/' + fea)
        print "delete file ( 12b empty feat err )" , fea
print sys.argv
f = open("train.lst" , "w" )
text = os.listdir('feaDir')
count = len(os.listdir('feaDir'))

# train all files
for t in text :
    f.write("feaDir/" + t.split('.')[0] + ".fea\n")

for i in range(10) :
    ff = open('test_sh_' + str(i) + '.lst' , 'w')
    for j in range(100) :
	ff.write("feaDir/" + random.choice(text).split('.')[0] + ".fea\n" )
    ff.close()

f.close()

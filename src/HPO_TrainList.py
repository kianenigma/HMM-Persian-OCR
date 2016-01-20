#!/usr/bin/env python

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
    
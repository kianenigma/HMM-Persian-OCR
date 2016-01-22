#!/usr/bin/env python

from scipy import ndimage
from scipy import misc
import numpy as np 
import sys
import os

print sys.argv
if len(sys.argv) != 4 : 
    print "Usage : featExtractor <ImageDir> <outDir> <Dim>"
    exit()
imgs = os.listdir(sys.argv[1]) 
os.system("mkdir " + sys.argv[2])

for img_dir in imgs : 
    img = ndimage.imread(sys.argv[1]+"/"+img_dir)  
    print sys.argv[1]+"/"+img_dir , img 
    #Fix image size 
    img = misc.imresize(img , float(sys.argv[3])/len(img) )
    
    sobel_x = ndimage.filters.sobel(img , -1) 
    sobel_y = ndimage.filters.sobel(img , 0 )
    laplace = ndimage.filters.laplace(img)
    
    
    f = open( sys.argv[2] + "/" + img_dir.split('.')[0] + ".rawFeat" , 'w') 
    acc = True 
    for frame_idx in range(len(img[0])) : 
	print frame_idx
	for v in range(len(sobel_x)) : 
            f.write(str(sobel_x[v][frame_idx]) + ' ') 
        for v in range(len(sobel_y)) : 
            f.write(str(sobel_y[v][frame_idx]) + ' ') 
        for v in range(len(laplace)) : 
            f.write(str(laplace[v][frame_idx]) + ' ')
	#Fix 
	for v in range( (3*int(sys.argv[3])) - ( len(sobel_x) + len(sobel_y) + len(laplace) ) ) : 
	    f.write('0 ') 
        f.write('\n')
    f.close()

    if acc : 
        os.system("pfl2htk " +
                  sys.argv[2] + "/" + img_dir.split('.')[0] + ".rawFeat " +
                  sys.argv[2] + "/" + img_dir.split('.')[0] + ".fea")
    os.system("rm " + sys.argv[2] + "/" + img_dir.split('.')[0] + ".rawFeat")

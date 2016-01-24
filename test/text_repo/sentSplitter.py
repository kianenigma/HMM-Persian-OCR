import os 
import sys
reload(sys)
sys.setdefaultencoding('utf-8')

sents = tuple(open(sys.argv[1] , "r"))
out = open(sys.argv[1].split('.')[0] + "_2split_.txt" , "w")

o = []  

for s in sents :
    print s 
    pred = s.strip().split(" ")
    pred[-1] = pred[-1].replace('\n' , '') 
    for idx , w in enumerate(pred) :
        w = w.strip()
        if len(w) >= 10 :
            print list(w) , w  , len(w)
            o.append(w)

	#out.write(w +  ("\n" if idx != len(pred)-1 else "" ) ) 
    
#    print len(pred)
#    o.append(' '.join(pred[0:5]))
for l in o :
    if l != " " and l != "\n"  :
        fix = l.replace('\n' , '').replace("   " , " ").strip() 
        out.write(fix + "\n" )


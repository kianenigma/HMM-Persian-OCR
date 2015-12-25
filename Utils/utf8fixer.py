import os 
import sys 
import re 
import array

f = open(sys.argv[1] ,"r") 
n = open(sys.argv[1]+"_fix" , "w") 
lines = tuple(f) 

for line in lines : 
	print(line)
	n_line = ""
	for idx , ch in enumerate(line) :
		if ch == "=" : 
			if line[idx+1] == "\\" : 
				print("GOTCHAAA")
				s = line[idx+1:len(line)]
				s = s.split('\\')
				del s[0] 
				s[-1] = s[-1].replace('\n' , '') 
				print(s)
				s = [ int(h , base=8) for h in s ]
				s = bytes(s).decode('utf-8')
				n_line = line[0:idx+1] + s 
				print(n_line)
	
	if n_line : 
		n.write(n_line+"\n") 
	else :
		n.write(line)
n.close()			

import os 

out = open("sampleDB.dat" , "w" )  
for file in os.listdir("data/text") :
    f = open("data/text/" + file)
    ch = f.read() 
    out.write("<s> " + ch.replace(' ' , '@ ')  + " </s>\n")
    out.write("<s> " + ch.replace(' ' , '@ ')  + "@ </s>\n")

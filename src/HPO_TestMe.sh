if [ $# -ne 2 ]; then
  echo "Usage: testme.sh <text> "
  exit 1
fi
pango-view --text "$2" --no-display --font="B Mitra" --dpi=700 --align left --output temp.png
pngtopnm temp.png > temp.pnm
rm temp.png
ppmtopgm temp.pnm > temp.pgm 
rm temp.pnm 
pgmtopbm temp.pgm > temp.pbm 
rm temp.pgm 

f=temp.pbm
dim=20
pnmdepth 255 temp.pbm |
pgmmedian |          
pgmslope|            
pgmslant -M M |       
pgmnormsize -c 5 |    
pgmtextfea -c 20  > _tmp.fea     
pfl2htk _tmp.fea temp.fea

rm temp.pbm _tmp.fea

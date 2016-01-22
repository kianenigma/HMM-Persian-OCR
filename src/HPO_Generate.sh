if [ $# -ne 2 ]; then
  echo "Usage: generator.sh <TEST-NAME> <text-file>"
  exit 1
fi

i="1"

mkdir $1 

ROOT=$1/data

mkdir $ROOT

rm -Rf $ROOT/*

mkdir $ROOT/img
mkdir $ROOT/text

while read p; do
	pango-view --text "$p" --no-display --font="B Mitra" --dpi=1200 --align left --output $ROOT/img/train_$i.png
	echo $p >> $ROOT/text/train_$i.txt
	
	pngtopnm $ROOT/img/train_$i.png > $ROOT/img/train_$i.pnm
	rm $ROOT/img/train_$i.png
	
	ppmtopgm $ROOT/img/train_$i.pnm > $ROOT/img/train_$i.pgm 
	rm $ROOT/img/train_$i.pnm 

	pgmtopbm $ROOT/img/train_$i.pgm > $ROOT/img/train_$i.pbm 
	rm $ROOT/img/train_$i.pgm 

	i=$(($i+1))
	echo "generating image #$i with data $p"
done < $2


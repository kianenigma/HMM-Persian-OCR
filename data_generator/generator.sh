if [ $# -ne 1 ]; then
  echo "Usage: generator.sh <text-file>"
  exit 1
fi

i="1"

mkdir outdir
mkdir outdir/png
mkdir outdir/text

rm outdir/png/*
rm outdir/text/*

while read p; do
	pango-view --text "$p" --no-display --font="B Mitra" --dpi=700 --align left --output outDir/png/train_$i.png
	echo $p >> outdir/text/train_$i.txt 
	i=$[$i+1]
	echo "generating image #$i with data $p"
done < $1


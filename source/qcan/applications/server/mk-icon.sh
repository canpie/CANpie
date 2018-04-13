#!/usr/bin/env bash 
sizes=(16 32 64 128 256 512)
largfile='icon_512x512@2x.png'
if [ ! -f "$largfile" ]; then
  convert -background none -resize 1024x1024 "$1" "$largfile"
fi
for s in "${sizes[@]}"; do
  echo $s
  convert -background none -resize ${s}x${s} "$largfile" "icon_${s}x$s.png"
done

cp 'icon_32x32.png'     'icon_16x16@2x.png'
mv 'icon_64x64.png'     'icon_32x32@2x.png'
cp 'icon_256x256.png'   'icon_128x128@2x.png'
cp 'icon_512x512.png'   'icon_256x256@2x.png'

mkdir icon.iconset
mv icon_*x*.png icon.iconset
iconutil -c icns icon.iconset

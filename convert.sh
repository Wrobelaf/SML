
for i in `find $1 -name '*.*'`
do
   echo $i
   dos2unix $i
done

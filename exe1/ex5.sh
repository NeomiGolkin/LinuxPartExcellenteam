mkdir -p ~/UUni/subs
cd ~/UUni/subs

touch FileA.txt FileB.txt FileC.txt FileD.txt FileE.txt FileF.txt

rm FileC.txt FileD.txt FileE.txt FileF.txt

mv FileA.txt old.FileA

rm *

cp /etc/*.conf . 2>/dev/null

cat l* 2>/dev/null

ls -d ????

ls -t | head -n 1

echo "The last modified file is $(ls -t | head -n 1)"

cut -d: -f1 /etc/group | xargs mkdir

echo '}{-: ;-) *-:'
who -a > logged_is_who

echo "42 is answer The " > fact

cat logged_is_who >> fact

grep "Alice" alice.txt

grep -c "Why" alice.txt

grep "^CHAPTER" alice.txt | sed 's/^CHAPTER [0-9IVX]*[.]* *//' > chapters.txt

grep "fear" alice.txt | tr 'e' 'o'

grep -n "Alice" alice.txt > alice_numbered.txt

grep -v -E "fear|rabbit" alice.txt

grep '\*' alice.txt | sort -u
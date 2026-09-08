HOME_DIR="/c/Users/USER001"

ls -laS

largest_file=$(ls -pS /etc | grep -v / | head -n 1)
cat "/etc/$largest_file"

ls -laSr

smallest_file=$(ls -pSr /etc | grep -v / | head -n 1)
cat "/etc/$smallest_file"

mkdir -p "$HOME_DIR/grandpa/father/son"

command -v bc >/dev/null 2>&1 && echo "5 + 3" | bc || awk 'BEGIN { print 5 + 3 }'

command -v bc >/dev/null 2>&1 && echo "5 + 3" | bc -q || awk 'BEGIN { print 5 + 3 }'

command -v cal >/dev/null 2>&1 && cal || python -m calendar

command -v cal >/dev/null 2>&1 && cal 2026 || python -m calendar 2026
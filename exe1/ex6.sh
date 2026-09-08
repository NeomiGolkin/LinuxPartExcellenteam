NUM=${1:-50}
DIR_NAME="files_$(date +%Y%m%d_%H%M%S)_${NUM}"
mkdir -p "$DIR_NAME"

for ((i=1; i<=NUM; i++)); do
    head -c $i /dev/zero > "$DIR_NAME/dat_${i}_File"
done
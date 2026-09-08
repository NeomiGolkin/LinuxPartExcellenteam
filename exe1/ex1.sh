HOME_DIR="/c/Users/USER001"

echo "NeomiGolkin" > "$HOME_DIR/neomi1.txt"
cat << 'EOF' > "$HOME_DIR/neomi2.txt"
NeomiGolkin
EOF

mkdir -p "$HOME_DIR/new"

cp "$HOME_DIR/neomi1.txt" "$HOME_DIR/new/"

mv "$HOME_DIR/neomi1.txt" "$HOME_DIR/new/NeomiGolkin.txt"

echo -e "NeomiGolkin\nFavorite Animal: Dog" > "$HOME_DIR/new/NeomiGolkin.txt"

cp "$HOME_DIR/new/NeomiGolkin.txt" "$HOME_DIR/new/newFile"

cd "$HOME_DIR/new"
ls -la

cd ~
cd "$HOME_DIR"

rm -rf "$HOME_DIR/new"

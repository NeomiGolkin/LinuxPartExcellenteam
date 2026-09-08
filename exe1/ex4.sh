commnd -v pv >/dev/null 2>&1 && pv -s 100M /dev/zero > testfile && rm testfile || dd if=/dev/zero of=testfile bs=1M count=100 && rm testfile

command -v pv >/dev/null 2>&1 && pv -s 500M /dev/zero > /dev/null || dd if=/dev/zero of=/dev/null bs=1M count=500

command -v pv >/dev/null 2>&1 && pv -s 50M /dev/random > /dev/null || dd if=/dev/random of=/dev/null bs=1M count=50

command -v pv >/dev/null 2>&1 && pv -s 100M /dev/urandom > /dev/null || dd if=/dev/urandom of=/dev/null bs=1M count=100

dd if=/dev/zero of=bigfile bs=1M count=1024

command -v pv >/dev/null 2>&1 && pv bigfile > /tmp/bigfile || cp bigfile /tmp/bigfile
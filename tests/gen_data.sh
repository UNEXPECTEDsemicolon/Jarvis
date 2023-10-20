#!/bin/bash

python3 -c "from random import randint; print(*[randint(0, 1000) for i in range(50000)], sep='', end='')" > resources/A/a_big.txt
cat resources/A/a_big.txt  resources/A/a_big.txt resources/A/a_big.txt > resources/B/b_big.txt

python3 -c "from random import randint; print(*[randint(0, 1000) for i in range(500)], sep='', end='')" > resources/C/c1.txt
cat resources/C/c1.txt resources/C/c1.txt resources/C/c1.txt > resources/D/d1.txt
cat resources/D/d1.txt resources/D/d1.txt > resources/C/c3.txt
cat resources/C/c3.txt > resources/D/d2.txt

echo "qwertyuiop" > resources/E/e1.txt
echo "qetuo" > resources/F/f1.txt
echo "qWeRtYuIoP" > resources/F/f2.txt

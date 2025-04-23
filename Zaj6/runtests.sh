echo "Tests starting"

for i in {1..15}; do
    echo "Test $i"
    printf "\r\n" > eol
    cat "tests/t$i" eol > test
    cat "tests/r$i" eol > wzor
    socat -t 10.0 stdio tcp4:127.0.0.1:2020 < test > wynik
    diff -ys wzor wynik
done

echo "Tests finished"
rm eol
rm test
rm wzor
rm wynik
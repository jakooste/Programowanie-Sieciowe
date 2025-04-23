echo "Tests starting"

for i in {1..15}; do
    echo "Test $i"
    ./_mini_udpcat.py 127.0.0.1 2020 < "t$i" > tmp
    echo "" >> tmp
    diff -y "r$i" tmp
done

echo "Tests finished"
rm tmp

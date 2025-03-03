rm -rf submit
mkdir -p submit/Code
cp Code/*.c submit/Code
cp Code/*.h submit/Code
cp Code/*.l submit/Code
cp Code/*.y submit/Code
cp Code/Makefile submit/Code
cp doc/report.pdf submit
cp README submit
cd Code && make
cp parser ../submit
make clean
cd ../submit
zip -r ../submit.zip *
cd ..
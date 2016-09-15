g++ -o q2 q2.cpp
./q2 OCRDataset/data/small/images.dat out.txt
python wordacc.py OCRDataset/data/small/words.dat out.txt
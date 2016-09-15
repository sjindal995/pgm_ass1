import sys

def equalcount(filename1 , filename2):
	f1 = open(filename1);
	f2 = open(filename2)
	s = ""
	count = 0
	count1 = 0
	for line in f2:
		s2 = f1.readline()
		if(line == s2):
			count+=1
			# print line[:-1]
		count1 += 1
	print "Word Accuracy" , count/float(count1) *100
	f1.close()
	f2.close()
	charcount = 0
	f1 = open("OCRDataset/data/small/words.dat");
	f2 = open("extra/out_extra_dec.txt")
	s1 = f1.read()
	s2 = f2.read()
	a = [i for i in xrange(len(s2)) if (s1[i] == s2[i] and s1[i] != '\n')]
	b = [i for i in xrange(len(s2)) if (s1[i] != '\n')]
	print "Char Accuracy" , float(len(a))/float(len(b)) *100
	# print a 

# Original Word File
# Output File
fil1 = sys.argv[1]
fil2 = sys.argv[2]
equalcount(fil1,fil2)

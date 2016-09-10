import sys
import random
adj_list = []

def randomBN():
	n = int(sys.argv[1])
	k = int(sys.argv[2])
	comp_list = []
	for i in range(1,n+1):
		comp_list.append(i)
	for i in range(1,n+1):
		n_children = random.randint(0,k)
		if(n_children > n-1):
			child_indices = comp_list
			child_indices.remove(i)
		else:
			child_indices = random.sample(range(1,i) + range(i+1, n+1),n_children)
		adj_list.append(child_indices)
		print str(i) + " : " +  str(n_children) +  " : " + str(child_indices)

	with open('random-bn.txt','w') as f:
		for index in range(1,n+1):
			f.write(str(index) + " " + str(adj_list[index-1]) + "\n")

randomBN()

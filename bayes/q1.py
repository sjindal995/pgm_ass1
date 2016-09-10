import sys
from random import randint

def randomBN():
	n = int(sys.argv[1])
	k = int(sys.argv[2])
	left = []
	adj_list = [];
	for i in range(0,n):
		left.append(i);
		adj_list.append([])
	queue = []
	while (len(left) > 0):
		if(len(queue) > 0):
			cur_element = queue[0]
			queue = queue[1:]
			u = randint(0,k)
			for pick_num in range(0,u-1):
				if(len(left) > 0):
					child_num = randint(0,len(left)-1)
					child = left[child_num]
					adj_list[cur_element].append(child+1)
					left.remove(child)
					queue.append(child)
				else:
					break
		else:
			queue.append(left[0])
			left.remove(left[0])

	with open('random-bn.txt','w') as f:
		for index in range(1,n+1):
			f.write(str(index) + " " + str(adj_list[index-1]) + "\n")


def randomBN():
	n = int(sys.argv[1])
	k = int(sys.argv[2])
	adj_list = [];
	for i in range(0,n):
		adj_list.append([])
		n_children = randint(0,k)
		child_indices = random.sample(range(1,n),n_children)
		with open('random-bn.txt','w') as f:
			for index in range(1,n+1):
				f.write(str(index) + " " + str(adj_list[index-1]) + "\n")

randomBN()

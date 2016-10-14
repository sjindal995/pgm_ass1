import sys
import random
from ast import literal_eval
class Node:
	def __init__(self, node_id, children, parents):
		self.node_id = node_id
		self.children = children
		self.parents = parents


dummy_node = Node(0, [], [])
bn = [dummy_node]
def randomBN():
	n = int(sys.argv[1])
	k = int(sys.argv[2])
	comp_list = []

	for i in range(1,n+1):
		new_node = Node(i, [], [])
		bn.append(new_node)
		comp_list.append(i)

	comp_list.remove(1)

	for i in range(1,n+1):
		n_children = random.randint(0,k)
		if(n_children > n-1):
			child_indices = comp_list
			child_indices.remove(i)
		else:
			child_indices = random.sample(range(2,i) + range(i+1, n+1),n_children)
		bn[i].children = child_indices
		for child in child_indices:
			bn[child].parents.append(i)
		


def exportBN(file_name):
	with open(file_name,'w') as f:
		f.write(str(len(bn)-1) + "\n")
		for index in range(1,len(bn)):
			str_children = str(bn[index].children).replace(" ", "")
			f.write(str(index) + " " + str_children + "\n")
	
def importBN(file_name):
	with open(file_name,'r') as f:
		n = int(f.readline()[:-1])
		for i in range(1,n+1):
			new_node = Node(i, [], [])
			bn.append(new_node)
		for line in f:
			line = line[:-1]
			line_list = line.split(" ",1)
			node_id = int(line_list[0])
			children = literal_eval(line_list[1])
			bn[node_id].children = children
			for child in children:
				bn[child].parents.append(node_id)


def getActiveTrail(x, y, z, output_query_file_name):
	observed = z
	observed_ancestors = []
	while(len(observed) > 0):
		cur = observed[0]
		observed = observed[1:]
		if cur not in observed_ancestors:
			observed += bn[cur].parents
			observed_ancestors.append(cur)

	path_prev = []
	for i in range(0,len(bn)+1):
		path_prev.append(0)
	to_visit = [[x,"out"]]
	visited = []
	found_path = False
	while(len(to_visit) > 0):
		cur = to_visit[0]
		to_visit = to_visit[1:]
		if cur not in visited:
			cur_observed = False
			if cur[0] in z:
				cur_observed = True
			if((not cur_observed) and (cur[0] == y)):
				found_path = True
				break
			visited.append(cur)
			if((cur[1] == "out") and (not cur_observed)):
				for par in bn[cur[0]].parents:
					if([par,"out"] not in visited):
						if path_prev[par] == 0:
							path_prev[par] = cur[0]
						to_visit.append([par,"out"])
				for child in bn[cur[0]].children:
					if([child,"in"] not in visited):
						if path_prev[child] == 0:
							path_prev[child] = cur[0]
						to_visit.append([child,"in"])
			elif(cur[1] == "in"):
				if ((not cur_observed)):
					for child in bn[cur[0]].children:
						if([child,"in"] not in visited):
							if path_prev[child] == 0:
								path_prev[child] = cur[0]
							to_visit.append([child,"in"])
				if (cur[0] in observed_ancestors):
					for par in bn[cur[0]].parents:
						if([par,"out"] not in visited):
							if path_prev[par] == 0:
								path_prev[par] = cur[0]
							to_visit.append([par,"out"])
	if(found_path):
		path = [y]
		cur = y
		while(cur != x):
			cur = path_prev[cur]
			path.append(cur)
		path.reverse()
		str_path = str(path).replace(" ", "")
		print "no " + str_path
		with open(output_query_file_name,"a") as f:
			f.write("no " + str_path + "\n")
	else:
		print "yes"
		with open(output_query_file_name,"a") as f:
			f.write("yes\n")


def runQueryFile(filename, query_ouput_file):
	with open(query_ouput_file,"w") as f2:
				f2.write("")
	with open(filename,'r') as f:
		n_queries = int(f.readline()[:-1]);
		for line in f:
			line = line[:-1]
			if(line[-1] <> ']'):
				line += ']'
			with open(query_ouput_file,"a") as f2:
				f2.write(line + "\n")
			line_list = line.split(" ", 2)
			getActiveTrail(int(line_list[0]), int(line_list[1]), literal_eval(line_list[2]), query_ouput_file) 


# randomBN()
# exportBN("random-bn.txt")
importBN("sample-bn.txt")
runQueryFile("sample-query.txt","sample-query-output1.txt")

# randomBN()
# exportBN("bn.txt")

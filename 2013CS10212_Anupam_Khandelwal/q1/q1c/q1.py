import random
import numpy as np
import copy
import re

# n -> size of bayes net
# maxChild -> array of max no of child nodes

def genBayesNet(n , maxChild , outFilename):
	allChildren = []
	numChild = []

	#Generate number of children for every node
	for ind in xrange(1,n+1):
		i = ind - 1
		random.seed()
		numChild.append(random.randint(0,maxChild[i]))
	numChild.sort(reverse=True)
	# Topological ordering -> node with largest numChild first and so on

	for ind in xrange(1,n+1):
		i = ind - 1
		random.seed()
		children = random.sample(range(ind + 1 , n + 1), numChild[i])
		print children
		allChildren.append(children)
	

	f = open(outFilename , "wb")
	f.write(str(n) + "\n")
	for ind in xrange(1,n+1):
		i = ind - 1
		childstr = "["
		if(len(allChildren[i])==0):
			childstr = "[]"
		else:
			for j in xrange(0,len(allChildren[i])):
				childstr += str(allChildren[i][j])+","
			childstr = childstr[:-1] + "]"

		f.write(str(ind)+" "+childstr+"\n")

	f.close()
	return allChildren





# Gc -> Bayes Net with children stored
# Xi -> source
# Xj -> destination
# Z -> observed variables

def findActiveTrail(Gc , Xi , Xj , Z):
	# Gp -> Bayes net with parents stored (local variable)
	Gp = []
	for i in xrange(0,len(Gc)):
		Gp.append([])

	for i in xrange(0,len(Gc)):
		for child in Gc[i]:
			Gp[child].append(i)
	
	# Initiated to store path parent 
	pathParent = []
	for i in xrange(0,len(Gp)):
		pathParent.append(-1)

	# Include all variables that can form V - structure
	L = copy.copy(Z)
	A = []
	while(len(L)>0):
		Y = L[0]
		# Remove 1st element from L
		L = L[1:]
		if(Y not in A):
			for items in Gp[Y]:
				L.append(items)
		A.append(Y)


	# Start BFS from source to destination
	L = [(Xi,1)]
	R = []		# Reachable 
	V = []		# Visited (Node , direction)

	while(len(L)>0):
		(Y,d) = L[0]
		L = L[1:]
		if((Y,d) not in V):
			# Found in set of active trails
			if(Y not in Z):
				R.append(Y)
				if(Y == Xj):
					# Build path from pathParent array
					src = Xj
					path = []
					while(pathParent[src] != Xi):
						path.append(src+1)
						src = pathParent[src]
					path.append(src+1)
					path.append(Xi+1)
					
					return [0,list(reversed(path))] 

			V.append((Y,d))

			if(d == 1 and Y not in Z):
				# Case :  -> Y -> is active 
				for items in Gp[Y]:
					if((items,1) not in V and (items,0) not in V):
						pathParent[items] = Y
					L.append((items,1))

				# Case :  <- Y -> is active 
				for items in Gc[Y]:
					if((items,1) not in V and (items,0) not in V):
						pathParent[items] = Y
					L.append((items,0))


			elif(d == 0):
				if(Y not in Z):
					# Case :  <- Y <- is active 
					for items in Gc[Y]:
						if((items,1) not in V and (items,0) not in V):
							pathParent[items] = Y
						L.append((items,0))

				if(Y in A):
					# Case :  -> Y <- is active if Y belongs to A , forming a V-structure 
					for items in Gp[Y]:
						if((items,1) not in V and (items,0) not in V):
							pathParent[items] = Y
						L.append((items,1))


	return [1,[]]



def sub1(x):
	return x - 1

# Converts list string input to list and reduces node no by 1 to convert to 0,1,2,...,n-1
def convStringToList(listString):
	outList =  listString[1:-1].split(',')
	if(outList == ['']):
		outList = []
	else:
		outList = map(sub1 ,map(int,listString[1:-1].split(',')))
	return outList

def HandleQueries(networkFile , queryFile , outFile):
	# Read Network from file and store it in Gc
	f = open(networkFile,"r")
	input_list = re.split(' |\n|\t',f.read())
	n = int(input_list[0])
	Gc = []
	for i in xrange(1,n+1):
		Gc.append(convStringToList(input_list[2*i]))

	f.close()

	f = open(queryFile,"r")
	input_list = re.split(' |\n|\t',f.read())
	numQueries = len(input_list)/3
	f.close()

	f = open(outFile,"wb")

	# Handle queries using above function and output result in proper format
	for i in xrange(0,numQueries):
		[indep , path ] = findActiveTrail(Gc , int(input_list[3*i])-1, int(input_list[3*i+1])-1 , convStringToList(input_list[3*i+2]))
		if(indep):
			f.write(input_list[3*i]+" "+input_list[3*i+1]+" "+ input_list[3*i+2]+"\n")
			f.write("yes\n")
		else:
			f.write(input_list[3*i]+" "+input_list[3*i+1]+" "+ input_list[3*i+2]+"\n")
			pathstr = "["
			for i in xrange(0,len(path)):
				pathstr += str(path[i])+","
			pathstr = pathstr[:-1] + "]"
			f.write("no "+pathstr+"\n")

	f.close()
	


Gc = genBayesNet(7,np.array([6,5,4,3,2,1,0]),"bn.txt")
print Gc



HandleQueries("sample-bn.txt","sample-query.txt","bn-out.txt")


# print findActiveTrail(Gc , 0 , 2 , [3])










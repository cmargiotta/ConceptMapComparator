import numpy as np
import random

def check_connected(matrix, index):
	for i in matrix:
		if i[index] == 1:
			return True
			
	return False

class ConceptMap:
	root = None
	nodes = []
	matrix = []
	
	def __init__(self, nnodes):
		for i in range(nnodes):
			self.nodes.append(random.random())
			
		for i in range(nnodes):
			self.matrix.append([])
			for j in range(nnodes):
				self.matrix[i].append(0)
			
		c = 1	
		i = 0
		while c < nnodes:
			if i == 0:
				r = random.randint(1, (nnodes - c)/2)
				
				for i in range(1, r+1):
					self.matrix[0][i] = 1
				
				c = c+r
				i = 1

				continue				
			else:
				r = random.randint(0, nnodes - c)
				
			if r == 0:
				continue
			
			i = random.randint(1, nnodes-1)
			while not check_connected(self.matrix, i):
				i = random.randint(1, nnodes-1)
					
			count = 0
			while count < r:
				target = 1
				
				while check_connected(self.matrix, target):
					target = random.randint(1, nnodes-1)
					
				self.matrix[i][target] = 1
				count = count + 1
			
			c = c + r
		
		print(self.matrix)
		
		
		
cm = ConceptMap(5)

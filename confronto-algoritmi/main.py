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
	
	adj_matrix = None
	deg_matrix = None
	lap_matrix = None
	adj_eigenvector = None
	lap_eigenvector = None
	deg_eigenvector = None
	
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
		
		self.adj_matrix = np.matrix(self.matrix)
		
		D = []
		for i in range(nnodes):
			D.append([])
			for j in range(nnodes):
				if i != j:
					D[i].append(0)
				else:
					degree = 0
					for ind in range(nnodes):
						degree = degree + self.matrix[i][ind]
					D[i].append(degree)
		
		self.deg_matrix = np.matrix(D)
		self.lap_matrix = np.subtract(self.deg_matrix, self.adj_matrix)
		self.adj_eigenvector, v = np.linalg.eig(self.adj_matrix)
		self.adj_eigenvector = self.adj_eigenvector
		self.lap_eigenvector, v = np.linalg.eig(self.lap_matrix)
		self.lap_eigenvector = self.lap_eigenvector
		self.deg_eigenvector, v = np.linalg.eig(self.deg_matrix)
		self.deg_eigenvector = self.deg_eigenvector
		
cm = ConceptMap(3)
print(cm.adj_matrix)
print(cm.deg_matrix)
print(cm.lap_matrix)
print(cm.adj_eigenvector)
print(cm.lap_eigenvector)
print(cm.deg_eigenvector)



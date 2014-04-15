import cv2
import numpy as np
import sys
import collections
import os
from IPython.core import ultratb
sys.excepthook = ultratb.FormattedTB(mode='Verbose', color_scheme='Linux', call_pdb=1)



# Retuns a summary of the photos of a person as a single point in the eigenspace
# This is actually assuming that we need to summarize the amount of people will have. We should only implement this later
def get_person_point(person, eigenspace):
	# project each column into the eigenspace
	#for photo in person.col
	print "point"
	# return the average of all the points

def matrix_writer(matrix, f):
	if(len(matrix.shape) == 1):
		matrix_limit = matrix.shape[0] - 1
	else:
		matrix_limit = matrix.shape[0]*matrix.shape[1] - 1
	for i,v in enumerate(matrix.flat):
		f.write(str(v))
		if(i != matrix_limit):
			f.write(",")
	f.write("\n")

# Constants
IM_WIDTH = 100
IM_HEIGHT = 100
k = 15 # The number of eigenvalues to use, as determined by plotting the eigenvalues and seeing where they start to flatten out.

# Other variables
face_imgs = []
people_dict = collections.OrderedDict()
rootdir = "/home/sean/EE4208_Photos/"

# load faces as greyscale and shove them into a matrix
# Keep track of which photos belong to which people using an ordered dictionary
column_count = 0
starting_column = 0
for subdir, dirs, files in os.walk(rootdir):
	if(dirs == []):
		photo_number = 0
		for fi in files:
			if(fi.find("hist_") != -1): #Because regexes take too long to program
				#print "reading "+subdir+"/"+fi
				face_imgs.append(cv2.imread(subdir+"/"+fi,0).reshape(IM_WIDTH*IM_HEIGHT,1))
				photo_number +=1
				column_count += 1
		name = os.path.basename(os.path.normpath(subdir))
		people_dict[name] = (starting_column, column_count-1)
		starting_column = column_count
A = np.concatenate(face_imgs, axis = 1)
A = A.astype(float)

# Get the mean and subtract from each image
mean_face_matrix = np.mean(A,axis=1).T
for c in range(0,A.shape[1]):
	A[:,c] = np.subtract(A[:,c], mean_face_matrix)
magic_matrix = np.dot(A.T,A)

# Get the eigen vectors and avlues
eigen_values, eigen_vectors = np.linalg.eig(magic_matrix)

# Filter the eigenvectors based off their associated eigenvalues
new_index = np.argsort(eigen_values)[::-1] # Note "[::-1]"" just inverses the array
new_index = new_index[:k] # Select only the k biggest values
eigen_vectors = eigen_vectors[:,new_index]

# This part is hard to explain, but basically we are making the eigenspace to compare photos with
A_eigen=np.dot(A, eigen_vectors)
new_A_eigen=A_eigen[:,new_index]
L=np.delete(new_A_eigen,A_eigen.shape[1]-1,1) # Delete the least significant eigen vectors
number_of_eigenvectors = L.shape[1]
people_matrix = np.dot(A.T, L).T # Transposed because we want to project every vector onto L and then untransposed

with open("hist_sean.txt", 'wb') as l:
	l.write(str(number_of_eigenvectors)+"\n")
	matrix_writer(mean_face_matrix, l)
	print "L: "
	print L
	matrix_writer(L, l)
	print "Number of people"
	print people_matrix.shape
	l.write(str(people_matrix.shape[1])+"\n")
	matrix_writer(people_matrix, l)
	print "People: "
	print people_dict
	names = people_dict.keys()
	for index, name in enumerate(names):
		for i in range(people_dict[name][0],people_dict[name][1]+1):
			l.write(name) # Will have to write out the name multiple times, if we're going to give multiple points
			if(index < len(names)-1 or i != people_dict[name][1]):
				l.write(",")

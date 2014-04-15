import numpy as np
import cv2
import sys
import os
from IPython.core import ultratb
#sys.excepthook = ultratb.FormattedTB(mode='Verbose', color_scheme='Linux', call_pdb=1)

IM_WIDTH = 100
IM_HEIGHT = 100
IM_WIDTH_pre = 500
IM_HEIGHT_pre = 500

face_cascade = cv2.CascadeClassifier("/opt/ros/groovy/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml")
rootdir = os.getcwd()

for subdir, dirs, files in os.walk(rootdir):
	if(dirs == []):
		for f in files:
		#img = cv2.imread(os.path.join(name, exp, f))
			if(f.find("resized_") != -1 or f.find("hist_") != -1 or ("hist_"+f) in files):
				#print "Skipping! Already processed " + f
				continue

			print subdir + "/" + f
			img = cv2.imread(subdir + "/" + f)
			gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
			#resized1 = np.zeros((IM_WIDTH_pre, IM_HEIGHT_pre), np.uint8)
			#cv2.resize(gray, (IM_WIDTH_pre, IM_HEIGHT_pre),dst=resized1, interpolation=cv2.INTER_CUBIC)
			print "detecting faces"
			#faces = face_cascade.detectMultiScale(resized1, 1.3, 5) # Yep, that's still slow as hell
			faces = face_cascade.detectMultiScale(gray, 1.3, 5)
			print "found faces " + str(len(faces))
			for (x,y,w,h) in faces:
				#cropped = resized1[y:y+h, x:x+w]
				cropped = gray[y:y+h, x:x+w]
				resized = np.zeros((IM_WIDTH, IM_HEIGHT), np.uint8)
				cv2.resize(cropped, (IM_WIDTH, IM_HEIGHT),dst=resized, interpolation=cv2.INTER_CUBIC)
				resized = cv2.equalizeHist(resized)
				print "writing " + subdir+"/hist_"+ f
				cv2.imwrite(subdir+"/hist_"+ f, resized)

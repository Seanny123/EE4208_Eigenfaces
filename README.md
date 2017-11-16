EE4208_Eigenfaces
=================

Eigenfaces implementation for EE4208, including a brief analysis.

Dependencies
------------
- OpenCV
- Numpy

How to run
-----------

`Multi_Hist_Cropper.py` is the python script that goes through each folder lower than where it's found and crops, resizes and historgram equalizes the picture found within. It currently is optimized to not process photos that have been processed, but is still single-threaded because I'm lazy and don't have Python multi-threading memorized like I do for Ruby.

`sean_hist_pca.py` is the Python script for processing all the pre-formatted pictures into an eigenspace and then projecting each person/photo onto that eigenspace. It then writes the results to a textfile, so they can be read by the C++ program.

`main_hist.cpp` is c++ program that reads in the text file information to build a face database and then compares incoming faces to the database. It's basically copied from an OpenCV tutorial I found somewhere on the Internet, but is probably way less efficient.

FAQ
---

**Why didn't you just write the whole thing in Python?**

I found a tutorial for C++ first and I had trouble getting frames from the camera, but if I could do it all over again, I would totally just suck it up and head for Python.

**Why is it so slow?**

Err... Probably for a lot of reasons.

**Why is it so innacurate?**

This is an old-school implementation of face-recognition. These days, everyone uses Convolutional Neural Networks. But I wrote this back in 2014, before Deep Learning had become mainstream for anything other than image recognition.

**Did you do Edge Detection too?**

Yep!

- [Gaussian Edge Detector](https://gist.github.com/Seanny123/10452462), as well as [plotting the Laplacian of Gaussian Mask](https://gist.github.com/Seanny123/10452919)
- [Canny Edge Detector](https://gist.github.com/Seanny123/10538465)

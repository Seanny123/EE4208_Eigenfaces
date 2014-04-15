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

Well, it's only using a video feed when all the modern approaches use many different inputs with feature tracking (for example see [cob_people_detection](http://wiki.ros.org/cob_people_detection)) and it isn't using any of the [probabilistic improvements you can find here](http://vismod.media.mit.edu/pub/facereco/papers/TR-443.pdf).

**Did you do Edge Detection too?**

Yep! Check out my gists for tons of Python code! Most importantly, how to load those freaking weird `.raw` files.

#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include <boost/algorithm/string.hpp>
#include <stdlib.h>
#include <vector>
#include <string>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <iterator>
#include <sstream>

const int IM_WIDTH = 100;
const int IM_HEIGHT = 100;

// Get frame from the camera
IplImage* get_camera_frame(CvCapture* &camera)
{
	IplImage *frame;
	frame = cvQueryFrame(camera);
	if (!frame) {
		std::cout << "Couldn't get a frame \n";
		exit(1);
	}
	return frame;
}

// NCC classifier
// Where x is the value to compare, m is the points to compare and names is the labels of the points, max_thresh is the maximum threhold
// Returns the label of the point with the nearest center
std::string ncc(const cv::Mat &x, const cv::Mat &m, const std::vector<std::string> &names, const float &max_thresh = 2*std::pow(10,20)){

    double v_min, v_max;
    int idx_min[2] = {255,255}, idx_max[2] = {255, 255};
    //each row is a result
    cv::Mat d = cv::Mat::zeros(m.cols, 1, CV_32F);

    // Hypothetically this should be a single operation, but I'm hoping that C++ is fast enough that I won't notice.
    //std::cout << "FOR" << std::endl;
    for(int i = 0; i < m.cols; i++)
    {
        //std::cout << "x shape " << x.t().size() << " m shape " << m.col(i).size() << std::endl;
        d.row(i) = (x.t() * m.col(i) - 0.5*m.col(i).t()*m.col(i));
    }
    //std::cout << "d" << d << std::endl;
    //std::cout << "MAX" << std::endl;
    cv::minMaxIdx(d, &v_min, &v_max, idx_min, idx_max);
    //std::cout << "idx_min=" << idx_min[0] << ", idx_max=" << idx_max[0] << std::endl;

    if(max_thresh < v_max)
    {
        //std::cout << "accessing index " << idx_max[0] << " names length " << names.size() << std::endl;
        std::cout << "Would have returned " << names[idx_max[0]] << " but v_max = " << v_max << std::endl;
        return "unknown";
    }else{
        return names[idx_max[0]];
    }
}
// The main function
int main(int argc, const char* argv[])
{
std::cout << "Start\n";

    // Load the training data
    //std::ifstream infile("./py_unit_test/hist_sean.txt");
    std::ifstream infile("./py_unit_test/hist_sean.txt");
    int number_of_eigenvectors, number_of_people;
    infile >> number_of_eigenvectors;
    std::cout << "K:" << number_of_eigenvectors << std::endl;

    // Read in the eigenvectors
    std::vector<std::string> temp_vector;
    std::vector<float> into_mat;
    std::vector<float> into_avg;
    std::string temp;

    for(int i = 0; i < 2; i++)
    {
        // Apparently I could use an vector::iterator here instead of a for loop, but I don't know what the motivation behind doing that is
        infile >> temp;
        boost::split(temp_vector, temp, boost::is_any_of(","));
        for(int j = 0; j < temp_vector.size(); j++)
        {
            if(i != 0){
                into_mat.push_back(::atof(temp_vector[j].c_str()));
            }else{
                into_avg.push_back(::atof(temp_vector[j].c_str()));
            }
        }
    }
    cv::Mat face_avg = cv::Mat(into_avg, CV_32F);
    cv::Mat eigenvectors = cv::Mat(into_mat, CV_32F);
    std::cout << "face_avg shape " << face_avg.size() << std::endl;
    std::cout << "eigenvectors shape " << eigenvectors.size() << std::endl;
    std::cout << into_mat.size()/number_of_eigenvectors << "," << number_of_eigenvectors << std::endl;
    eigenvectors = eigenvectors.reshape(1, into_mat.size()/number_of_eigenvectors);
    //Okay, if reshaping is failing, maybe it's time to change how the configuration file is loaded so this shit doesn't happen? // Or we could do the projection by hand, so that OpenCV doesn't have a hissy fit? // Or we could just re-write the whole thing in Python?
    //eigenvectors = eigenvectors.reshape(10000, 5);
    //std::cout << "eigenvectors shape " << eigenvectors.size() << std::endl;
    //std::cout << eigenvectors << std::endl;

    infile >> number_of_people;
    // Read in the people locations
    into_mat.clear();

    infile >> temp;
    //std::cout << temp << std::endl;
    boost::split(temp_vector, temp, boost::is_any_of(","));
    for(int j = 0; j < temp_vector.size(); j++)
    {
        into_mat.push_back(::atof(temp_vector[j].c_str()));
    }

    cv::Mat people = cv::Mat(into_mat, CV_32F);
    //std::cout << "People! " << (people.type() == CV_32F) << std::endl;
    // Each column is a person
    people = people.reshape(1, into_mat.size()/number_of_people);
    std::cout << "people size: " << people.size() <<std::endl;

    // Read in the people names
    std::vector<std::string> names;
    infile >> temp;
    boost::split(names, temp, boost::is_any_of(","));

    // Load the face detection using the HAAR classifiers
    cv::CascadeClassifier face_cascade;
    bool file_result = face_cascade.load("/opt/ros/groovy/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml");
    std::cout << "Did we find the file? " << file_result << std::endl;

	cv::VideoCapture cap(0);
	// Wait a little, so that the camera can auto-adjust its brightness.
	sleep(1);

	cv::Mat current_frame;
	// Quit on "Escape" key.
	while (cvWaitKey(10) != 27) {
		cap >> current_frame;
		cv::Mat original = current_frame.clone();
		cv::Mat gray;
		// Convert to grayscale
		cvtColor(original, gray, CV_BGR2GRAY);
		std::vector< cv::Rect_<int> > faces;
		// Use HAAR to get all the features
		face_cascade.detectMultiScale(gray, faces);

		// iterate through the faces
		for(int i = 0; i < faces.size(); i++){
			cv::Rect face = faces[i];
			// crop the face
			cv::Mat cropped_face = gray(face);
			// resize the face
			cv::Mat resized_face;
			cv::resize(cropped_face, resized_face, cv::Size(IM_WIDTH, IM_HEIGHT), 1.0, 1.0, cv::INTER_CUBIC);

			// NOTE THIS WILL ONLY WORK FOR NON-ROTATED FACES

            // So first we need to project the AVERAGED face into the EigenSpace
            cv::Mat hist_face;
            cv::equalizeHist(resized_face, hist_face);
            //resized_face = hist_face;

            resized_face = resized_face.reshape(1,IM_HEIGHT*IM_WIDTH);
            cv::Mat resized_face_f;
            resized_face.convertTo(resized_face_f, CV_32F);
            //std::cout << "subtract" << std::endl;
            //std::cout << "resized_face shape " << resized_face_f.size() << " face_avg shape " << face_avg.size() << std::endl;
            resized_face_f -= face_avg;
            //std::cout << "multiply" << std::endl;
            //std::cout << "resized_face shape " << resized_face_f.size() << " eigenvectors shape " << eigenvectors.size() << std::endl;
            //std::cout << "resized_face type " << (resized_face_f.type() == CV_32F) << " eigenvectors type " << (eigenvectors.type() == CV_32F) << std::endl;
            resized_face_f = resized_face_f.t() * eigenvectors;
			// then we just compare the centers
			std::string box_text = ncc(resized_face_f.t(), people, names);

			rectangle(original, face, CV_RGB(0, 255, 0), 1);
			int pos_x = std::max(face.tl().x - 10, 0);
			int pos_y = std::max(face.tl().y - 10, 0);
			cv::putText(original, box_text, cv::Point(pos_x, pos_y), cv::FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0, 255, 0), 2.0);
		}
		imshow("face_recognizer", original);
	}

	std::cout << "Done\n";
	return 0;
}

//
//  functions.cpp
//  featureMatching
//
//  Created by Jeff Ithier on 3/04/2015.
//  Copyright (c) 2015 jeff. All rights reserved.
//

#include <iostream>
#include <fstream>

#include <boost/filesystem.hpp>
#include <opencv2/features2d/features2d.hpp>

#include "functions.h"

#include "ImageFrame.h"

namespace fs = ::boost::filesystem;     // Shorthand for boost filesystem namespace.
using namespace std;
using namespace cv;

/* Read all jpgs in a given directory */

void getAllJPG(fs::path& path, vector<fs::path>& files){
    
    if (!fs::exists(path) || !fs::is_directory(path)){
        cout << "Path is not a valid directory." << endl;
        exit(0);
    }
    
    fs::recursive_directory_iterator it(path);  // Iterator for this directory.
    fs::recursive_directory_iterator endit;     // Indicating the end of directory.
    
    while (it != endit){
        if (fs::is_regular_file(*it)){
            if (it->path().extension() == ".jpg"){
                files.push_back(it->path().filename());
                ++it;
            }else{
                ++it;
            }
        }
    }
    
};

/* Write keypoints to file in SIFT format. */
void writeKeyPointsToFile(vector<KeyPoint>* inputPoints, string imageName){
    
    ofstream outputFile;
    
    outputFile.open("outputFiles/features/"+imageName+".sift");
    
    long numKeyPoints = inputPoints->size();
    
    // Num keypoints followed by "128"
    outputFile << numKeyPoints << " 128" << endl;
    
    for (int i = 0; i < numKeyPoints; i++) {
        
        // Image coordinate followed by two zeros.
        outputFile << inputPoints->at(i).pt.x << " " << inputPoints->at(i).pt.y << " 0" << " 0" << endl;
        
        
        // Six rows of 20 zeros.
        for(int j = 0; j < 6; j++){
            
            for (int k = 0; k < 20; k++){
                outputFile << 0 << " ";
            }
            
            outputFile << endl;
            
        }
        
        // One row of 8 zeros/
        for (int j = 0; j < 8; j++){
            outputFile << 0 << " ";
        }
        
        outputFile << endl;
        
    }
    
    outputFile.close();
    
}

/* Write .txt file containing matches between all keypoints in all images to use in vsfm. */

void writeMatchesToFile(vector<ImageFrame>* images, vector<DMatch>* matches, int idx1, int idx2){
    
    ofstream outputFile;
    outputFile.open("outputFiles/matches.txt", ofstream::out | ofstream::app);      // Open file in append mode if it exists.
    
    /* Printing the two images matched and no. matches. */
    outputFile << images->at(idx1).getFileName() << " " << images->at(idx2).getFileName()
                << " " << matches->size() << endl;
    
    /* Print all keypoint indexes from image 1. */
    
    for (int i = 0; i < matches->size(); i++){
        outputFile << matches->at(i).queryIdx << " ";
    }
    outputFile << endl;
    
    /* Print keypoint indexes matching those listed above. */
    
    for (int i = 0; i < matches->size(); i++) {
        outputFile << matches->at(i).trainIdx << " ";
    }
    outputFile << endl;
    
}

/* Print all descriptors to file to use in the live camera pose estimation. */
// - Print new file for each image (to preserve floating precision).

void writeDescriptorsToFile(vector<ImageFrame>* images){
    
    
    /* Write the file name + descriptor matrix for each image individually. */
    
    for (int i=0; i < images->size(); i++) {
        
        Mat des = images->at(i).getDescriptors();
        string fileName = images->at(i).getFileStem();
        
        /* Open a new file. */
        
        cv::FileStorage file("outputFiles/descriptors/"+fileName+".yml", cv::FileStorage::WRITE);
        
        /* Write descriptors to the file. */
        
        file << images->at(i).getFileStem() << des;
        
        file.release();
        
    }
    
}

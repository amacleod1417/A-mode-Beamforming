#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include <algorithm>
using namespace std;

float **createDataMatrix(int numElement, int numSample)
{
    // Create a 2D data matrix of size numElement and numSample
    float** RFData = new float*[numElement];

    // Allocate memory for each row (float array)
    for (int i = 0; i < numElement; ++i) {
        RFData[i] = new float[numSample];
    }

    return RFData;
}


int loadRFData(float **RFData, const char *fileName, int numElement, int numSample)
{
    // Open the text file fileName, read the data and store into RFData
    ifstream inputFile (fileName);

    // Check if the file is successfully opened
    if (!inputFile) {
        cerr << "Could not open the file " << fileName << endl;
        return -1; 
    }

    // Use the getline() command to extract the data lines from the txt files

   char line[30];
    for (int i = 0; i < numElement; ++i) {

        for (int j = 0; j < numSample; ++j) {
            inputFile.getline(line, 29);
            //getline(inputFile,line);
            RFData[i][j] = atof(line);
        
        }
    // Close file after reading
    
    }
    inputFile.close();

    return 0; 
}



// Create an array containing the depth location (in z-direction) for each pixel on the scanline
float *genScanlineLocation(int &numPixel)
{
    // Allocate scanline array

        float scanlinedepth;
        cout<< "input desired scaline depth"<< endl;
        cin>> scanlinedepth;
        cout<< "input desired number of pixels"<< endl;
        cin >> numPixel;
        float* scanlineLocation = new float[numPixel];


        for (int i = 0; i < numPixel; ++i) {

        scanlineLocation[i] = scanlinedepth/(numPixel-1) *i;

        }

        return scanlineLocation;
    
    }






// Create an array containing the element location (in x-direction) of the ultrasound transducer
float *genElementLocation(int numElement, float PITCH)
{
    // Create array to store the transducer element locations
    float* eleLocation = new float[numElement];

    // Calculate the physical location of each transducer element
    for (int n = 0; n < numElement; ++n)
    {
       eleLocation[n] = (n - ( (float)numElement - 1) / 2.0) * PITCH;
    }


    return eleLocation;
}



// Allocate memory to store the beamformed scanline
float *createScanline(int numPixel)
{
    float *scanline = new float[numPixel];
    return scanline;
  
}

// Beamform the A-mode scanline
void beamform(float *scanline, float **realRFData, float **imagRFData, float *scanlinePosition, float *elementPosition, int numElement, int numSample, int numPixel, float FS, float SoS)
{

 //initialize arrays for times

float tforward[numPixel];
float tbackward[numPixel][numElement];
float ttotal[numPixel][numElement];


    // Iterate over each pixel
    for (int i = 0; i < numPixel; ++i)
    {
        // initiate p-real and p-imag for each pixel
        float Preal = 0.0;
        float Pimag = 0.0;
        tforward[i] = scanlinePosition[i] / SoS;

        // Iterate over each element on the scanline
        for (int k = 0; k < numElement; ++k)
        {
        
            float distance = sqrt(pow(scanlinePosition[i], 2) + pow(elementPosition[k], 2));
    
            // Calculate the backward time
            tbackward [i][k]= distance / SoS;

            // Calculate the total time
            ttotal [i][k] = tforward [i] + tbackward [i][k];

            // Calculate total number of samples corresponding to the total time - round from float to int
            int s = floor(ttotal[i][k] * FS);

            // check if the sample index is within bounds - where is check?
                // add corresponding values to p-real and p-imag
            Preal += realRFData[k][s];
            Pimag += imagRFData[k][s];
            //cout << Preal << " ";
        
        }

        // calculate magnitude and assign it inside scanline
        float mag = sqrt(pow(Preal, 2) + pow(Pimag, 2));
        scanline[i] = mag;
      

    }

}




    


// Write the scanline to a csv file
int outputScanline(const char *fileName, float *scanlinePosition, float *scanline, int numPixel)
{
// Open the file for writing
    ofstream outputFile(fileName);

    // Check if the file is successfully opened
    if (!outputFile)
    {
        cerr << "Error: Could not create or open the file " << fileName << " for writing." << endl;
        return -1; 
    }


    // Write scanline data to the output file
    for (int i = 0; i < numPixel; ++i)
    {
        outputFile << scanlinePosition[i] << "," << scanline[i] << endl;
    }

    // Close the file after writing
    outputFile.close();

    return 0; 
}
 


// Destroy all the allocated memory
void destroyAllArrays(float *scanline, float **realRFData, float **imagRFData, float *scanlinePosition, float *elementPosition, int numElement, int numSample, int numPixel)
{
    // Deallocate memory for scanline
    delete scanline;

    // Deallocate memory for realRFData
    for (int i = 0; i < numElement; ++i)
    {
        delete realRFData[i];
    }
    delete realRFData;

    // Deallocate memory for imagRFData
    for (int i = 0; i < numElement; ++i)
    {
        delete imagRFData[i];
    }
    delete imagRFData;

    // Deallocate memory for scanlinePosition
    delete scanlinePosition;

    // Deallocate memory for elementPosition
    delete elementPosition;
}
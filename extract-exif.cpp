#include <stdio.h>
#include <vector>
#include <iostream>
#include <fstream>

#ifdef CPP_EXPERIMENTAL
#include <experimental/filesystem>
typedef std::vector<std::string> (*get_file_names)(std::experimental::filesystem::path);
#elif _WIN32 
#include <windows.h>
#include <fileapi.h>
typedef std::vector<std::string> (*get_file_names)(char*);
#elif __linux__
#include <dirent.h>
typedef std::vector<std::string> (*get_file_names)(char*);
#endif

#include "exif.h"

#define MEGABYTES(value)  1000000*(value)

static int
FileToBuffer(char *Filename, unsigned  char *OutputBuffer)
{
    FILE *fp = fopen(Filename, "rb");
    if(!fp) return(-1);
    
    fseek(fp, 0, SEEK_END);
    unsigned long fsize = ftell(fp);
    rewind(fp);
    //unsigned char *buf = new unsigned char[fsize];
    if (fread(OutputBuffer, 1, fsize, fp) != fsize) 
        return(-2);
    
    return(fsize);
}

#ifdef CPP_EXPERIMENTAL
//NOTE(George): This function works only if Path is a directory
static std::vector<std::string>
CPPEXGetFileNames(std::experimental::filesystem::path Path)
{
    namespace stdfs = std::experimental::filesystem;
    std::vector<std::string> FileNames;
    
    const stdfs::directory_iterator end{};
    
    for(stdfs::directory_iterator iter{Path}; iter != end; ++iter)
    {
        if(stdfs::is_regular_file(*iter))
            FileNames.push_back(iter->path().string());
        
    } 
    return(FileNames);
}

get_file_names GetFileNames = CPPEXGetFileNames;

#elif _WIN32
//NOTE(George): This functions works for any Path regular 
//expression or single file
static std::vector<std::string>
WINGetFileNames(char * Path)
{
    
    TCHAR Buffer[4096];
    std::vector<std::string> FileNames;
    WIN32_FIND_DATA Data;
    
    //Get the file's full directory name
    GetFullPathName(Path, 4096, Buffer, 0);
    std::string BaseDir = std::string(Buffer);
    int found =  BaseDir.find_last_of("/\\");
    BaseDir = BaseDir.substr(0, found).append(1,'\\');
    
    
    HANDLE hFind = FindFirstFile(Path, &Data);
    if(hFind != INVALID_HANDLE_VALUE)
    {
        do 
        {
            printf("%s\n", Data.cFileName);
            GetFullPathName(Path, 4096, Buffer,0);
            FileNames.push_back(BaseDir + std::string(Data.cFileName));
        } while(FindNextFile(hFind, &Data));
    }
    
    FindClose(hFind);
    return(FileNames);
}

get_file_names GetFileNames = WINGetFileNames;

#elif __linux__
//Not tested, 100% has bugs
static std::vector<std::string>
LINUXGetFileNames(char* Path)
{
    std::vector<std::string> FileNames;
    
    struct dirent **namelist;
    int n;
    
    n = scandir(Path, &namelist, 0, alphasort);
    //if (n < 0)
    //perror("scandir");
    
    if(n >= 0)
    {
        while (n--)
        {
            FileNames.push_back(std::string(namelist[n]->d_name));
            free(namelist[n]);
        }
        free(namelist);
    }
    
    return(FileNames);
}

get_file_names GetFileNames = LINUXGetFileNames;
#endif

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        printf("Usage: extract-exif [file(s)]\n");
        return(-10);
    }
    
    std::vector<std::string> FileNames; 
    FileNames = GetFileNames(argv[1]);
    //Let's say we wont be processing pictures larger than 10 MB
    unsigned char *Buffer = new unsigned char[MEGABYTES(10)];
    
    
    std::ofstream OutputFile("out.txt", std::ofstream::out);
    
    //Open, parse each file and dump header info to OutputFile
    for(std::string FileName : FileNames)
    {
        printf("%s: ", FileName.c_str());
        
        int FileSize = FileToBuffer((char *)FileName.c_str(), Buffer);
        
        //Parse EXIF
        easyexif::EXIFInfo Result;
        int Code = Result.parseFrom(Buffer, FileSize);
        
        
        if(Code)
            printf("Error code %d\n", Code);
        //TODO(George): Make this output more modular...
        else
        {
            printf("OK\n");
            
            //Dump almost all data....
            /*
            OutputFile << Result.ByteAlign << " ";
            OutputFile << Result.ImageDescription << " ";
            OutputFile << Result.Make << " ";
            OutputFile << Result.Model << " ";
            OutputFile << Result.Orientation << " ";
            OutputFile << Result.BitsPerSample << " ";
            OutputFile << Result.Software << " ";
            OutputFile << Result.DateTime << " ";
            OutputFile << Result.DateTimeOriginal << " ";
            OutputFile << Result.DateTimeDigitized << " ";
            OutputFile << Result.SubSecTimeOriginal << " ";
            OutputFile << Result.Copyright << " ";
            OutputFile << Result.ExposureTime << " ";
            OutputFile << Result.FNumber << " ";
            OutputFile << Result.ExposureProgram << " ";
            OutputFile << Result.ISOSpeedRatings << " ";
            OutputFile << Result.ShutterSpeedValue << " ";
            OutputFile << Result.ExposureBiasValue << " ";
            OutputFile << Result.SubjectDistance << " ";
            OutputFile << Result.FocalLength << " ";
            OutputFile << Result.FocalLengthIn35mm << " ";
            OutputFile << Result.Flash << " ";
            OutputFile << Result.FlashReturnedLight << " ";
            OutputFile << Result.FlashMode << " ";
            OutputFile << Result.MeteringMode << " ";
            OutputFile << Result.ImageWidth << " ";
            OutputFile << Result.ImageHeight << " ";
            OutputFile << Result.GeoLocation.Latitude << " ";
            OutputFile << Result.GeoLocation.Longitude << " ";
            OutputFile << Result.GeoLocation.Altitude << " ";
            OutputFile << Result.GeoLocation.AltitudeRef << " ";
            OutputFile << Result.GeoLocation.DOP << " ";
            OutputFile << Result.LensInfo.FStopMin << " ";
            OutputFile << Result.LensInfo.FStopMax << " ";
            OutputFile << Result.LensInfo.FocalLengthMin << " ";
            OutputFile << Result.LensInfo.FocalLengthMax << " ";
            OutputFile << Result.LensInfo.FocalPlaneXResolution << " ";
            OutputFile << Result.LensInfo.FocalPlaneYResolution << "\n";
            */
            OutputFile << Result.ExposureTime << ";";
            OutputFile << Result.ISOSpeedRatings << ";";
            OutputFile << Result.ShutterSpeedValue << ";";
            OutputFile << Result.GeoLocation.Latitude << ";";
            OutputFile << Result.GeoLocation.Longitude << ";";
            OutputFile << Result.GeoLocation.Altitude << "\n";
        }
        //NOTE(George): To open in matlab
        //fid = fopen('out.txt'); C = textscan(fid, '%f;%f;%f;%f;%f;%f');
    }
    
    delete[] Buffer;
    return(0);
}
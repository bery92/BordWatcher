/************************************************************************/
/*                     C/C++ Library                                    */
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ConfigOptions.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <atlstr.h>



/************************************************************************/
/*                     OpenCV Library                                   */
/************************************************************************/

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>


#define minHessian 100
#define C 0.05
#define R 0.05


cv::Mat imagine;
cv::VideoCapture capture;
int line; 
int cols;
float **average;
int DIVIDER = 1; 
const char *output;
const char *mask_folder;
int CONTOR=0;
char video[50][100];
char mask[50][100];
char year[5],month[3],day[3],hour[3],min[3],sec[3];
bool ok=true;
CString store_name;
char ch[100]={0};
void analiza(cv::Mat,char *);
cv::Mat  procesare(const char *);
bool ListDirectoryMask(const char *);
bool ListDirectoryVideo(const char *sDir)
{
	WIN32_FIND_DATA fdFile;
	HANDLE hFind = NULL;
	char aux2[50]={0};
	char path[200] = { 0 };
	char sPath[2048];
	char *p;

	sprintf(sPath, "%s\\*.*", sDir);

	if((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
	{
		printf("Path not found: [%s]\n", sDir);
		return false;
	}	

	do
	{

		if(strcmp(fdFile.cFileName, ".") != 0
			&& strcmp(fdFile.cFileName, "..") != 0)
		{
			sprintf(sPath, "%s%s", sDir, fdFile.cFileName);


			if(fdFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY)
			{

			}
			else
			{
				year[0] =fdFile.cFileName[0];
				year[1] =fdFile.cFileName[1];
				year[2] =fdFile.cFileName[2];
				year[3] =fdFile.cFileName[3];
				year[4] =0;

				month[0]=fdFile.cFileName[4];
				month[1] =fdFile.cFileName[5];
				month[2] =0;

				day[0] = fdFile.cFileName[6];
				day[1] =fdFile.cFileName[7];
				day[2] = 0;

				hour[0] = fdFile.cFileName[8];
				hour[1] = fdFile.cFileName[9];
				hour[2] = 0;

				min[0] = fdFile.cFileName[10];
				min[1] = fdFile.cFileName[11];
				min[2] = 0;

				sec[0] = fdFile.cFileName[12];
				sec[1] = fdFile.cFileName[13];
				sec[2] = 0;
				printf("%s-%s-%s-%s-%s-%s\n",year,month,day,hour,min,sec);
			
				

				sprintf_s(path,"%s",sDir);
				p = strtok (path,"\\");
				while (p != NULL)
				{
					sprintf_s(ch,"%s",p);
					p = strtok (NULL, "\\");
				}
				
				imagine = procesare(sPath);


				ListDirectoryMask(mask[CONTOR]);
				ok = true;
				

			}
		}
	} while(FindNextFile(hFind, &fdFile)); 

	FindClose(hFind);
	return true;
}



bool ListDirectoryMask(const char *sDir)
{
	WIN32_FIND_DATA fdFile;
	HANDLE hFind = NULL;
	char aux2[50]={0};
	char path[200] = { 0 };
	char sPath[2048];	


	sprintf(sPath, "%s\\*.*", sDir);

	if((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
	{
		printf("Path not found: [%s]\n", sDir);
		return false;
	}	

	do
	{

		if(strcmp(fdFile.cFileName, ".") != 0
			&& strcmp(fdFile.cFileName, "..") != 0)
		{
			sprintf(sPath, "%s%s", sDir, fdFile.cFileName);


			if(fdFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY)
			{

			}
			else
			{
				printf("Processing %s\n", sPath);
				analiza(imagine,sPath);
				
			}
		}
	} while(FindNextFile(hFind, &fdFile)); 

	FindClose(hFind);
	return true;
}





cv::Mat processCanny(cv::Mat image_O)
{
	cv::Canny(image_O, image_O, 70, 170, 3, false);
	return image_O;
}

void showImage(const char *name, cv::Mat img)
{
	cv::namedWindow(name);
	cv::imshow(name, img);
}

void allocateMatrix()
{
	average = (float **)malloc(line*sizeof(float*));

	for(int i = 0;i < line; i++)
		average[i]=(float *)malloc(cols*sizeof(float));

	for(int j = 0;j < line; j++)
		for(int k = 0;k < cols; k++)
			average[j][k] = 0.0;
}

void addCannyFrame(cv::Mat img)
{
	for(int j = 0; j < line; j++)
		for(int k = 0; k < cols; k++)
			average[j][k] += img.at<uchar>(j,k);

}

cv::Mat calAverage(int i, cv::Mat img)
{
	for(int j = 0; j < line; j++)
		for(int k = 0; k < cols; k++)
			average[j][k] /= i;

	for(int j = 0; j < line; j++)
		for(int k = 0; k < cols; k++)
			img.at<uchar>(j,k)=(int)average[j][k];

	return img;

}

cv::Mat procesare(const char * path)
{
	cv::Mat img;
	cv::Mat result;	
	bool ok=true;
	int i;
	int total_nr_frames = 0;

	capture.open(path);

	capture.read(imagine);
	line = img.rows;
	cols = img.cols;

	if(!capture.isOpened())
	{
		printf("Error on video opening\n");
		return imagine;
	}
	allocateMatrix();
	while(capture.read(img))
	{
		for(i=0; i<DIVIDER-1; i++)
		{
			ok=capture.read(img);
			if(!ok)
				break;
		}
		if(ok)
		{
			showImage("Original", img);
			cv::waitKey(1);
			total_nr_frames++;
			result = processCanny(img);
			addCannyFrame(result);		
		} 
		else
		{
			printf("\nBad\n");
			break;
		}
	}
	printf("\nTotal number of frames %d\n",total_nr_frames);
	result=calAverage(total_nr_frames, result);
	
	return result;

}

void analiza(cv::Mat img, char *mask)
{
	char out_path[500]={0};
	char out_ext[500] ={0};
	char procent[10]={0};
	int info;

	cv::Mat work_img = img.clone();
	printf("\nNumar de canale %d\n",work_img.channels());
	cv::Mat report = img.clone();
	printf("masca %s\n",mask);
	cv::Mat imgMask = cv::imread(mask,1);
	if(!imgMask.data)
	{
		printf("Bad mask");
		return;
	}
	cv::cvtColor(imgMask,imgMask,cv::COLOR_BGR2GRAY);
	printf("Canale masca %d\n",imgMask.channels());
	showImage("MASK",imgMask);
	showImage("imaginea",work_img);

	for(int i = 0; i < imgMask.rows; i++ )
		for(int j = 0; j < imgMask.cols; j++)
		{
			if (imgMask.at<uchar>(i,j)==0)
			{

				work_img.at<uchar>(i,j)=(int)0;
			}

		}

	int totalPixel = 0;
    float totalValue = 0.0;

		for(int i = 0; i < imgMask.rows; i++ )
			for(int j = 0; j < imgMask.cols; j++)
			{	
				if (imgMask.at<uchar>(i,j)>0)
				{
				
					if(work_img.at<uchar>(i, j)>=10)
						totalValue += 1;
				totalPixel++;
				}
			}

			showImage("Dupa_mascare",work_img);

			float percentage = totalValue / totalPixel;
			printf("Val %f Pix %d\n",totalValue,totalPixel);
			printf("Procent %f \n",percentage);
			if (percentage <0.4 && ok)
			{

				sprintf_s(out_path,"%s%s_%s_%s_%s_%s.jpg",output,year,month,day,hour,min);
				sprintf_s(out_ext,"%s_%s_%s_%s_%s.jpg",year,month,day,hour,min);
				printf("\n%s\n",out_path);

				cv::imwrite(out_path,report);
				CString start = "uploadIMG";
			   

				// ./upload.exe filepath filename store=sda#date=2013-03-11#hour=12:34#ch=CH02
				ok = false;
				info = percentage * 300;
				itoa(info,procent,10);
				system((CString)"echo " + out_path + " " + out_ext + " " + "#store="+store_name+"#date="+year+"-"+month+"-"+day+"#hour="+hour+":"+min+"#ch="+ch);
				system(start+ " " +out_path + " " + out_ext + " " + "#store="+store_name+"#date="+year+"-"+month+"-"+day+"#hour="+hour+":"+min+"#ch="+ch+"#info="+procent);

				cv::waitKey(1);
			
			}
			
			
}

int main(int argc, char *argv[])
{
	
	char path_v[5000]={0};
	char path_m[5000]={0};
	char *p,*pp;
	

	int i,nr_ch; 
	bool ok = true;

	if(argc < 2)
	{
		printf("Usage: %s <config file>\n", argv[0]);
		exit(EXIT_FAILURE);

	}

	ConfigOptions fin = ConfigOptions::buildFromFile(argv[1]);
	CString video_path= fin.from("MERC").get("VIDEO_PATH");
	CString mask_path= fin.from("MERC").get("MASK_PATH");
	CString result_path= fin.from("MERC").get("RESULT_PATH");
	CString sample_rate= fin.from("MERC").get("SAMPLE_RATE");
	store_name = fin.from("MERC").get("STORE_NAME");
	
	DIVIDER = atoi(sample_rate.GetString());
	output = result_path.GetString();
	const char *vid = video_path.GetString();
	mask_folder = mask_path.GetString();
	strcpy(path_v, vid);
	strcpy(path_m, mask_folder);
	
	p = strtok(path_v, ",");

	i=0;
 	while (p!=NULL)
 	{
 		strcpy(video[i],p);
	//	printf("%s\n",video[i]);
		i++;
		p = strtok(NULL,",");
	//	scanf("%d",&i);
 	}
	nr_ch=i;

	pp = strtok(path_m,",");
	i=0;
	
	while (pp!=NULL)
	{
		strcpy(mask[i],pp);
	//	printf("%s\n",mask[i]);
		i++;
		pp = strtok(NULL,",");
	//	scanf("%d",&i);

	}
	
	for(CONTOR = 0 ; CONTOR < nr_ch; CONTOR++)
	{
		ListDirectoryVideo(video[CONTOR]);
	}
	
	cv::waitKey(1);

	return 0;

}
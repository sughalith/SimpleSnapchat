#include <cv.hpp>
#include <highgui.h>
#include <SDL2/SDL.h>
//#include <SDL2/SDL2_gfxPrimitives.h>
#include "findLargestFieldWithColor.h"
#include <cmath>
#include <vector>
#include <iostream>
//#include <SDL2/SDL_mixer.h>

//#define MUS_PATH "epic.ogg"

//Mix_Music *music = NULL;

using namespace cv;
using namespace std;

class TransformableMat: public Mat
{
	public:
			
		SDL_Surface* transformMatToSDLSurface(){
			
			return SDL_CreateRGBSurfaceFrom(
				this->data, this->cols, this->rows,
				elemSize()*8,
				elemSize()*this->cols,
				0xff0000, 0x00ff00, 0x0000ff, 0
			);

	
		};
		
		TransformableMat& operator=(const Mat& m){
				Mat::operator=(m);
				return *this;
		}
	
};

void drawCircle(unsigned int R, int Ox, int Oy, SDL_Surface* surface, Uint32 RGBA);
void drawPixel(int x, int y, SDL_Surface* surface, Uint32 RGBA);
void initColor(SDL_Surface* surface, Uint32 RGBA);

int main()
{
	
	SDL_Window* gWindow = NULL;
	//SDL_Surface* gScreenSurface = NULL;
	SDL_Surface* gThingsToDisplay = NULL;
	SDL_Surface* gThingsToDraw = NULL;
	SDL_Texture* gTextureToRender = NULL;
	SDL_Renderer* gRender = NULL;
	SDL_Event event;
	SDL_Rect gThingsToDrawRange;
	unsigned int brushSize = 4;
	
	vector<SDL_Rect> gFaceDrawingRangeList;
	
	SDL_Init( SDL_INIT_EVERYTHING );
	
	/*gScreenSurface =*/
	
    VideoCapture cap(0);
    TransformableMat inputFrame;
    TransformableMat orginalFrame;
    CascadeClassifier faceDetector("haarcascade_frontalface_default.xml");
    vector<Rect> faces;
    bool spaceHit = false;
    bool keepRunning = true;
    bool clearNow = false;
    Scalar low(0,100,140);
    Scalar high(30,190,255);
    Rect closestFace;
    unsigned int faceSearchPeriod = 3;
    unsigned int faceSearchCount = faceSearchPeriod;
    unsigned int closestFaceRefreshPeriod = 5;
    unsigned int closestFaceRefreshCount = closestFaceRefreshPeriod;
    
    if(cap.read(inputFrame)) {
		
			gWindow = SDL_CreateWindow(
						"Simple Snapchat",
						SDL_WINDOWPOS_UNDEFINED,
						SDL_WINDOWPOS_UNDEFINED,
						inputFrame.cols,
						inputFrame.rows,
						SDL_WINDOW_SHOWN
					);
			
			gThingsToDraw = SDL_CreateRGBSurface(0,
                                  inputFrame.cols,
                                  inputFrame.rows,
                                  32,
                                  0xff000000,
                                  0x00ff0000,
                                  0x0000ff00,
                                  0x000000ff);
			initColor(gThingsToDraw,0x000000ff);
			
			/*gThingsToDisplay = SDL_CreateRGBSurface(0,
                                  inputFrame.cols,
                                  inputFrame.rows,
                                  32,
                                  0xff000000,
                                  0x00ff0000,
                                  0x0000ff00,
                                  0x000000ff);*/
			
			
			gThingsToDrawRange = {inputFrame.cols/2,inputFrame.rows/2,0,0};
			cout<<"init"<<endl;
	}
	
	SDL_GetWindowSurface( gWindow );
	
	 gRender = SDL_CreateRenderer(
					gWindow,
					-1,
					SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
				);
	
	//namedWindow("test", 1);
    
    do
    {
		
		if(cap.read(inputFrame)) {
			
			++faceSearchCount;
			++closestFaceRefreshCount;
			
			orginalFrame = inputFrame.clone();

			cvtColor( inputFrame, inputFrame, CV_BGR2GRAY );

			if(faceSearchCount >= faceSearchPeriod) {
				
				faces.clear();
				gFaceDrawingRangeList.clear();
				
				faceSearchCount = 0;
				faceDetector.detectMultiScale(inputFrame, faces);
			}
			
			
			if(closestFaceRefreshCount >= closestFaceRefreshPeriod) {
				
				closestFaceRefreshCount = 0;
				
				if(!faces.empty())
					closestFace = faces.at(0);
				
				for(int i = 0; i<faces.size(); ++i)
				{
					
					if(abs( faces.at(i).tl().x - orginalFrame.cols/2 )< abs( closestFace.tl().x - orginalFrame.cols/2 ))
						closestFace = faces.at(i);

				}
			
			}
			
			//line(orginalFrame, closestFace.tl(), closestFace.tl()+Point(0,closestFace.height), Scalar(255,255,0), 2);
			//line(orginalFrame, closestFace.tl()+Point(0,closestFace.height), closestFace.tl()+Point(closestFace.width,closestFace.height), Scalar(255,255,0), 2);
			
			flip(orginalFrame,orginalFrame,1);
			gThingsToDisplay = orginalFrame.transformMatToSDLSurface();
			
			if(spaceHit) {
				
					/*cout<<"drawing"<<endl;
					
					if(SDL_Init(SDL_INIT_AUDIO) < 0)
					return -1;
					
					if(Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2 , 4096 ) == -1)
					return -1;
					
					music = Mix_LoadMUS(MUS_PATH);
					if(music == NULL)
					return -1;
					
					if(Mix_PlayMusic(music, -1) == -1)
					return -1;
					while (Mix_PlayingMusic() );*/
					
					Point p = findLargestFieldWithColor(orginalFrame, low, high);
					
					p -= ( closestFace.tl()
							+ Point(
								(closestFace.width - gThingsToDraw->w)/2,
								(closestFace.height - gThingsToDraw->h)/2
								)
						);
					
					if(p.x<0)
						p.x = 0;
					if(p.y<0)
						p.y = 0;
					
					//cout<<"p:"<<p.x<<";"<<p.y<<endl;
						
					drawCircle(brushSize, p.x, p.y, gThingsToDraw, 0xff0000ff);
					
					//cout<<"test1"<<endl;
					
					if(gThingsToDrawRange.x > p.x-brushSize) {
							
							gThingsToDrawRange.w += gThingsToDrawRange.x - (p.x-brushSize);
							gThingsToDrawRange.x = p.x-brushSize;
	
					}
					if(gThingsToDrawRange.y > p.y-brushSize) {
							
							gThingsToDrawRange.h += gThingsToDrawRange.y - (p.y-brushSize);
							gThingsToDrawRange.y = p.y-brushSize;
	
					}
					
					if(gThingsToDrawRange.x+gThingsToDrawRange.w < p.x+brushSize) {
						
							gThingsToDrawRange.w += p.x+brushSize - (gThingsToDrawRange.x+gThingsToDrawRange.w);
							
					}
					
					if(gThingsToDrawRange.y+gThingsToDrawRange.h < p.y+brushSize) {
					
							gThingsToDrawRange.h += p.y+brushSize - (gThingsToDrawRange.y+gThingsToDrawRange.h);
							
					}
					//Mix_FreeMusic(music);
					//Mix_CloseAudio();
					
			}
			
			
			gFaceDrawingRangeList.reserve(faces.size());
					
			for(int i = 0; i < faces.size(); ++i)
			{
					Rect face = faces.at(i);
					SDL_Rect gFaceDrawingRange;
					gFaceDrawingRange.x = face.tl().x - (gThingsToDrawRange.w - face.width)/2;
					gFaceDrawingRange.y = face.tl().y - (gThingsToDrawRange.h - face.height)/2;
					gFaceDrawingRange.w = gThingsToDrawRange.w;
					gFaceDrawingRange.h = gThingsToDrawRange.h;
					gFaceDrawingRangeList.push_back(gFaceDrawingRange);
						
						
					cout<< gFaceDrawingRange.x <<endl;
					cout<< gFaceDrawingRange.y <<endl;
					cout<< gFaceDrawingRange.w <<endl;
					cout<< gFaceDrawingRange.h <<endl;
			}
					
			for(int i = 0; i<gFaceDrawingRangeList.size(); ++i)
					SDL_BlitSurface(gThingsToDraw, &gThingsToDrawRange, gThingsToDisplay, &(gFaceDrawingRangeList.at(i)));
			
			if(clearNow) {
				
				clearNow = false;
				
				initColor(gThingsToDraw,0x000000ff);
			
			}
			
			//imshow("test out", orginalFrame );
			gTextureToRender = SDL_CreateTextureFromSurface(gRender, gThingsToDisplay);
			SDL_RenderClear(gRender);
			SDL_RenderCopy(gRender, gTextureToRender, NULL, NULL);
			SDL_RenderPresent(gRender);
		
		}
		
		while( SDL_PollEvent( &event ) != 0 )
		{
				if( event.type == SDL_QUIT )
					keepRunning = false;
				else if( event.type == SDL_KEYDOWN )
					if(event.key.keysym.sym == SDLK_SPACE)
						spaceHit = ! spaceHit;
					else if(event.key.keysym.sym == SDLK_ESCAPE)
						keepRunning = false;
					else if(event.key.keysym.sym == SDLK_DELETE)
						clearNow = true;
		}
	}
	while(keepRunning);
	
	SDL_FreeSurface(gThingsToDraw);
	SDL_DestroyWindow( gWindow );
	SDL_Quit();
	
	return 0;
	
}

void drawCircle(unsigned int r, int Ox, int Oy, SDL_Surface* surface, Uint32 RGBA) {
		
		
		
		for(int R = r; R>0; --R)
		{
			int i = 0;
			int j = R;
			int f = 5-4*R;
			
			drawPixel(i + Ox,j + Oy,surface,RGBA);
			drawPixel(j + Ox,i + Oy,surface,RGBA);
			drawPixel(i + Ox,-j + Oy,surface,RGBA);
			drawPixel(j + Ox,-i + Oy,surface,RGBA);
			drawPixel(-i + Ox,j + Oy,surface,RGBA);
			drawPixel(-j + Ox,i + Oy,surface,RGBA);
			drawPixel(-i + Ox,-j + Oy,surface,RGBA);
			drawPixel(j + Ox,i + Oy,surface,RGBA);
			
			while(i <= j)
			{
					
					if(f>0){
						f = f + 8*(i-j) -20;
						--j;
					}
					else
						f = f + 8*i +12;
					
					++i;
					drawPixel(i + Ox,j + Oy,surface,RGBA);
					drawPixel(j + Ox,i + Oy,surface,RGBA);
					drawPixel(i + Ox,-j + Oy,surface,RGBA);
					drawPixel(j + Ox,-i + Oy,surface,RGBA);
					drawPixel(-i + Ox,j + Oy,surface,RGBA);
					drawPixel(-j + Ox,i + Oy,surface,RGBA);
					drawPixel(-i + Ox,-j + Oy,surface,RGBA);
					drawPixel(j + Ox,i + Oy,surface,RGBA);
			}
			
	}
		
}

void drawPixel(int x, int y, SDL_Surface* surface, Uint32 RGBA){

		if(surface) {
				
				int width = surface->w;
				int height = surface->h;
				int pSize = surface->format->BytesPerPixel;
				
				if( width>=x || x>=0 )
					if( height>=y || y>=0 ) {
						
						unsigned char * pixels_ = (unsigned char*)(surface->pixels);
						*( pixels_ +width*pSize*y + x*pSize) = (RGBA & 0xff000000)>>24;
						*( pixels_ +width*pSize*y + x*pSize +1) = (RGBA & 0x00ff0000)>>16;
						*( pixels_ +width*pSize*y + x*pSize +2) = (RGBA & 0x0000ff00)>>8;
						*( pixels_ +width*pSize*y + x*pSize +3) = RGBA & 0x000000ff;
					}
		}
	
}

void initColor(SDL_Surface* surface, Uint32 RGBA){
	
	if(surface){
		unsigned char * pixels_ = (unsigned char*)(surface->pixels);
		int width = surface->w;
		int height = surface->h;
		int pSize = surface->format->BytesPerPixel;
		
		for(int j = 0; j<height; ++j)
			for(int i = 0; i<width; ++i)
			{
				*( pixels_ +width*pSize*j + i*pSize) = (RGBA & 0xff000000)>>24;
				*( pixels_ +width*pSize*j + i*pSize +1) = (RGBA & 0x00ff0000)>>16;
				*( pixels_ +width*pSize*j + i*pSize +2) = (RGBA & 0x0000ff00)>>8;
				*( pixels_ +width*pSize*j + i*pSize +3) = RGBA & 0x000000ff;
			}
			
	}
}

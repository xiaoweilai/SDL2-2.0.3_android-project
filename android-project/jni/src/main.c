#include <jni.h>
#include <string.h>
#include <stdio.h>
#include <android/log.h>
#include <android/bitmap.h>

#include "libavcodec/avcodec.h"
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswscale/swscale.h>
#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_events.h>
#include <SDL_main.h>

#define  LOG_TAG    "FFMPEGSample"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#define TARGET_WIDTH  (1366)
#define TARGET_HEIGHT (768)
#define PLAYVIDEONAME  "/mnt/sdcard/h264/test.mpg"
//#define PLAYVIDEONAME  "/mnt/sdcard/h264/box.mp4"



#define BMP_BACKGROUND  "/mnt/sdcard/h264/Background.bmp"
#define BMP_SHAPE       "/mnt/sdcard/h264/Blueshapes.bmp"



/*
 * the last screen size
 *
 */
#define COOLPAD5217 1
#define ANDROID_TINY210_800_480 2
#define ANDROID_TINY210_800_480_widthwrap 3
#define TESTID 4
#define DEVICEID TESTID


#if (DEVICEID == COOLPAD5217)
#define SCREENSHOWWIDTH   (480)
#define SCREENSHOWHEIGHT  (320)

#elif (DEVICEID == ANDROID_TINY210_800_480)
#define SCREENSHOWWIDTH   (800)
#define SCREENSHOWHEIGHT  (480)
#elif (DEVICEID == ANDROID_TINY210_800_480_widthwrap)
#define SCREENSHOWWIDTH   (800-30)
#define SCREENSHOWHEIGHT  (480)
#elif (DEVICEID == TESTID)
#define SCREENSHOWWIDTH   (TARGET_WIDTH)
#define SCREENSHOWHEIGHT  (TARGET_HEIGHT)
#else
#define SCREENSHOWWIDTH  (pCodecCtx->width/2)
#define SCREENSHOWHEIGHT  (pCodecCtx->height/2)
#endif



#include "SDL.h"

#if 0


#define BMP_BACKGROUND  "/mnt/sdcard/h264/Background.bmp"
#define BMP_SHAPE       "/mnt/sdcard/h264/Blueshapes.bmp"

/*
 * for test background color eg..
 *
 *
 * */
#include "SDL.h"
#define SHAPE_SIZE 200

int main(int argc, char *argv[])
{
	SDL_Window* Main_Window;
	SDL_Renderer* Main_Renderer;
	SDL_Surface* Loading_Surf;
	SDL_Texture* Background_Tx;
	SDL_Texture* BlueShapes;

	/* Rectangles for drawing which will specify source (inside the texture)
  and target (on the screen) for rendering our textures. */
	SDL_Rect SrcR;
	SDL_Rect DestR;

	SrcR.x = 0;
	SrcR.y = 0;
	SrcR.w = SHAPE_SIZE;
	SrcR.h = SHAPE_SIZE;

	DestR.x = 640 / 2 - SHAPE_SIZE / 2;
	DestR.y = 580 / 2 - SHAPE_SIZE / 2;
	DestR.w = SHAPE_SIZE;
	DestR.h = SHAPE_SIZE;


	/* Before we can render anything, we need a window and a renderer */
	Main_Window = SDL_CreateWindow("SDL_RenderCopy Example",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 580, 0);
	Main_Renderer = SDL_CreateRenderer(Main_Window, -1, SDL_RENDERER_ACCELERATED);

	/* The loading of the background texture. Since SDL_LoadBMP returns
  a surface, we convert it to a texture afterwards for fast accelerated
  blitting. */
	Loading_Surf = SDL_LoadBMP(BMP_BACKGROUND);
	Background_Tx = SDL_CreateTextureFromSurface(Main_Renderer, Loading_Surf);
	SDL_FreeSurface(Loading_Surf);  // we got the texture now -> free surface

	/* Load an additional texture */
	Loading_Surf = SDL_LoadBMP(BMP_SHAPE);
	BlueShapes = SDL_CreateTextureFromSurface(Main_Renderer, Loading_Surf);
	SDL_FreeSurface(Loading_Surf);

	/* now onto the fun part.
  This will render a rotating selection of the blue shapes
  in the middle of the screen */
	int i;
	int n;
	for (i = 0; i < 2; ++i) {
		for(n = 0; n < 4; ++n) {
			SrcR.x = SHAPE_SIZE * (n % 2);
			if (n > 1) {
				SrcR.y = SHAPE_SIZE;
			} else {
				SrcR.y = 0;
			}

			/* render background, whereas NULL for source and destination
      rectangles just means "use the default" */
			SDL_RenderCopy(Main_Renderer, Background_Tx, NULL, NULL);

			/* render the current animation step of our shape */
			SDL_RenderCopy(Main_Renderer, BlueShapes, &SrcR, &DestR);
			SDL_RenderPresent(Main_Renderer);
			SDL_Delay(50000);
		}
	}


	/* The renderer works pretty much like a big canvas:
  when you RenderCopy you are adding paint, each time adding it
  on top.
  You can change how it blends with the stuff that
  the new data goes over.
  When your 'picture' is complete, you show it
  by using SDL_RenderPresent. */

	/* SDL 1.2 hint: If you're stuck on the whole renderer idea coming
  from 1.2 surfaces and blitting, think of the renderer as your
  main surface, and SDL_RenderCopy as the blit function to that main
  surface, with SDL_RenderPresent as the old SDL_Flip function.*/

	SDL_DestroyTexture(BlueShapes);
	SDL_DestroyTexture(Background_Tx);
	SDL_DestroyRenderer(Main_Renderer);
	SDL_DestroyWindow(Main_Window);
	SDL_Quit();


	return 0;
}


#elif 1


/*
 * test change it
 * but color not good, is also ok
 *
 * */


/* Cheat to keep things simple and just use some globals. */
AVFormatContext *pFormatCtx;
AVCodecContext *pCodecCtx;
AVFrame *pFrame;
AVFrame *pFrameRGB;
AVFrame *pFrameYUV;
int videoStream;
int i, numBytes;


int main(int argc, char* argv[])
{
#if 1 //�̶���Ŀ¼
	//	LOGI("Screen->width-height:%d-%d\n", width,height);
	char *file_path = PLAYVIDEONAME;
	LOGI("file_path:%s", file_path);
#else
	char *file_path = argv[1];
	LOGI("file_path %s", file_path);
#endif

	//	AVFormatContext *pFormatCtx;
	//	AVCodecContext *pCodecCtx;
	AVCodec *pCodec;
	//	AVFrame *pFrame, *pFrameYUV;
	AVPacket *packet;
	uint8_t *out_buffer;

	SDL_Texture *tex   = NULL; /*Ӳ��������*/
	SDL_Window *window = NULL; /*����*/
//	SDL_GLContent maincontext;//���ǵĴ��ھ��
	SDL_Surface *bitmapSurface = NULL; /*����ɫ*/
	SDL_Rect rect;
	SDL_Event event;

	struct SwsContext *img_convert_ctx = NULL;

	//	int videoStream, i, numBytes;
	int ret, got_picture;

	av_register_all();
	pFormatCtx = avformat_alloc_context();
    if(NULL == pFormatCtx)
    {
    	LOGE("Couldn't open pFormatCtx. \n");
        return -1; // Couldn't open pFormatCtx
    }

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
    	LOGE("Could not initialize SDL - %s\n", SDL_GetError());
        exit(1);
    }
	if (av_open_input_file(&pFormatCtx, file_path,  NULL, 0, NULL) != 0) {
		LOGE("can't open the file. \n");
		return -1;
	}

	if (av_find_stream_info(pFormatCtx) < 0) {
		LOGE("Could't find stream infomation.\n");
		return -1;
	}


	videoStream = 1;
	for (i = 0; i < pFormatCtx->nb_streams; i++) {
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoStream = i;
		}
	}
	LOGI("videoStream:%d", videoStream);
	if (videoStream == -1) {
		LOGE("Didn't find a video stream.\n");
		return -1;
	}

	pCodecCtx = pFormatCtx->streams[videoStream]->codec;
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL) {
		LOGE("Codec not found.\n");
		return -1;
	}
	LOGI("pCodecCtx codec_id:%d", pCodecCtx->codec_id);

	if(avcodec_open(pCodecCtx, pCodec)<0) {
		LOGE("Unable to open codec.\n");
		return -1;
	}

	pFrame = avcodec_alloc_frame();
	pFrameYUV = avcodec_alloc_frame();

	//	LOGI("pCodecCtx width:%d", pCodecCtx->width); //1366
	//	LOGI("pCodecCtx height:%d", pCodecCtx->height); //768
	//

//	SDL_Window* window;
	SDL_Renderer* Main_Renderer;/*��Ⱦ��*/
	SDL_Texture* Background_Tx;/* ����ɫ */
	SDL_Surface* Loading_Surf;
	SDL_Texture* BlueShapes;

	//	SDL_Init(SDL_INIT_EVERYTHING);
	// Initialize SDL.
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		return 1;
	}
	LOGI("initialize SDL done!!. \n");

	//	/* Request opengl 3.2 context.
	//	 * SDL doesn't have the ablitity to choose
	//	 * which profile at this time of writing.
	//	 *but it should default to the core profile
	//	 */
	//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);



	/*�������ǻ��ƵĴ���ǰ�����������ƴ���λ�ã�Ȼ���Ǵ��ڴ�С ��Ȼ����λ�꣨falg��*/
	// Create the window where we will draw.
	window = SDL_CreateWindow(
			"SDL_RenderClear",         // window title
			SDL_WINDOWPOS_UNDEFINED,   // initial x position
			SDL_WINDOWPOS_UNDEFINED,   // initial y position
			SCREENSHOWWIDTH,                      // width, in pixels
			SCREENSHOWHEIGHT,                       // height, in pixels
			SDL_WINDOW_FULLSCREEN|SDL_WINDOW_OPENGL          // flags - see below
			);
//	Remarks
//
//	flags may be any of the following OR'd together:
//
//	SDL_WINDOW_FULLSCREEN
//	fullscreen window
//	SDL_WINDOW_FULLSCREEN_DESKTOP
//	fullscreen window at the current desktop resolution
//	SDL_WINDOW_OPENGL
//	window usable with OpenGL context
//	SDL_WINDOW_HIDDEN
//	window is not visible
//	SDL_WINDOW_BORDERLESS
//	no window decoration
//	SDL_WINDOW_RESIZABLE
//	window can be resized
//	SDL_WINDOW_MINIMIZED
//	window is minimized
//	SDL_WINDOW_MAXIMIZED
//	window is maximized
//	SDL_WINDOW_INPUT_GRABBED
//	window has grabbed input focus
//	SDL_WINDOW_ALLOW_HIGHDPI
//	window should be created in high-DPI mode if supported (>= SDL 2.0.1)
//	SDL_WINDOW_SHOWN is ignored by SDL_CreateWindow(). The SDL_Window is implicitly shown if SDL_WINDOW_HIDDEN is not set. SDL_WINDOW_SHOWN may be queried later using SDL_GetWindowFlags().

	// Check that the window was successfully made
	if (window == NULL) {
		// In the event that the window could not be made...
		printf("Could not create window: %s\n", SDL_GetError());
		return 1;
	}




	// We must call SDL_CreateRenderer in order for draw calls to affect this window.
//	renderer = SDL_CreateRenderer(window, -1, 0);
	/*��Ⱦ�� ��һ��λ���ʾ�Կ� -1��ʾ���Կ�����һ�� �ڶ�����ʾ����ʾ����ˢ������ˢ�»���*/
	Main_Renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
			|SDL_RENDERER_TARGETTEXTURE);


	//	/* The loading of the background texture. Since SDL_LoadBMP returns
	//	  a surface, we convert it to a texture afterwards for fast accelerated
	//	  blitting. */
	//	Loading_Surf = SDL_LoadBMP(BMP_BACKGROUND);
	////	SDL_SetRenderDrawColor(Main_Renderer, 255, 0, 0, 255);
	//	Background_Tx = SDL_CreateTextureFromSurface(Main_Renderer, Loading_Surf);
	//	SDL_FreeSurface(Loading_Surf);  // we got the texture now -> free surface


	//	BlueShapes = SDL_CreateTextureFromSurface(Main_Renderer, bitmapSurface);

	BlueShapes = SDL_CreateTexture(Main_Renderer, SDL_PIXELFORMAT_YV12,
			SDL_TEXTUREACCESS_STREAMING, pCodecCtx->width, pCodecCtx->height); /*ʹ��Ӳ������*/
	//-------------------------------------------------------------//
	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
			pCodecCtx->pix_fmt, SCREENSHOWWIDTH, SCREENSHOWHEIGHT,
			PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

	numBytes = avpicture_get_size(PIX_FMT_YUV420P, pCodecCtx->width,
			pCodecCtx->height);
	out_buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
	avpicture_fill((AVPicture *) pFrameYUV, out_buffer, PIX_FMT_YUV420P,
			pCodecCtx->width, pCodecCtx->height);

#if 1
	rect.x = 0;
	rect.y = 0;
	rect.w = pCodecCtx->width;
	rect.h = pCodecCtx->height;
#else
	rect.x = 0;
	rect.y = 0;
	rect.w = SCREENSHOWWIDTH*2*2;
	rect.h = SCREENSHOWHEIGHT*2*2;
#endif

	int y_size = pCodecCtx->width * pCodecCtx->height;
	packet = (AVPacket *) malloc(sizeof(AVPacket));
	av_new_packet(packet, y_size);

	//	av_dump_format(pFormatCtx, 0, file_path, 0);

	// Select the color for drawing. It is set to red here.
	//	SDL_SetRenderDrawColor(Main_Renderer, 255, 255, 255, 255);
	//	SDL_Rect rectangle;
	//
	//	rectangle.x = 0;
	//	rectangle.y = 0;
	//	rectangle.w = 50;
	//	rectangle.h = 50;
	//	SDL_RenderFillRect(Main_Renderer, &rectangle);
	// Clear the entire screen to our selected color.
	//	SDL_RenderClear(Main_Renderer);

	// Up until now everything was drawn behind the scenes.
	// This will show the new, red contents of the window.
	//	SDL_RenderPresent(Main_Renderer);
	// Give us time to see the window.
	//	SDL_Delay(5000);


	while (av_read_frame(pFormatCtx, packet) >= 0) {
		if (packet->stream_index == videoStream) {
			got_picture = -1;
			ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture,
					packet);
			if (ret < 0) {
				LOGE("decode error.\n");
				return -1;
			}

			LOGI("got_picture:%d", got_picture);
			if (got_picture) {
				sws_scale(img_convert_ctx,
						(uint8_t const * const *) pFrame->data,
						pFrame->linesize, 0, pCodecCtx->height, pFrameYUV->data,
						pFrameYUV->linesize);
				//				SDL_LockTexture(BlueShapes, &rect, pFrameYUV->data[0], pFrameYUV->linesize[0]);
				////iPitch ����yuvһ������ռ���ֽ���
#if 1
				SDL_UpdateTexture(BlueShapes, &rect, pFrameYUV->data[0], pFrameYUV->linesize[0]); /*�������������*/
				SDL_RenderClear(Main_Renderer);  /*�����Ļ*/
				/* render background, whereas NULL for source and destination
			      rectangles just means "use the default" */
				//				SDL_RenderCopy(Main_Renderer, Background_Tx, NULL, NULL);

				/* render the current animation step of our shape */
				//				SDL_RenderCopy(Main_Renderer, BlueShapes, &SrcR, &DestR);
//				SDL_RenderCopy(Main_Renderer, BlueShapes, &rect, &rect); /*�����Ƹ���Ⱦ��,��ʵ�ʷֱ�����ʾ����ʾ��ȫ*/
				SDL_RenderCopy(Main_Renderer, BlueShapes, NULL, NULL); /*�����Ƹ���Ⱦ����������Ļ��ȫ��*/
				SDL_RenderPresent(Main_Renderer); /*������Ļ ��ʾ */
#endif
#if 0
				/* ����ɫ�����Կ�����ӰƬ�����⣬�ٲ���������Ƶ���� */
				SDL_Delay(1000);
				SDL_RenderClear(Main_Renderer);  /*�����Ļ*/
				SDL_SetRenderDrawColor(Main_Renderer, 255, 255, 255, 255);
				SDL_RenderPresent(Main_Renderer); /*������Ļ ��ʾ */
				//				SDL_Delay(4);
#endif

				//				SDL_UnlockTexture(BlueShapes);//������
			}

			SDL_Delay(40);
		}

		av_free_packet(packet);

		SDL_PollEvent(&event);
		switch (event.type) {
		case SDL_QUIT:
			SDL_Quit();
			exit(0);
			break;
		default:
			break;
		}
	}

#if 1
	// Select the color for drawing. It is set to red here.
	//	SDL_SetRenderDrawColor(Main_Renderer, 100, 200, 100, 255);

	// Clear the entire screen to our selected color.
	//	SDL_RenderClear(Main_Renderer);

	// Up until now everything was drawn behind the scenes.
	// This will show the new, red contents of the window.
	//	SDL_RenderPresent(Main_Renderer);

	// Give us time to see the window.
	SDL_Delay(5000);

	//	// Always be sure to clean up
	//	SDL_Quit();
#endif
	// Close and destroy the window
	SDL_DestroyWindow(window);
	SDL_DestroyTexture(tex); /*�ͷ��ڴ�*/
	// Always be sure to clean up
	SDL_Quit();


	av_free(out_buffer);
	av_free(pFrameYUV);
	avcodec_close(pCodecCtx);
	av_close_input_file(pFormatCtx);
	return 0;
}














#elif 1

/*
 * test change it
 * but color not good, is also ok
 *
 * */


/* Cheat to keep things simple and just use some globals. */
AVFormatContext *pFormatCtx;
AVCodecContext *pCodecCtx;
AVFrame *pFrame;
AVFrame *pFrameRGB;
AVFrame *pFrameYUV;
int videoStream;
int i, numBytes;


int main(int argc, char* argv[])  
{  

	//	LOGI("Screen->width-height:%d-%d\n", width,height);
	char *file_path = PLAYVIDEONAME;
	LOGI("file_path:%s", file_path);


	//	AVFormatContext *pFormatCtx;
	//	AVCodecContext *pCodecCtx;
	AVCodec *pCodec;
	//	AVFrame *pFrame, *pFrameYUV;
	AVPacket *packet;
	uint8_t *out_buffer;

	SDL_Texture *tex = NULL;
	SDL_Window *screen = NULL;
	SDL_Rect rect;
	SDL_Event event;

	static struct SwsContext *img_convert_ctx;

	//	int videoStream, i, numBytes;
	int ret, got_picture;

	av_register_all();
	pFormatCtx = avformat_alloc_context();

	if (av_open_input_file(&pFormatCtx, file_path,  NULL, 0, NULL) != 0) {
		LOGE("can't open the file. \n");
		return -1;
	}

	if (av_find_stream_info(pFormatCtx) < 0) {
		LOGE("Could't find stream infomation.\n");
		return -1;
	}


	videoStream = 1;
	for (i = 0; i < pFormatCtx->nb_streams; i++) {
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoStream = i;
		}
	}
	LOGI("videoStream:%d", videoStream);
	if (videoStream == -1) {
		LOGE("Didn't find a video stream.\n");
		return -1;
	}

	pCodecCtx = pFormatCtx->streams[videoStream]->codec;
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL) {
		LOGE("Codec not found.\n");
		return -1;
	}
	LOGI("pCodecCtx codec_id:%d", pCodecCtx->codec_id);

	if(avcodec_open(pCodecCtx, pCodec)<0) {
		LOGE("Unable to open codec.\n");
		return -1;
	}

	pFrame = avcodec_alloc_frame();
	pFrameYUV = avcodec_alloc_frame();

	//	LOGI("pCodecCtx width:%d", pCodecCtx->width); //1366
	//	LOGI("pCodecCtx height:%d", pCodecCtx->height); //768
	//

	//	SDL_Window* window;
	SDL_Renderer* renderer;

	// Initialize SDL.
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		return 1;
	}
	LOGI("initialize SDL done!!. \n");

	// Create the window where we will draw.
	screen = SDL_CreateWindow(
			"SDL_RenderClear",         // window title
			SDL_WINDOWPOS_UNDEFINED,   // initial x position
			SDL_WINDOWPOS_UNDEFINED,   // initial y position
			SCREENSHOWWIDTH,                      // width, in pixels
			SCREENSHOWHEIGHT,                       // height, in pixels
			SDL_WINDOW_FULLSCREEN|SDL_WINDOW_OPENGL          // flags - see below
	);
	//	Remarks
	//
	//	flags may be any of the following OR'd together:
	//
	//	SDL_WINDOW_FULLSCREEN
	//	fullscreen window
	//	SDL_WINDOW_FULLSCREEN_DESKTOP
	//	fullscreen window at the current desktop resolution
	//	SDL_WINDOW_OPENGL
	//	window usable with OpenGL context
	//	SDL_WINDOW_HIDDEN
	//	window is not visible
	//	SDL_WINDOW_BORDERLESS
	//	no window decoration
	//	SDL_WINDOW_RESIZABLE
	//	window can be resized
	//	SDL_WINDOW_MINIMIZED
	//	window is minimized
	//	SDL_WINDOW_MAXIMIZED
	//	window is maximized
	//	SDL_WINDOW_INPUT_GRABBED
	//	window has grabbed input focus
	//	SDL_WINDOW_ALLOW_HIGHDPI
	//	window should be created in high-DPI mode if supported (>= SDL 2.0.1)
	//	SDL_WINDOW_SHOWN is ignored by SDL_CreateWindow(). The SDL_Window is implicitly shown if SDL_WINDOW_HIDDEN is not set. SDL_WINDOW_SHOWN may be queried later using SDL_GetWindowFlags().

	// Check that the window was successfully made
	if (screen == NULL) {
		// In the event that the window could not be made...
		printf("Could not create window: %s\n", SDL_GetError());
		return 1;
	}
	// We must call SDL_CreateRenderer in order for draw calls to affect this window.
	//	renderer = SDL_CreateRenderer(screen, -1, 0);
	renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);


	tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_YV12,
			SDL_TEXTUREACCESS_STREAMING, pCodecCtx->width, pCodecCtx->height);
	//-------------------------------------------------------------//
	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
			pCodecCtx->pix_fmt, SCREENSHOWWIDTH, SCREENSHOWHEIGHT,
			PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

	numBytes = avpicture_get_size(PIX_FMT_YUV420P, pCodecCtx->width,
			pCodecCtx->height);
	out_buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
	avpicture_fill((AVPicture *) pFrameYUV, out_buffer, PIX_FMT_YUV420P,
			pCodecCtx->width, pCodecCtx->height);


	rect.x = 0;
	rect.y = 0;
	rect.w = pCodecCtx->width;
	rect.h = pCodecCtx->height;

	int y_size = pCodecCtx->width * pCodecCtx->height;
	packet = (AVPacket *) malloc(sizeof(AVPacket));
	av_new_packet(packet, y_size);

	//	av_dump_format(pFormatCtx, 0, file_path, 0);


	while (av_read_frame(pFormatCtx, packet) >= 0) {
		if (packet->stream_index == videoStream) {
			got_picture = -1;
			ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture,
					packet);
			if (ret < 0) {
				LOGE("decode error.\n");
				return -1;
			}

			LOGI("got_picture:%d", got_picture);
			if (got_picture) {
				sws_scale(img_convert_ctx,
						(uint8_t const * const *) pFrame->data,
						pFrame->linesize, 0, pCodecCtx->height, pFrameYUV->data,
						pFrameYUV->linesize);
				////iPitch ����yuvһ������ռ���ֽ���
				SDL_UpdateTexture(tex, &rect, pFrameYUV->data[0], pFrameYUV->linesize[0]);
				SDL_RenderClear(renderer);
				//				SDL_BlitSurface(renderer, tex, screen, &rect);
				//				SDL_Flip(screen);
				SDL_RenderCopy(renderer, tex, &rect, &rect);
				SDL_RenderPresent(renderer);
			}

			SDL_Delay(40);
		}

		av_free_packet(packet);

		SDL_PollEvent(&event);
		switch (event.type) {
		case SDL_QUIT:
			SDL_Quit();
			exit(0);
			break;
		default:
			break;
		}
	}

#if 1
	// Select the color for drawing. It is set to red here.
	SDL_SetRenderDrawColor(renderer, 100, 200, 100, 255);

	// Clear the entire screen to our selected color.
	SDL_RenderClear(renderer);

	// Up until now everything was drawn behind the scenes.
	// This will show the new, red contents of the window.
	SDL_RenderPresent(renderer);

	// Give us time to see the window.
	//	SDL_Delay(5000);

	//	// Always be sure to clean up
	//	SDL_Quit();
#endif
	// Close and destroy the window
	SDL_DestroyWindow(screen);
	SDL_DestroyTexture(tex);
	// Always be sure to clean up
	SDL_Quit();


	av_free(out_buffer);
	av_free(pFrameYUV);
	avcodec_close(pCodecCtx);
	av_close_input_file(pFormatCtx);
	return 0;
}  
#else


/*
 * org main,good
 * */
int main(int argc, char* argv[])
{
	SDL_Window* window;
	SDL_Renderer* renderer;

	// Initialize SDL.
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return 1;

	// Create the window where we will draw.
	window = SDL_CreateWindow("SDL_RenderClear",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			512, 512,
			SDL_WINDOW_SHOWN);

	// We must call SDL_CreateRenderer in order for draw calls to affect this window.
	renderer = SDL_CreateRenderer(window, -1, 0);

	// Select the color for drawing. It is set to red here.
	SDL_SetRenderDrawColor(renderer, 100, 200, 100, 255);

	// Clear the entire screen to our selected color.
	SDL_RenderClear(renderer);

	// Up until now everything was drawn behind the scenes.
	// This will show the new, red contents of the window.
	SDL_RenderPresent(renderer);

	// Give us time to see the window.
	SDL_Delay(5000);

	// Always be sure to clean up
	SDL_Quit();
	return 0;
}

#endif

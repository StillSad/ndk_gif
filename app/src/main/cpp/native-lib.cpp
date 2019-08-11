#include <jni.h>
#include <string>
#include "gif_lib.h"


#define argb(a, r, g, b) ( ((a) & 0xff) << 24 ) | ( ((b) & 0xff) << 16 ) | ( ((g) & 0xff) << 8 ) | ((r) & 0xff)
typedef struct GifBean{
    int current_frame;
    int total_frames;
    int *delays;
}GifBean;


void drawFrame(GifFileType *pType, GifBean *pBean, AndroidBitmapInfo info, void **pVoid);

extern "C"
JNIEXPORT jlong JNICALL
Java_com_ice_ndkgif_GifNDKDecoder_loadGifNative(JNIEnv *env, jclass type, jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);
    int error;
    GifFileType *gifFileType = DGifOpenFileName(path,&error);

    DGifSlurp(gifFileType);

    GifBean *gifBean = (GifBean *)(malloc(sizeof(GifBean)));
    memset(gifBean,0, sizeof(GifBean));

    gifBean->delays = (int *)(malloc(sizeof(int) * gifFileType->ImageCount));
    memset(gifBean->delays,0, sizeof(int) * gifFileType->ImageCount);

    ExtensionBlock *ext;

    for (int i = 0; i < gifFileType->ImageCount; ++i) {
        SavedImage frame = gifFileType->SavedImages[i];
        for (int j = 0; j < frame.ExtensionBlockCount; ++j) {
            if (frame.ExtensionBlocks[j].Function == GRAPHICS_EXT_FUNC_CODE) {
                ext = &frame.ExtensionBlocks[j];
                break;
            }
        }
        if (ext) {
            int frame_delay = (ext->Bytes[2] << 8 | ext->Bytes[1]) * 10;
            gifBean->delays[i] = frame_delay;
        }
    }



    gifBean->total_frames = gifFileType->ImageCount;
    gifFileType->UserData = gifBean;
    env->ReleaseStringUTFChars(path_, path);
    return (jlong)(gifFileType);
}




extern "C" JNIEXPORT jstring JNICALL
Java_com_ice_ndkgif_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

void drawFrame(GifFileType *gifFileType, GifBean *gifBean, AndroidBitmapInfo info, void *pixels) {
    SavedImage savedImage = gifFileType->SavedImages[gifBean->current_frame];

    GifImageDesc imageInfo = savedImage.ImageDesc;

    int *px = (int *)pixels;

    ColorMapObject *colorMapObject= imageInfo.ColorMap;
    if (colorMapObject == NULL) {
        colorMapObject = gifFileType->SColorMap;
    }

    px = (int *)((char *) px + info.stride *  imageInfo.Top);
    int pointPixel;
    GifByteType  gifByteType;
    GifColorType gifColorType;
    int *line;
    for (int y = imageInfo.Top; y < imageInfo.Top + imageInfo.Height; ++y) {
        line = px;
        for (int x = imageInfo.Left; x < imageInfo.Left + imageInfo.Width; ++x) {
            pointPixel = (y - imageInfo.Top) * imageInfo.Width + (x - imageInfo.Left);
            gifByteType = savedImage.RasterBits[pointPixel];
            if (colorMapObject != NULL) {
                gifColorType = colorMapObject->Colors[gifByteType];
                line[x] = argb(255,gifColorType.Red,gifColorType.Green,gifColorType.Blue);
            }
        }
        px = (int *)((char *)px + info.stride);
    }
}



extern "C"
JNIEXPORT jint JNICALL
Java_com_ice_ndkgif_GifNDKDecoder_getWidth(JNIEnv *env, jclass type, jlong gifPointer) {
    GifFileType *gifFileType = (GifFileType *)(gifPointer);
    return gifFileType->SWidth;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_ice_ndkgif_GifNDKDecoder_getHeight(JNIEnv *env, jclass type, jlong gifPointer) {
    GifFileType *gifFileType = (GifFileType *)(gifPointer);
    return gifFileType->SHeight;

}

extern "C"
JNIEXPORT jint JNICALL
Java_com_ice_ndkgif_GifNDKDecoder_updateFrame(JNIEnv *env, jclass type, jobject bitmap,
                                              jlong gifPointer) {

    GifFileType *gifFileType = (GifFileType *)gifPointer;
    GifBean *gifBean = (GifBean *)gifFileType->UserData;
    AndroidBitmapInfo info;
    AndroidBitmap_getInfo(env,bitmap,&info);

    void *pixels;

    AndroidBitmap_lockPixels(env,bitmap,&pixels);
    drawFrame(gifFileType,gifBean,info,pixels);
    gifBean->current_frame +=1;
    if (gifBean->current_frame >= gifBean->total_frames) {
        gifBean->current_frame = 0;
    }
    AndroidBitmap_unlockPixels(env,bitmap);
    return gifBean->delays[gifBean->current_frame];
}
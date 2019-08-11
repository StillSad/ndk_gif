package com.ice.ndkgif;

import android.graphics.Bitmap;

public class GifNDKDecoder {
    private long gifPointer;

    static {
        System.loadLibrary("native-lib");
    }

    public GifNDKDecoder(long gifPointer){
        this.gifPointer = gifPointer;
    }

    public long getGifPointer() {
        return gifPointer;
    }

    public static GifNDKDecoder load(String path){
        long gifHander = loadGifNative(path);
        GifNDKDecoder gifNDKDecoder = new GifNDKDecoder(gifHander);
        return gifNDKDecoder;
    }


    private static native long loadGifNative(String path);

    public static native int getWidth(long gifPointer);
    public static native int getHeight(long gifPointer);

    public static native int updateFrame(Bitmap bitmap,long gifPointer);
}

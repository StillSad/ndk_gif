package com.ice.ndkgif;

import androidx.appcompat.app.AppCompatActivity;

import android.graphics.Bitmap;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    private ImageView ivGif;
    private GifNDKDecoder gifNDKDecoder;
    private Bitmap bitmap;

    Handler mHandler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            int nextDelayTime = gifNDKDecoder.updateFrame(bitmap,gifNDKDecoder.getGifPointer());
            mHandler.sendEmptyMessageDelayed(1,nextDelayTime);
            ivGif.setImageBitmap(bitmap);
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        ivGif = findViewById(R.id.iv_gif);

    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    public void ndkLoadGif(View view) {
        File file = new File(Environment.getExternalStorageDirectory(),"demo2.gif");
        gifNDKDecoder = GifNDKDecoder.load(file.getAbsolutePath());
        int width = GifNDKDecoder.getWidth(gifNDKDecoder.getGifPointer());
        int height = GifNDKDecoder.getHeight(gifNDKDecoder.getGifPointer());
        bitmap = Bitmap.createBitmap(width,height,Bitmap.Config.ARGB_8888);
        int nextDelayTime = gifNDKDecoder.updateFrame(bitmap,gifNDKDecoder.getGifPointer());
        mHandler.sendEmptyMessageDelayed(1,nextDelayTime);
    }


}

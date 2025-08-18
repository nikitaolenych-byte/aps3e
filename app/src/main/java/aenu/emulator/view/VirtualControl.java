// SPDX-License-Identifier: WTFPL
package aenu.emulator.view;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.util.DisplayMetrics;
import android.view.MotionEvent;
import android.view.View;

public class VirtualControl extends android.view.SurfaceView{

    interface OnKeyEventListener{
        public void onKeyEvent(int key_code,boolean pressed);
    }

    interface Component{

        void set_scale(float scale);
        void set_opacity(float opacity);
        void draw(Canvas canvas);
    }

    static Bitmap create_ratio_bitmap(Context context,int res_id,float ratio){
        DisplayMetrics dm=context.getResources().getDisplayMetrics();
        int min_dimension=Math.min(dm.widthPixels,dm.heightPixels);
        return Bitmap.createScaledBitmap(BitmapFactory.decodeResource(context.getResources(),res_id),
                (int)(min_dimension*ratio),
                (int)(min_dimension*ratio),
                true);
    }

    class Dpad implements Component {

        int x;
        int y;

        int res_id;
        int pressed_res_id;

        float scale=1.0f;
        float opacity=1.0f;

        Bitmap default_bitmap;
        Bitmap default_pressed_bitmap;
        Bitmap bitmap;
        Bitmap pressed_bitmap;

        Context context;

        public Dpad(Context context,int res_id,int pressed_res_id){
            this.context=context;
            this.res_id=res_id;
            this.pressed_res_id=pressed_res_id;
            final float ratio=0.35f;
            default_bitmap=VirtualControl.create_ratio_bitmap(context,res_id,ratio);
            default_pressed_bitmap=VirtualControl.create_ratio_bitmap(context,pressed_res_id,ratio);
        }

        void setup_bitmap(){

        }

        @Override
        public void set_scale(float scale) {

        }

        @Override
        public void set_opacity(float opacity) {

        }

        @Override
        public void draw(Canvas canvas) {

        }
    }

    class ᐃ口OX{

    }

    public VirtualControl(Context context) {
        this(context,null);
    }
    public VirtualControl(Context context, android.util.AttributeSet attrs) {
        super(context, attrs);
        setWillNotDraw(false);
        requestFocus();
    }

    @Override
    public void draw(Canvas canvas) {
        super.draw(canvas);
    }

    public void onTouch(MotionEvent event) {
    }
}

package test.cam;

import java.io.IOException;

import opengl.jni.Natives;
import opengl.scenes.GLThread;
import opengl.scenes.Renderer;

import android.content.Context;
import android.hardware.Camera;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class GLSurface extends SurfaceView implements SurfaceHolder.Callback {
	
	private static final String TAG = "Preview";

	private SurfaceHolder mHolder;
    private GLThread mGLThread;

	
	public GLSurface(Context context) {
		super(context);
		// Install a SurfaceHolder.Callback so we get notified when the
		// underlying surface is created and destroyed.
		mHolder = getHolder();
		mHolder.addCallback(this);
		mHolder.setType(SurfaceHolder.SURFACE_TYPE_GPU);
	}

    public void setRenderer(Renderer renderer) {
        mGLThread = new GLThread(renderer, mHolder);
        mGLThread.start();
        
        Log.d(TAG, "GLSurfaceView::setRenderer setting natives listener");
        Natives.setListener(mGLThread);
    }
	
	
	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
		// 4 = RGB_565 
		Log.d(TAG,"surfaceChanged Surface fomat=" + format + " w=" + w + " h=" + h);
		
	
		mGLThread.onWindowResize(w, h);
	}

	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		Log.d(TAG, "surfaceCreated");
		
		mGLThread.surfaceCreated();
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		mGLThread.surfaceDestroyed();
	}
	
    /**
     * Inform the view that the activity is paused.
     */
    public void onPause() {
        mGLThread.onPause();
    }

    /**
     * Inform the view that the activity is resumed.
     */
    public void onResume() {
        mGLThread.onResume();
    }
	
    /**
     * Inform the view that the window focus has changed.
     */
    @Override 
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        mGLThread.onWindowFocusChanged(hasFocus);
    }
	

}

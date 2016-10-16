package test.cam;

import java.io.IOException;


import android.content.Context;
import android.hardware.Camera;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class CameraSurface extends SurfaceView implements SurfaceHolder.Callback {
	
	private static final String TAG = "Preview";

	private SurfaceHolder mHolder;

	public Camera camera;
	
	public CameraSurface(Context context) {
		super(context);
		// Install a SurfaceHolder.Callback so we get notified when the
		// underlying surface is created and destroyed.
		mHolder = getHolder();
		mHolder.addCallback(this);
		mHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);	
	}

	
	
	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
		// 4 = RGB_565 
		Log.d(TAG,"surfaceChanged Surface fomat=" + format + " w=" + w + " h=" + h);
		
		// Now that the size is known, set up the camera parameters and begin
		// the preview.
		Camera.Parameters parameters = camera.getParameters();
		
		Log.d(TAG,"surfaceChanged Preview size: " + parameters.getPreviewSize());
		
		// (17)- YCbCr_420_SP (NV21)
		Log.d(TAG,"surfaceChanged Preview format: " + parameters.getPreviewFormat());
		
		camera.startPreview();
	}

	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		Log.d(TAG, "surfaceCreated");
		
		camera = Camera.open();
		try {
			camera.setPreviewDisplay(holder);
			
			camera.setPreviewCallback(new Camera.PreviewCallback() {
				public void onPreviewFrame(byte[] data, Camera cam) {
					// Here we have a frame image in NV21 format
				}
			});
		} catch (IOException e) {
			e.printStackTrace();
		}

	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		// Surface will be destroyed when we return, so stop the preview.
		// Because the CameraDevice object is not a shared resource, it's very
		// important to release it when the activity is paused.
		Log.d(TAG, "surfaceCreated Stop camera");
		
		camera.stopPreview();
		camera = null;
			
	}
	
    /**
     * Inform the view that the activity is paused.
     */
    public void onPause() {
    }

    /**
     * Inform the view that the activity is resumed.
     */
    public void onResume() {
    }
	
    /**
     * Inform the view that the window focus has changed.
     */
    @Override 
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
    }
	

}

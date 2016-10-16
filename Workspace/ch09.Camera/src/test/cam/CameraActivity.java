package test.cam;

import opengl.scenes.cubes.CubeRenderer;
import android.app.Activity;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;
import android.widget.FrameLayout;

public class CameraActivity extends Activity {
	private static final String TAG = "CameraDemo";
	
	//Camera camera;
	
	CameraSurface mCameraSurface;
	GLSurface mGLSurface;
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        mCameraSurface = new CameraSurface(this);
        mGLSurface = new GLSurface(this);
        
        mGLSurface.setRenderer(new CubeRenderer(true, false));
        
//        mCameraSurface = new CameraSurfaceGL(this);
//        mCameraSurface.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
        
        FrameLayout preview = ((FrameLayout) findViewById(R.id.preview));
        
        preview.addView(mGLSurface);
        preview.addView(mCameraSurface);
        
        //setContentView(mCameraSurface);
        
        Log.d(TAG, "onCreate'd");
    }
    
    @Override
    protected void onResume() {
        // Ideally a game should implement onResume() and onPause()
        // to take appropriate action when the activity looses focus
        super.onResume();
        mCameraSurface.onResume();
        mGLSurface.onResume();
    }

    @Override
    protected void onPause() {
        // Ideally a game should implement onResume() and onPause()
        // to take appropriate action when the activity looses focus
        super.onPause();
        mCameraSurface.onPause();
        mGLSurface.onPause();
    }
    
}
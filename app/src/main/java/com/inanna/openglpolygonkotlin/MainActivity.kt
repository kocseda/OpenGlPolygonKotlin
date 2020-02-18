package com.inanna.openglpolygonkotlin

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import kotlinx.android.synthetic.main.activity_main.*
import android.opengl.GLSurfaceView
import java.util.Random
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10
import android.app.Activity

class MainActivity : AppCompatActivity() {

    override fun onCreate(icicle: Bundle?) {
        super.onCreate(icicle)
        createViewAndRender()
    }

    //create an opengl surface view and render through c++ class.
    protected fun createViewAndRender() {
        pushToVertices()
        var mView: GLSurfaceView = GLSurfaceView(getApplication())
        mView.setEGLContextClientVersion(2)
        mView.setRenderer(object: GLSurfaceView.Renderer {
            override fun onSurfaceCreated(gl: GL10, config: EGLConfig) {
                PolygonJNI().onSurfaceCreated()
            }

            override fun onSurfaceChanged(gl: GL10, width: Int, height: Int) {
                PolygonJNI().onSurfaceChanged()
            }

            //draw polygon with new color params every 10 sec.
            override fun onDrawFrame(gl: GL10) {
                val colorRgb = getRandomRgb()
                PolygonJNI().onDrawFrame(colorRgb[0], colorRgb[1], colorRgb[2])
            }
        })
        setContentView(mView)
    }

    //push at least three vertices to create at least one triangle.
    protected fun pushToVertices() {
        PolygonJNI().pushToVertices(-1.0f, 0.0f)
        PolygonJNI().pushToVertices(-1.0f, -1.0f)
        PolygonJNI().pushToVertices(1.0f, -1.0f)
        PolygonJNI().pushToVertices(1.0f, 0.0f)
        PolygonJNI().pushToVertices(1.0f, 1.0f)
    }

    //create random color within 0-1 range.
    protected fun getRandomRgb(): FloatArray {
        val rnd = Random()
        val colorRed = rnd.nextFloat()
        val colorGreen = rnd.nextFloat()
        val colorBlue = rnd.nextFloat()
        return floatArrayOf(colorRed, colorGreen, colorBlue)
    }
}
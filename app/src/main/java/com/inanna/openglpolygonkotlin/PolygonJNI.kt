package com.inanna.openglpolygonkotlin

class PolygonJNI {
    init {
        System.loadLibrary("polygon")
    }

    external fun onSurfaceCreated()
    external fun onSurfaceChanged()
    external fun onDrawFrame(red: Float, green: Float, blue: Float)
    external fun pushToVertices(x: Float, y: Float)
}

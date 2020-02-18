
#include <jni.h>
#include <list>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
#include "Includes/triangulate.h"
#include "Includes/esUtil.h"
vector<Vector2d> vector1_list;
vector<Vector2d> vector2_list;
vector<Vector2d> vector3_list;
Vector2dVector vector_list;
GLuint mProgramObject;
int mWidth;
int mHeight;

string fShaderStr =
        "precision mediump float; \n"
        "void main() \n"
        "{ \\n"
        " gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); \n"
        "} \n";
typedef struct
{
    GLuint programObject;
} UserData;

//creates shader with given vertex shader and fragment shader sources.
GLuint loadShader(const char *shaderSrc, GLuint type) {
    GLuint shader;
    int* compiled = new int[1];
    shader = glCreateShader(type);
    if (shader == 0)
        return 0;

    glShaderSource(shader, 1, &shaderSrc, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, compiled);
    if(compiled[0] == 0)
    {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

        if(infoLen > 1)
        {
            cout<<"Error compiling shader";
        }
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

//with respect to color params from Android, a fragment shader string is generated.
void generateFragmentShader(float colorRed, float colorGreen, float colorBlue) {
    string beginning =  "precision mediump float; \n void main() \n { \n gl_FragColor = vec4(";
    string red = to_string(colorRed);
    string green = to_string(colorGreen);
    string blue = to_string(colorBlue);
    string remainder = ",1.0); \n} \n";
    string fragmentShaderString = beginning + red + ',' + green + ',' + blue + remainder;
    fShaderStr = fragmentShaderString;
}

//creates a surface with respect to vertex shader and fragment shader. fragment shader content changes with new colors.
void createSurface() {
    char vShaderStr[] =
            "attribute vec4 vPosition; \n"
            "void main() \n"
            "{ \n"
            " gl_Position = vPosition; \n"
            "} \n";
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint programObject;
    GLint linked;
    vertexShader = loadShader(vShaderStr, 0x8B31);
    char fShader[fShaderStr.length() + 1];
    strcpy(fShader, fShaderStr.c_str());
    fragmentShader = loadShader(fShader, 0x8B30);
    // Create the program object
    programObject = glCreateProgram();
    if(programObject == 0)
        return;
    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);
    glBindAttribLocation(programObject, 0, "vPosition");
    glLinkProgram(programObject);
    glGetProgramiv(programObject, 0x8B82, &linked);
    if(!linked)
    {
        GLint infoLen = 0;
        glGetProgramiv(programObject, 0x8B84, &infoLen);

        if(infoLen > 1)
        {
            cout<<("Error compiling shader:\n");
        }
        glDeleteProgram(programObject);
        return;
    }
    mProgramObject = programObject;
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

// draws resulted triangles.
void draw()
{
    createSurface();
    glViewport(mWidth/3, mHeight/3, mWidth, mHeight);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(mProgramObject);
    int vector_size = vector1_list.size();
    for (int y = 0; y < vector_size; y++) {
        GLfloat vVertices[] = { vector1_list.at(y).GetX(), vector1_list.at(y).GetY(), 0.0f,
                                vector2_list.at(y).GetX(), vector2_list.at(y).GetY(), 0.0f,
                                vector3_list.at(y).GetX(), vector3_list.at(y).GetY(), 0.0f };
        glVertexAttribPointer(0, 3, 0x1406, 0, 0, vVertices);
        glEnableVertexAttribArray(0);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
}

void changeSurface(int width, int height) {
    mWidth = width;
    mHeight = height;
}

//used for pushing the points from Android side to a vector.
void push_to_vectorlist(float pointX, float pointY) {
    vector_list.push_back(Vector2d(pointX, pointY));
}

//triangulates the shape that will occur with points. resulted triangles points are puched to three vectors.
void triangulate() {
    Vector2dVector result;
    Triangulate::Process(vector_list, result);
    int tcount = result.size() / 3;
    for (int i = 0; i < tcount; i++)
    {
        const Vector2d& p1 = result[i * 3 + 0];
        const Vector2d& p2 = result[i * 3 + 1];
        const Vector2d& p3 = result[i * 3 + 2];
        printf("Triangle %d => (%0.0f,%0.0f) (%0.0f,%0.0f) (%0.0f,%0.0f)\n", i + 1, p1.GetX(), p1.GetY(), p2.GetX(), p2.GetY(), p3.GetX(), p3.GetY());
        vector1_list.push_back(p1);
        vector2_list.push_back(p2);
        vector3_list.push_back(p3);
    }
}

extern "C" {
JNIEXPORT void JNICALL
    Java_com_inanna_openglpolygonkotlin_PolygonJNI_onSurfaceCreated(JNIEnv *env, jclass type) {
        (void) type;
        triangulate();
        createSurface();
    }
}

extern "C" {
JNIEXPORT void JNICALL
    Java_com_inanna_openglpolygonkotlin_PolygonJNI_onDrawFrame(JNIEnv *env, jclass type, jfloat red, jfloat green, jfloat blue) {
        (void) type;
        generateFragmentShader(red, green, blue);
        draw();
        usleep(500000); // draw every 0.5 seconds with new colors.
    }
}

extern "C" {
JNIEXPORT void JNICALL
    Java_com_inanna_openglpolygonkotlin_PolygonJNI_onSurfaceChanged(JNIEnv *env, jclass type) {
        (void) type;
        changeSurface(640, 480);
    }
}

extern "C" {
JNIEXPORT void JNICALL
    Java_com_inanna_openglpolygonkotlin_PolygonJNI_pushToVertices(JNIEnv *env, jclass type, jfloat x, jfloat y) {
        (void) type;
        push_to_vectorlist(x, y);
    }
}


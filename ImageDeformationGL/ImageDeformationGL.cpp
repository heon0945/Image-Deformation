#define GLEW_STATIC
#define STB_IMAGE_IMPLEMENTATION

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb-master/stb_image.h>
#include <toys.h>
#include <vector>

using namespace glm;
using namespace std;


/*
    - 이미지 변경 :
                    이미지 파일 로드
                    SCR_WIDTH, SCR_HEIGHT 조절

    - 마우스 오른쪽 버튼 : 포인트 추가
                         : 기존의 포인트에서 우클릭 시 포인트 삭제
    - 마우스 왼쪽 버튼
              + Dragging : 포인트 이동

    - ESC : 프로그램 종료
    - 上(상) 방향키 : 범위 줄이기
    - 下(하) 방향키 : 범위 키우기
    - 左(좌) 방향키 : 강도 높히기
    - 右(우) 방향키 : 강도 낮추기
    
*/


void init();
void render(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouseClick_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_callback(GLFWwindow* window, double xpos, double ypos);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

GLuint fbo = 0;
GLuint texID = 0;
GLuint vertBuf = 0;
GLuint textBuf = 0;
GLuint VAO = 0;
GLuint EBO = 0;

int SCR_WIDTH = 727, SCR_HEIGHT = 911; // size of window
float pointSize = 10;
//movingRange = 80;

bool dragging = false;

int const COUNT_CONTROL = 10; // the number of control points, 입력 받을 수 있는 최대 포인트 개수
vec2 controls[COUNT_CONTROL]; // control points
vec2 ranges[COUNT_CONTROL]; // control ranges for each point
vec2 deformDir[COUNT_CONTROL];
vec2 prepo;
int movingControl; // index: moving control point, 움직이고 있는 control point의 index 찾아서 저장함

int pointnum = 0; //입력 받은 포인트 개수
float changeMovingRange = 50;
float divInput = 5;
int deleteIndex = -1;

Program deformPro;

int main(void) {
    
    if (!glfwInit())
        exit(EXIT_FAILURE);
    
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Deformation", NULL, NULL);

    glfwMakeContextCurrent(window);
    glewInit();
    glfwSwapInterval(1);

    // when window size changed, viewport maintains image aspect ratio
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    //glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_callback);
    glfwSetMouseButtonCallback(window, mouseClick_callback);
    glfwSetKeyCallback(window, keyCallback);
    init();

    /*set control points
    controls[0] = vec2(width * 1 / 3, height - height * 1 / 3);
    controls[1] = vec2(width * 2 / 3, height - height * 1 / 3);
    controls[2] = vec2(width * 1 / 3, height - height * 3 / 4);
    controls[3] = vec2(width * 2 / 3, height - height * 3 / 4);
    ranges[0] = vec2(width * 1 / 3, height - height * 1 / 3);
    ranges[1] = vec2(width * 2 / 3, height - height * 1 / 3);
    ranges[2] = vec2(width * 1 / 3, height - height * 3 / 4);
    ranges[3] = vec2(width * 2 / 3, height - height * 3 / 4);*/

    while (!glfwWindowShouldClose(window)) {
        render(window);
        glfwPollEvents();
    }
    
    glDeleteFramebuffers(1, &fbo);
    glfwDestroyWindow(window);
    glfwTerminate();
}

void init() {

    // load shaders
    deformPro.loadShaders("shader.vert", "shader.frag");
    
    int channel, imgwidth, imgheight; //size of image

    // load image from file
    stbi_set_flip_vertically_on_load(true);
    unsigned char* bitmap 
        = stbi_load("/Users/SHINWOOSUB/Desktop/ImageDeformationGL/ImageDeformationGL/Sanghuni.png", &imgwidth, &imgheight, &channel, 4);

    if (bitmap == NULL)
        cout << "fail";

    // generate texture
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // send data to openGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, imgwidth, imgheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap);
    glGenerateMipmap(GL_TEXTURE_2D);

    // delete data
    stbi_image_free(bitmap);

    // vertices coordinates
    float vert[] = {
        1.f, 1.f, 0.0f,   //우측 상단
        1.f, -1.f, 0.0f,  //우측 하단
        -1.f, -1.f, 0.0f, //좌측 하단
        -1.f, 1.f, 0.0f   //좌측 상단
    };

    // elements(triangles)
    int triangle[] = {
        0, 1, 2,
        0, 2, 3
    };
 
    // texture coordinates
    float texCoord[] = {
    1.0f, 1.0f,  // 우측 상단 
    1.0f, 0.0f,  // 우측 하단
    0.0f, 0.0f,  // 좌측 하단
    0.0f, 1.0f   // 좌측 상단
    };

    // generate vertex buffer
    glGenBuffers(1, &vertBuf);
    glBindBuffer(GL_ARRAY_BUFFER, vertBuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);

    // generate texture buffer
    glGenBuffers(1, &textBuf);
    glBindBuffer(GL_ARRAY_BUFFER, textBuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoord), texCoord, GL_STATIC_DRAW);

    // generate vertex array
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // bind buffers to vertex array
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertBuf);
    glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, textBuf);
    glVertexAttribPointer(1, 2, GL_FLOAT, 0, 0, 0);
    
    // element array
    glGenBuffers(1, &EBO); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
}

void render(GLFWwindow* window) {
    
    int width, height;
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
   
    glfwGetFramebufferSize(window, &width, &height);

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(deformPro.programID);
    GLuint loc;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);
    loc = glGetUniformLocation(deformPro.programID, "myTexture");
    glUniform1i(loc, 0);   
    
    // screen size
    loc = glGetUniformLocation(deformPro.programID, "screen");
    glUniform2fv(loc, 1, value_ptr(vec2(SCR_WIDTH, SCR_HEIGHT)));

    loc = glGetUniformLocation(deformPro.programID, "pointSize");
    glUniform1f(loc, pointSize);

    //loc = glGetUniformLocation(deformPro.programID, "movingRange");
    //glUniform1f(loc, movingRange);

    loc = glGetUniformLocation(deformPro.programID, "changeMovingRange");
    glUniform1f(loc, changeMovingRange);

    loc = glGetUniformLocation(deformPro.programID, "divInput");
    glUniform1f(loc, divInput);

    loc = glGetUniformLocation(deformPro.programID, "deleteIndex");
    glUniform1f(loc, deleteIndex);
    


    // index number of all control points inclued user input 
    loc = glGetUniformLocation(deformPro.programID, "pointnum");
    glUniform1i(loc, pointnum);

    // control points
    loc = glGetUniformLocation(deformPro.programID, "controls");
    glUniform2fv(loc, pointnum, value_ptr(controls[0]));

    // control ranges
    loc = glGetUniformLocation(deformPro.programID, "ranges");
    glUniform2fv(loc, pointnum, value_ptr(ranges[0]));

    // control direction
    loc = glGetUniformLocation(deformPro.programID, "deformDir");
    glUniform2fv(loc, pointnum, value_ptr(deformDir[0]));

    // moving control point
    loc = glGetUniformLocation(deformPro.programID, "movingControl");
    glUniform1i(loc, movingControl);

    glBindVertexArray(VAO);  
    glBindBuffer(GL_ARRAY_BUFFER, VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glfwSwapBuffers(window);

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {

    float imgRatio = SCR_WIDTH / (float)SCR_HEIGHT;
    float frameRatio = width / (float)height;

    if (imgRatio == frameRatio) { // frame : same as viewport
        glViewport(0, 0, width, height);
    }
    else if (frameRatio < imgRatio) { // frame : longer than viewport
        float newHeight = width * SCR_HEIGHT / SCR_WIDTH;
        glViewport(0, (height - newHeight)/2, width, (int)newHeight);
    }
    else if (frameRatio > imgRatio) { // frame : wider than viewport
        float newWidth = height * SCR_WIDTH / SCR_HEIGHT;
        glViewport((width - newWidth)/2, 0, (int)newWidth, height);
    }

}

int width, height;

void cursor_callback(GLFWwindow* window, double xpos, double ypos) {

    if (dragging == true) {
        ypos = height - ypos;
        float dist =
            // sqrt(pow(ranges[movingControl].x-xpos, 2) + pow(ranges[movingControl].y-ypos, 2));
            length(ranges[movingControl] - vec2(xpos, ypos));

        if (dist < changeMovingRange) {
            controls[movingControl] = vec2(xpos, ypos);
        }
        else { // restrict moving control point from going out of the moving range
            vec2 temp;
            // changeMovingRange : dist = x : xpos
            temp.x = (changeMovingRange * (xpos-ranges[movingControl].x)) / dist;
            temp.y = (changeMovingRange * (ypos-ranges[movingControl].y)) / dist;
            controls[movingControl] = vec2(ranges[movingControl] + temp);
        }
        deformDir[movingControl] = vec2(controls[movingControl] - prepo);
    }
}

void mouseClick_callback(GLFWwindow* window, int button, int action, int mods) {

    glfwGetFramebufferSize(window, &width, &height);

    double xpo, ypo;
    glfwGetCursorPos(window, &xpo, &ypo);

    bool earlyclicked = false;

    if (button == GLFW_MOUSE_BUTTON_LEFT) {

        //change to frame 
        ypo = height - ypo;

        if (action == GLFW_PRESS) {

            prepo = vec2(xpo, ypo);

            for (int i = 0; i < pointnum; i++) {
                if (length(controls[i] - vec2(xpo, ypo)) < pointSize){
                    movingControl = i;
                    //cout << movingControl << endl;
                    dragging = true;
                }
            }
        }
        else if (action == GLFW_RELEASE)
            dragging = false;

    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {

            // change to frame 
            ypo = height - ypo;

            // delete point that user click
            for (int i = 0; i < pointnum; i++) {
                if (length(controls[i] - vec2(xpo, ypo)) < pointSize) {
                    // delete old input point
                    if (i != pointnum - 1) {
                        // copy and paste all elements
                        for (int j = i; j < pointnum - 1; j++) {
                            controls[j] = controls[j + 1];
                            ranges[j] = ranges[j + 1];
                            deformDir[j] = deformDir[j + 1];
                        }
                    }
                    // delete recent input point
                    else {   
                        controls[i] = vec2(0, 0);
                        ranges[i] = vec2(0, 0);
                        deformDir[i] = vec2(0, 0);
                    }
                    deleteIndex = i;
                    pointnum--;
                    earlyclicked = true;
                }
            }

            // create new point
            if (!earlyclicked && pointnum != COUNT_CONTROL) {
                controls[pointnum] = vec2(xpo, ypo);
                ranges[pointnum] = vec2(xpo, ypo);
                pointnum = pointnum++;
                deleteIndex = -1;
            }
        }
    }
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{  
    //ESC : 프로그램 종료
    //상하 방향키 : 범위
    //좌우 방향키 : 강도

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    else if (pointnum > 0) {
        if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
            if (changeMovingRange < 200)
                changeMovingRange = changeMovingRange + 10;
            //cout << "You clicked UP key, MovingRange is     " << changeMovingRange << endl;
        }
        else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {

            if (changeMovingRange > 0)
                changeMovingRange = changeMovingRange - 10;
            //cout << "You clicked DOWN key, MovingRange is     " << changeMovingRange << endl;
        }
        else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
            if (divInput > 1)
                divInput = divInput + 0.5;
            //cout << "You clicked LEFT key, divInput is     " << divInput << endl;
        }
        else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
            divInput = divInput - 0.5;
            //cout << "You clicked RIGHT key, divInput is     " << divInput << endl;
        }

    }
}
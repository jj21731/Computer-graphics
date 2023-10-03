#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>

#define WIDTH 320
#define HEIGHT 240


//--------task4---------
std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, int numberOfValues) {
    float stepsSizeX = (to.x - from.x) / (numberOfValues - 1);
    float stepsSizeY = (to.y - from.y) / (numberOfValues - 1);
    float stepsSizeZ = (to.z - from.z) / (numberOfValues - 1);
    std::vector<glm::vec3> steps; // Create a 3D container of steps
    for (int i = 0; i < numberOfValues; i++) { // Traverse all steps
        glm::vec3 step(from.x + i * stepsSizeX, from.y + i * stepsSizeY, from.z + i * stepsSizeZ);
        steps.push_back(step);//The purpose of this line of code is to store each interpolation step in the steps vector for later use.
    }
    return steps;
}
//---------task4----------

//----------task2---------
std::vector<float> interpolateSingleFloats(float from, float to, int numberOfValues) {
    std::vector<float> result;

    // Calculating step size
    float step = (to - from) / (numberOfValues - 1);

    // Fill the resulting vector with evenly spaced values
    for (int i = 0; i < numberOfValues; i++) {
        result.push_back(from + i * step);
    }

    return result;
}
//-----------task2-----------

void draw(DrawingWindow &window) {
    window.clearPixels();
    std::vector<float> gradientValues = interpolateSingleFloats(1.0f, 0.0f, window.width);
    for (size_t y = 0; y < window.height; y++) {
        for (size_t x = 0; x < window.width; x++) {
            float red = rand() % 256;
            float green = 0.0;
            float blue = 0.0;
//----------task3----------
            uint32_t grayValue = static_cast<uint8_t>(gradientValues[x] * 255);//灰色类型是uint8，这是类型转换
            uint32_t colour = (255 << 24) + (grayValue << 16) + (grayValue << 8) + grayValue;
            window.setPixelColour(x, y, colour);
        }
    }
}
//--------task3-------------

void handleEvent(SDL_Event event, DrawingWindow &window) {
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
        else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
        else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
        else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;
    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
        window.savePPM("output.ppm");
        window.saveBMP("output.bmp");
    }
}

//-------task5---------
void drawRGBColour(DrawingWindow &window){
    window.clearPixels();
    glm::vec3 topLeft(255, 0, 0);        // red
    glm::vec3 topRight(0, 0, 255);       // blue
    glm::vec3 bottomRight(0, 255, 0);    // green
    glm::vec3 bottomLeft(255, 255, 0);   // yellow
    std::vector<glm::vec3> steps;
    std::vector<glm::vec3> Left = interpolateThreeElementValues(topLeft,bottomLeft,window.height);
    std::vector<glm::vec3> Right = interpolateThreeElementValues(topRight,bottomRight,window.height);
    for(size_t y=0; y<window.height; y++){
        for(size_t x=0; x<window.width; x++){
            glm::vec3 left = Left[y];
            glm::vec3 right = Right[y];
            std::vector<glm::vec3>row = interpolateThreeElementValues(left,right,window.width);
            glm::vec3 currentColour = row[x];
            uint32_t colour = (255<<24) +(int(currentColour.x)<<16) + (int(currentColour.y)<<8) +int(currentColour.z);
            window.setPixelColour(x,y,colour);


        }
    }

}
//void drawRGBColors(DrawingWindow& window, int width,int height){
//     std::vector<glm::vec3> result;
//     glm::vec3 topleft(255,0,0);//red
//     glm::vec3 topRight(0,0,255);//blue
//     glm::vec3 bottomRight(0,255,0);//green
//     glm::vec3 bottomLeft(255,255,0);//yellow
//
// for(size_t y=0; y< window.height; y++){//Iterate over each line of the window (vertical）
//     glm::vec3 from(255,y,0);//red
//     glm::vec3 to(0,y,255-y);//blue
//      result = interpolateThreeElementValues(from, to, width);//give the value of the result
//        for(size_t x=0; x < window.width; x++){//iterate vertical
//        float red = result[x].x;
//        float green = result[x].y;
//        float blue = result[x].z;
//        uint32_t colour = (255<<24) + (int(red) << 16) + (int(green) << 8) + int(blue);
//
//        window.setPixelColour(x, y, colour);
//
//     }
//
// }
//
//}
//-------task5------------

int main(int argc, char *argv[]) {
//------task4--------
    glm::vec3 from(1.0f, 4.0f, 9.2f);
    glm::vec3 to(4.0f, 1.0f, 9.8f);
    int numberOfValues = 4;

    std::vector<glm::vec3> steps = interpolateThreeElementValues(from, to, numberOfValues);
     std::cout << std::fixed << std::setprecision(1);
     //We used std::fixed to indicate that std::setprecision retains zero digits after the decimal point. This will ensure that the output precision of floating-point numbers includes zero after the decimal point.
    // Print interpolation result
    for (const glm::vec3& value : steps) {
        std::cout << "(" << value.x << ", " << value.y << ", " << value.z << ")" << std::endl;
    }
//-------task4--------

//--------task2--------
    std::vector<float> result;
    result = interpolateSingleFloats(2.2f, 8.5f, 7);

    // 打印插值结果
    for (size_t i = 0; i < result.size(); i++) {
        std::cout << result[i] << " ";
    }
    std::cout << std::endl;
  //-----task2---------

    DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
    SDL_Event event;
    while (true) {
        // We MUST poll for events - otherwise the window will freeze!
        if (window.pollForInputEvents(event)) handleEvent(event, window);
        drawRGBColour(window);
//        drawRGBColors(window,window.width,window.height);
        // Need to render the frame at the end, or nothing actually gets shown on the screen!
        window.renderFrame();
    }

    return 0;
}
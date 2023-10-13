#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include<CanvasPoint.h>
#include<Colour.h>
#include<TextureMap.h>


#define WIDTH 320
#define HEIGHT 240

void drawline(CanvasPoint to, CanvasPoint from, Colour colour, DrawingWindow &window) {
    float xDiff = to.x - from.x;
    float yDiff = to.y - from.y;
    float numSteps = std::max(std::abs(xDiff), std::abs(yDiff));
    float xStepSize = xDiff / numSteps;
    float yStepSize = yDiff / numSteps;

    for (float i = 0.0; i < numSteps; i++) {
        float x = from.x + (xStepSize * i);
        float y = from.y + (yStepSize * i);
        uint32_t pixelColor = (255 << 24) + (int(colour.red) << 16) + (int(colour.green) << 8) + int(colour.blue);
        window.setPixelColour(round(x), round(y), pixelColor);
    }
}

void line(DrawingWindow &window){
    window.clearPixels();
    CanvasPoint startpoint1 = {160,0};
    CanvasPoint endpoint1 = {160,240};
    Colour Colourline1 = {255,255,255};


    CanvasPoint startpoint2 = {0,0};
    CanvasPoint endpoint2 = {160,120};

    drawline(endpoint1,startpoint1,Colourline1,window);
    drawline(endpoint2,startpoint2,Colourline1,window);
    CanvasPoint startpoint3 = {320,0};
    CanvasPoint endpoint3 = {160,120};

    drawline(endpoint3,startpoint3,Colourline1,window);

    CanvasPoint startpoint4 = {320/3,120};
    CanvasPoint endpoint4 = {2 * 320/3,120};

    drawline(endpoint4,startpoint4,Colourline1,window);

}

void drawtriangle(CanvasTriangle triangle, Colour colour, DrawingWindow &window){
    drawline(triangle.v0(), triangle.v1(), colour, window);
    drawline(triangle.v1(), triangle.v2(), colour, window);
    drawline(triangle.v2(), triangle.v0(), colour, window);
}
CanvasTriangle generateRandomTriangle(int width = WIDTH, int height = HEIGHT) {
    CanvasPoint v0 = CanvasPoint(rand() % width, rand() % height);
    CanvasPoint v1 = CanvasPoint(rand() % width, rand() % height);
    CanvasPoint v2 = CanvasPoint(rand() % width, rand() % height);//    rand()can give us a range from 0-width or o-height
    return CanvasTriangle(v0,v1,v2);
}

Colour trianglecoulour(int width = WIDTH, int height = HEIGHT){
    int red = rand() % 256;
    int green = rand() % 256;
    int blue = rand() % 256;
    return Colour(red, green, blue);

}



void fillTriangle(CanvasTriangle triangle, Colour colour,Colour outlineColour, DrawingWindow& window) {
    CanvasPoint v0 = triangle.v0();
    CanvasPoint v1 = triangle.v1();
    CanvasPoint v2 = triangle.v2();

    // 按垂直位置排序顶点
    if (v0.y > v1.y) std::swap(v0, v1);
    if (v0.y > v2.y) std::swap(v0, v2);
    if (v1.y > v2.y) std::swap(v1, v2);

    CanvasPoint topVertex = v0;
    CanvasPoint middleVertex = v1;
    CanvasPoint bottomVertex = v2;

    // 计算左边的斜率
    float Slope1 = (middleVertex.x - topVertex.x) / (middleVertex.y - topVertex.y);
    // 计算右边的斜率
    float Slope2 = (bottomVertex.x - topVertex.x) / (bottomVertex.y - topVertex.y);

    // 初始化左边和右边的x坐标
    float currentX1 = topVertex.x;
    float currentX2 = topVertex.x;

    // 处理上半部分三角形
    for (int y = topVertex.y; y <= middleVertex.y; ++y) {
        drawline(CanvasPoint(currentX1, y), CanvasPoint(currentX2, y), colour, window);
        currentX1 += Slope1;
        currentX2 += Slope2;
    }

    // 更新左边的斜率和x坐标
    Slope1 = (bottomVertex.x - middleVertex.x) / (bottomVertex.y - middleVertex.y);
    currentX1 = middleVertex.x;

    // 处理下半部分三角形
    for (int y = middleVertex.y + 1; y <= bottomVertex.y; ++y) {
        drawline(CanvasPoint(currentX1, y), CanvasPoint(currentX2, y), colour, window);
        currentX1 += Slope1;
        currentX2 += Slope2;
    }
    drawline(v0, v1, outlineColour, window);
    drawline(v1, v2, outlineColour, window);
    drawline(v2, v0, outlineColour, window);
}

void drawTexturedTriangle(CanvasTriangle triangle, TextureMap texture, DrawingWindow& window) {
    CanvasPoint v0 = triangle.v0();
    CanvasPoint v1 = triangle.v1();
    CanvasPoint v2 = triangle.v2();

    // Sort vertices by their y-coordinates
    if (v0.y > v1.y) {
        std::swap(v0, v1);
    }
    if (v0.y > v2.y) {
        std::swap(v0, v2);
    }
    if (v1.y > v2.y) {
        std::swap(v1, v2);
    }

    CanvasPoint top = v0;
    CanvasPoint middle = v1;
    CanvasPoint bottom = v2;

    // Calculate slopes for the top and bottom edges
    float slope1 = (middle.x - top.x) / (middle.y - top.y);
    float slope2 = (bottom.x - top.x) / (bottom.y - top.y);

    float left_x = top.x;
    float right_x = top.x;

    // First part of triangle (top to middle)
    for (int y = top.y; y <= middle.y; ++y) {
        // Calculate the corresponding texturePoints for CanvasPoints
        float t1 = (y - top.y) / (middle.y - top.y); //插值系数，用来插值top和middle两个点之间的值
        float t2 = (y - top.y) / (bottom.y - top.y); //bottom和top两个点之间的值
        float texX1 = top.texturePoint.x + (middle.texturePoint.x - top.texturePoint.x) * t1;
        float texX2 = top.texturePoint.x + (bottom.texturePoint.x - top.texturePoint.x) * t2;
        float texY1 = top.texturePoint.y + (middle.texturePoint.y - top.texturePoint.y) * t1;
        float texY2 = top.texturePoint.y + (bottom.texturePoint.y - top.texturePoint.y) * t2;

        // Draw the horizontal line with texture mapping
        for (int x = round(left_x); x <= round(right_x); ++x) {
            // Interpolate t based on the current x position
            float t = (x - left_x) / (right_x - left_x);

            // Calculate the interpolated texture coordinates
            float texXf = texX1 + (texX2 - texX1) * t;
            float texYf = texY1 + (texY2 - texY1) * t;

            // Ensure that texXf and texYf are within the texture dimensions
            if (texXf >= 0 && texXf < texture.width && texYf >= 0 && texYf < texture.height) {
                // Convert texXf and texYf to integers for indexing
                int texX = static_cast<int>(texXf);
                int texY = static_cast<int>(texYf);

                // Calculate the index to access the texture pixel
                int index = texY * texture.width + texX;
                window.setPixelColour(x, y, texture.pixels[index]);
            }
        }

        left_x += slope1;
        right_x += slope2;
    }

    // Initialize left and right x-coordinates for the bottom edge
    left_x = middle.x;
    float slope3 = (bottom.x - middle.x) / (bottom.y - middle.y);

    // Second part of triangle (middle to bottom)
    for (int y = middle.y + 1; y <= bottom.y; ++y) {
        // Calculate the corresponding texture coordinates for Canvas Points
        float t1 = (y - middle.y) / (bottom.y - middle.y);
        float t2 = (y - top.y) / (bottom.y - top.y);
        float texX1 = middle.texturePoint.x + (bottom.texturePoint.x - middle.texturePoint.x) * t1;
        float texX2 = top.texturePoint.x + (bottom.texturePoint.x - top.texturePoint.x) * t2;
        float texY1 = middle.texturePoint.y + (bottom.texturePoint.y - middle.texturePoint.y) * t1;
        float texY2 = top.texturePoint.y + (bottom.texturePoint.y - top.texturePoint.y) * t2;

        // Draw the horizontal line with texture mapping
        for (int x = round(left_x); x <= round(right_x); ++x) {
            // Interpolate t based on the current x position
            float t = (x - left_x) / (right_x - left_x);

            // Calculate the interpolated texture coordinates
            float texXf = texX1 + (texX2 - texX1) * t;
            float texYf = texY1 + (texY2 - texY1) * t;

            // Ensure that texXf and texYf are within the texture dimensions
            if (texXf >= 0 && texXf < texture.width && texYf >= 0 && texYf < texture.height) {
                // Convert texXf and texYf to integers for indexing
                int texX = static_cast<int>(texXf);
                int texY = static_cast<int>(texYf);

                // Calculate the index to access the texture pixel
                int index = texY * texture.width + texX;
                window.setPixelColour(x, y, texture.pixels[index]);
            }
        }
        left_x += slope3;
        right_x += slope2;
    }
    drawtriangle(triangle, Colour{255, 255, 255}, window);
}





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
    for (const glm::vec3 &value: steps) {
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
    bool drawTriangle = false; // 新增一个变量，用于控制是否绘制三角形
    while (true) {
        // 首先轮询事件
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                break; // 退出循环
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_q) {
                    break; // 按下 'q' 键退出循环
                } else if (event.key.keysym.sym == SDLK_u) {
                    // 按下 'u' 键生成并绘制随机三角形
                    drawTriangle = true; // 设置标志以绘制三角形
                } else if (event.key.keysym.sym == SDLK_c) {
                    // 按下 'c' 键清空屏幕
                    window.clearPixels();
                }
            }
            // 处理其他事件，例如鼠标事件
        }

        // 如果标志为 true，调用绘制三角形的函数
        if (drawTriangle) {
            CanvasTriangle triangle = generateRandomTriangle();
            Colour color = trianglecoulour();
            drawtriangle(triangle, color, window);
            drawTriangle = false; // 绘制完成后将标志设置为 false
        }

        // 渲染图像和其他逻辑
        // ...
        bool drawRandomFillTriangle = false;
        Colour randomOutlineColour = trianglecoulour();

// 主循环中的事件处理
        while (true) {
            // 处理事件
            SDL_Event event;
            if (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    break; // 退出循环
                } else if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_q) {
                        break; // 按下 'q' 键退出循环
                    } else if (event.key.keysym.sym == SDLK_f) {
                        drawRandomFillTriangle = true; // 按下 'f' 键设置标志以绘制随机填充三角形
                    }
                    // 处理其他事件
                }

                // 其他事件处理代码
            }

            // 清空屏幕或执行其他渲染逻辑
            // ...

            // 如果标志为 true，绘制随机填充三角形
            if (drawRandomFillTriangle) {
                CanvasTriangle randomTriangle = generateRandomTriangle();
                Colour randomColour = trianglecoulour();
                fillTriangle(randomTriangle, randomColour, randomOutlineColour, window);
                drawRandomFillTriangle = false; // 绘制完成后将标志设置为 false
            }
            TextureMap texture = TextureMap("/Users/lil/Desktop/CG2023-main/RedNoise/texture.ppm");

            // Define the vertices of the triangle
            CanvasPoint p1(160, 10);
            CanvasPoint p2(300, 230);
            CanvasPoint p3(10, 150);
            p1.texturePoint = TexturePoint(195,5);
            p2.texturePoint = TexturePoint(395,380);
            p3.texturePoint = TexturePoint(65,330);




            // Create a CanvasTriangle using the specified vertices
            CanvasTriangle triangle(p1, p2, p3);

            // Render the textured triangle
            drawTexturedTriangle(triangle,texture,window);




            // 渲染帧
            window.renderFrame();
        }
        return 0;
    }
}
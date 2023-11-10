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
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include<sstream>


#define WIDTH  1000
#define HEIGHT  1000

void drawline(CanvasPoint from, CanvasPoint to, Colour colour, DrawingWindow &window, std::vector<std::vector<float>>& depthBuffer) {
    float xDiff = to.x - from.x;
    float yDiff = to.y - from.y;
    float zDiff = to.depth - from.depth;
    float numSteps = 10 * std::max(std::abs(xDiff), std::abs(yDiff));
    float xStepSize = xDiff / numSteps;
    float yStepSize = yDiff / numSteps;
    float zStepSize = zDiff / numSteps;

    for (float i = 0.0; i < numSteps; i++) {
        float x = from.x + (xStepSize * i);
        float y = from.y + (yStepSize * i);
        float z = -(from.depth + (zStepSize * i));

        int roundedX = round(x);
        int roundedY = round(y);

        // 确保x和y在合适的范围内
        if (roundedX >= 0 && roundedX < window.width && roundedY >= 0 && roundedY < window.height) {
            if (z < depthBuffer[roundedY][roundedX]) {
                uint32_t pixelColor = (255 << 24) + (int(colour.red) << 16) + (int(colour.green) << 8) + int(colour.blue);
                window.setPixelColour(roundedX, roundedY, pixelColor);
                depthBuffer[roundedY][roundedX] = z;
            }
        }
    }
}



//void line(DrawingWindow &window){
//    window.clearPixels();
//    CanvasPoint startpoint1 = {160,0};
//    CanvasPoint endpoint1 = {160,240};
//    Colour Colourline1 = {255,255,255};
//
//
//    CanvasPoint startpoint2 = {0,0};
//    CanvasPoint endpoint2 = {160,120};
//
//    drawline(endpoint1,startpoint1,Colourline1,window);
//    drawline(endpoint2,startpoint2,Colourline1,window);
//    CanvasPoint startpoint3 = {320,0};
//    CanvasPoint endpoint3 = {160,120};
//
//    drawline(endpoint3,startpoint3,Colourline1,window);
//
//    CanvasPoint startpoint4 = {320/3,120};
//    CanvasPoint endpoint4 = {2 * 320/3,120};
//
//    drawline(endpoint4,startpoint4,Colourline1,window);
//
//}

//void drawtriangle(CanvasTriangle triangle, Colour colour, DrawingWindow &window){
//    drawline(triangle.v0(), triangle.v1(), colour, window);
//    drawline(triangle.v1(), triangle.v2(), colour, window);
//    drawline(triangle.v2(), triangle.v0(), colour, window);
//}
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



void fillTriangle(CanvasTriangle triangle, Colour colour, Colour outlineColour, DrawingWindow& window, std::vector<std::vector<float>>& depthBuffer) {
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
    float depthSlope1 = (middleVertex.depth - topVertex.depth) / (middleVertex.y - topVertex.y);
    float depthSlope2 = (bottomVertex.depth - topVertex.depth) / (bottomVertex.y - topVertex.y);


    // 初始化左边和右边的x坐标
    float left_x = topVertex.x;
    float right_x = topVertex.x;
    float left_depth = topVertex.depth;
    float right_depth = topVertex.depth;

    // 处理上半部分三角形
    for (float y = topVertex.y; y < middleVertex.y; y++) {
        CanvasPoint from(round(left_x),round(y),left_depth);
        CanvasPoint to(round(right_x),round(y),right_depth);
        drawline(from,to, colour, window, depthBuffer);
        right_x += Slope1;
        left_x += Slope2;
        left_depth += depthSlope2;
        right_depth += depthSlope1;
    }

       right_x = middleVertex.x;
       float slope3 = (bottomVertex.x - middleVertex.x) / (bottomVertex.y - middleVertex.y);
       float depthslope3 = (bottomVertex.depth - middleVertex.depth) / (bottomVertex.y - middleVertex.y);


    // 处理下半部分三角形
    for (float y = middleVertex.y; y <= bottomVertex.y; y++) {
        CanvasPoint from(round(left_x),round(y),left_depth);
        CanvasPoint to(round(right_x),round(y),right_depth);
        drawline(from, to, colour, window, depthBuffer);
        right_x += slope3;
        left_x += Slope2;
        left_depth += depthSlope2;
        right_depth += depthslope3;
    }

    drawline(v0, v1, outlineColour, window, depthBuffer);
    drawline(v1, v2, outlineColour, window, depthBuffer);
    drawline(v2, v0, outlineColour, window, depthBuffer);
}

//void drawTexturedTriangle(CanvasTriangle triangle, TextureMap texture, DrawingWindow& window) {
//    CanvasPoint v0 = triangle.v0();
//    CanvasPoint v1 = triangle.v1();
//    CanvasPoint v2 = triangle.v2();
//
//    // Sort vertices by their y-coordinates
//    if (v0.y > v1.y) {
//        std::swap(v0, v1);
//    }
//    if (v0.y > v2.y) {
//        std::swap(v0, v2);
//    }
//    if (v1.y > v2.y) {
//        std::swap(v1, v2);
//    }
//
//    CanvasPoint top = v0;
//    CanvasPoint middle = v1;
//    CanvasPoint bottom = v2;
//
//    // Calculate slopes for the top and bottom edges
//    float slope1 = (middle.x - top.x) / (middle.y - top.y);
//    float slope2 = (bottom.x - top.x) / (bottom.y - top.y);
//
//    float left_x = top.x;
//    float right_x = top.x;
//
//    // First part of triangle (top to middle)
//    for (int y = top.y; y <= middle.y; ++y) {
//        // Calculate the corresponding texturePoints for CanvasPoints
//        float t1 = (y - top.y) / (middle.y - top.y); //插值系数，用来插值top和middle两个点之间的值
//        float t2 = (y - top.y) / (bottom.y - top.y); //bottom和top两个点之间的值
//        float texX1 = top.texturePoint.x + (middle.texturePoint.x - top.texturePoint.x) * t1;
//        float texX2 = top.texturePoint.x + (bottom.texturePoint.x - top.texturePoint.x) * t2;
//        float texY1 = top.texturePoint.y + (middle.texturePoint.y - top.texturePoint.y) * t1;
//        float texY2 = top.texturePoint.y + (bottom.texturePoint.y - top.texturePoint.y) * t2;
//
//        // Draw the horizontal line with texture mapping
//        for (int x = round(left_x); x <= round(right_x); ++x) {
//            // Interpolate t based on the current x position
//            float t = (x - left_x) / (right_x - left_x);
//
//            // Calculate the interpolated texture coordinates
//            float texXf = texX1 + (texX2 - texX1) * t;
//            float texYf = texY1 + (texY2 - texY1) * t;
//
//            // Ensure that texXf and texYf are within the texture dimensions
//            if (texXf >= 0 && texXf < texture.width && texYf >= 0 && texYf < texture.height) {
//                // Convert texXf and texYf to integers for indexing
//                int texX = static_cast<int>(texXf);
//                int texY = static_cast<int>(texYf);
//
//                // Calculate the index to access the texture pixel
//                int index = texY * texture.width + texX;
//                window.setPixelColour(x, y, texture.pixels[index]);
//            }
//        }
//
//        left_x += slope1;
//        right_x += slope2;
//    }
//
//    // Initialize left and right x-coordinates for the bottom edge
//    left_x = middle.x;
//    float slope3 = (bottom.x - middle.x) / (bottom.y - middle.y);
//
//    // Second part of triangle (middle to bottom)
//    for (int y = middle.y + 1; y <= bottom.y; ++y) {
//        // Calculate the corresponding texture coordinates for Canvas Points
//        float t1 = (y - middle.y) / (bottom.y - middle.y);
//        float t2 = (y - top.y) / (bottom.y - top.y);
//        float texX1 = middle.texturePoint.x + (bottom.texturePoint.x - middle.texturePoint.x) * t1;
//        float texX2 = top.texturePoint.x + (bottom.texturePoint.x - top.texturePoint.x) * t2;
//        float texY1 = middle.texturePoint.y + (bottom.texturePoint.y - middle.texturePoint.y) * t1;
//        float texY2 = top.texturePoint.y + (bottom.texturePoint.y - top.texturePoint.y) * t2;
//
//        // Draw the horizontal line with texture mapping
//        for (int x = round(left_x); x <= round(right_x); ++x) {
//            // Interpolate t based on the current x position
//            float t = (x - left_x) / (right_x - left_x);
//
//            // Calculate the interpolated texture coordinates
//            float texXf = texX1 + (texX2 - texX1) * t;
//            float texYf = texY1 + (texY2 - texY1) * t;
//
//            // Ensure that texXf and texYf are within the texture dimensions
//            if (texXf >= 0 && texXf < texture.width && texYf >= 0 && texYf < texture.height) {
//                // Convert texXf and texYf to integers for indexing
//                int texX = static_cast<int>(texXf);
//                int texY = static_cast<int>(texYf);
//
//                // Calculate the index to access the texture pixel
//                int index = texY * texture.width + texX;
//                window.setPixelColour(x, y, texture.pixels[index]);
//            }
//        }
//        left_x += slope3;
//        right_x += slope2;
//    }
//    drawtriangle(triangle, Colour{255, 255, 255}, window);
//}



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

//void handleEvent(SDL_Event event, DrawingWindow &window) {
//    if (event.type == SDL_KEYDOWN) {
//        if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
//        else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
//        else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
//        else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;
//    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
//        window.savePPM("output.ppm");
//        window.saveBMP("output.bmp");
//    }
//
//}

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

//int main(int argc, char *argv[]) {
//
//
////------task4--------
//    glm::vec3 from(1.0f, 4.0f, 9.2f);
//    glm::vec3 to(4.0f, 1.0f, 9.8f);
//    int numberOfValues = 4;
//
//    std::vector<glm::vec3> steps = interpolateThreeElementValues(from, to, numberOfValues);
//    std::cout << std::fixed << std::setprecision(1);
//    //We used std::fixed to indicate that std::setprecision retains zero digits after the decimal point. This will ensure that the output precision of floating-point numbers includes zero after the decimal point.
//    // Print interpolation result
//    for (const glm::vec3 &value: steps) {
//        std::cout << "(" << value.x << ", " << value.y << ", " << value.z << ")" << std::endl;
//    }
////-------task4--------
//
//
//
//
//
//
//
//
//
//
//
//
////--------task2--------
//    std::vector<float> result;
//    result = interpolateSingleFloats(2.2f, 8.5f, 7);
//
//    // 打印插值结果
//    for (size_t i = 0; i < result.size(); i++) {
//        std::cout << result[i] << " ";
//    }
//    std::cout << std::endl;
//    //-----task2---------
//
//    DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
//    SDL_Event event;
//    bool drawTriangle = false; // 新增一个变量，用于控制是否绘制三角形
//    while (true) {
//        // 首先轮询事件
//        if (SDL_PollEvent(&event)) {
//            if (event.type == SDL_QUIT) {
//                break; // 退出循环
//            } else if (event.type == SDL_KEYDOWN) {
//                if (event.key.keysym.sym == SDLK_q) {
//                    break; // 按下 'q' 键退出循环
//                } else if (event.key.keysym.sym == SDLK_u) {
//                    // 按下 'u' 键生成并绘制随机三角形
//                    drawTriangle = true; // 设置标志以绘制三角形
//                } else if (event.key.keysym.sym == SDLK_c) {
//                    // 按下 'c' 键清空屏幕
//                    window.clearPixels();
//                }
//            }
//            // 处理其他事件，例如鼠标事件
//        }
//
//        // 如果标志为 true，调用绘制三角形的函数
//        if (drawTriangle) {
//            CanvasTriangle triangle = generateRandomTriangle();
//            Colour color = trianglecoulour();
//            drawtriangle(triangle, color, window);
//            drawTriangle = false; // 绘制完成后将标志设置为 false
//        }
//
//        // 渲染图像和其他逻辑
//        // ...
//        bool drawRandomFillTriangle = false;
//        Colour randomOutlineColour = trianglecoulour();
//
//// 主循环中的事件处理
//        while (true) {
//            // 处理事件
//            SDL_Event event;
//            if (SDL_PollEvent(&event)) {
//                if (event.type == SDL_QUIT) {
//                    break; // 退出循环
//                } else if (event.type == SDL_KEYDOWN) {
//                    if (event.key.keysym.sym == SDLK_q) {
//                        break; // 按下 'q' 键退出循环
//                    } else if (event.key.keysym.sym == SDLK_f) {
//                        drawRandomFillTriangle = true; // 按下 'f' 键设置标志以绘制随机填充三角形
//                    }
//                    // 处理其他事件
//                }
//
//                // 其他事件处理代码
//            }
//
//            // 清空屏幕或执行其他渲染逻辑
//            // ...
//
//            // 如果标志为 true，绘制随机填充三角形
//            if (drawRandomFillTriangle) {
//                CanvasTriangle randomTriangle = generateRandomTriangle();
//                Colour randomColour = trianglecoulour();
//                fillTriangle(randomTriangle, randomColour, randomOutlineColour, window);
//                drawRandomFillTriangle = false; // 绘制完成后将标志设置为 false
//            }
//            TextureMap texture = TextureMap("/Users/lil/Desktop/CG2023-main/RedNoise/texture.ppm");
//
//            // Define the vertices of the triangle
//            CanvasPoint p1(160, 10);
//            CanvasPoint p2(300, 230);
//            CanvasPoint p3(10, 150);
//            p1.texturePoint = TexturePoint(195,5);
//            p2.texturePoint = TexturePoint(395,380);
//            p3.texturePoint = TexturePoint(65,330);
//
//
//
//
//            // Create a CanvasTriangle using the specified vertices
//            CanvasTriangle triangle(p1, p2, p3);
//
//            // Render the textured triangle
//            drawTexturedTriangle(triangle,texture,window);
//
//
//
//
//            // 渲染帧
//            window.renderFrame();
//        }
//        return 0;
//    }
//}


void loadOBJ(const std::string& filename, float scalingFactor,
             std::vector<std::vector<float>>& vertices,
             std::vector<std::vector<int>>& faces,
             std::vector<std::tuple<int,int,int>>& faceColours,
             const std::unordered_map<std::string, std::tuple<int,int,int>>& materialMap) {

    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Unable to open OBJ file." << std::endl;
        return;
    }
    std::tuple<int,int,int> currentColour = std::make_tuple(255,255,255);
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "v") {
            std::vector<float> vertex(3);
            iss >> vertex[0] >> vertex[1] >> vertex[2];
            vertex[0] *= scalingFactor;
            vertex[1] *= scalingFactor;
            vertex[2] *= scalingFactor;
            vertices.push_back(vertex);
        } else if(type == "usemtl") {
            std::string materialName;
            iss >> materialName;
            currentColour = materialMap.at(materialName);  // 加上分号

        } else if (type == "f") {
            std::vector<int> face;
            std::string token;
            while (iss >> token) {
                size_t pos = token.find('/');
                if (pos != std::string::npos) {
                    token = token.substr(0, pos);  // 取/之前的索引
                }
                face.push_back(stoi(token));
            }
            faces.push_back(face);
            faceColours.push_back(currentColour);
        }
    }
}
std::unordered_map<std::string, std::tuple<int, int, int>> loadMTL(const std::string& filename) {
    std::ifstream file(filename);
    std::unordered_map<std::string, std::tuple<int, int, int>> materialMap;

    if (!file.is_open()) {
        std::cerr << "Error: Unable to open MTL file." << std::endl;
        return materialMap;
    }

    std::string line;
    std::string currentMaterial;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "newmtl") {
            iss >> currentMaterial;
        } else if (type == "Kd") {  // Assuming diffuse color
            float r, g, b;
            iss >> r >> g >> b;

// 验证颜色值在0到1的范围内
            if (r < 0.0f || r > 1.0f || g < 0.0f || g > 1.0f || b < 0.0f || b > 1.0f) {
                std::cerr << "Warning: Invalid color value detected in .mtl file!" << std::endl;
                // 可以选择继续执行，或者根据需要处理这个错误
            }

            materialMap[currentMaterial] = std::make_tuple(
                    static_cast<int>(r * 255 + 0.5),
                    static_cast<int>(g * 255 + 0.5),
                    static_cast<int>(b * 255 + 0.5)
            );
        }
    }

    return materialMap;
}
glm::mat3 cameraOrientation = glm::mat3(1.0f);//初始化单位矩阵
CanvasPoint getCanvasIntersection(const glm::vec3& cameraPosition, const glm::vec3& vertexPosition, const glm::mat3& cameraOrientation, float focallength) {
    glm::vec3 adjustedPosition = cameraOrientation * (vertexPosition - cameraPosition); // 应用相机方向
    float scaleFactor = 240.0f;

    float x = -scaleFactor * adjustedPosition.x / adjustedPosition.z + WIDTH / 2;
    float y = scaleFactor * adjustedPosition.y / adjustedPosition.z + HEIGHT / 2;

    return CanvasPoint(x, y, adjustedPosition.z);
}


//void renderPointCloud(DrawingWindow &window, const std::vector<std::vector<float>>& vertices, const glm::vec3& cameraPosition, float focalLength) {
//    for (const auto& vertexVec : vertices) {
//        glm::vec3 vertexPosition(vertexVec[0], vertexVec[1], vertexVec[2]);
//        CanvasPoint projectedPoint = getCanvasIntersection(cameraPosition, vertexPosition, focalLength);
//
//        int adjustedX = static_cast<int>(projectedPoint.x);
//        int adjustedY = static_cast<int>(projectedPoint.y);
//        uint32_t whiteColour = (255 << 16) + (255 << 8) + 255;  // RGB for white
//
//        // Bounding Check
//        if (adjustedX >= 0 && adjustedX < window.width && adjustedY >= 0 && adjustedY < window.height) {
//            window.setPixelColour(adjustedX, adjustedY, whiteColour);
//        }
//    }
//}
//void drawLine2(DrawingWindow &window, CanvasPoint start, CanvasPoint end, uint32_t color) {
//    float xDiff = end.x - start.x;
//    float yDiff = end.y - start.y;
//    float length = sqrt(xDiff*xDiff + yDiff*yDiff);
//
//    for (float t = 0; t <= 1; t += 1.0/length) {
//        float x = start.x + t * xDiff;
//        float y = start.y + t * yDiff;
//
//        int adjustedX = static_cast<int>(x);
//        int adjustedY = static_cast<int>(y);
//
//        if (adjustedX >= 0 && adjustedX < window.width && adjustedY >= 0 && adjustedY < window.height) {
//            window.setPixelColour(adjustedX, adjustedY, color);
//        }
//    }
//}
//void renderWireframe(DrawingWindow &window, const std::vector<std::vector<float>>& vertices, const std::vector<std::vector<int>>& faces, const glm::vec3& cameraPosition, float focalLength) {
//    uint32_t whiteColour = (255 << 16) + (255 << 8) + 255;  // RGB for white
//
//    for (const auto& face : faces) {
//        std::vector<CanvasPoint> projectedVertices;
//        for (size_t i = 0; i < face.size(); ++i) {
//            const auto& vertex3D = vertices[face[i] - 1];  // OBJ indices start from 1
//            projectedVertices.push_back(getCanvasIntersection(cameraPosition, glm::vec3(vertex3D[0], vertex3D[1], vertex3D[2]), focalLength));
//        }
//
//        drawLine(window, projectedVertices[0], projectedVertices[1], whiteColour);
//        drawLine(window, projectedVertices[1], projectedVertices[2], whiteColour);
//        drawLine(window, projectedVertices[2], projectedVertices[0], whiteColour);
//    }
//}



void renderFilledTriangles(DrawingWindow &window,
                           const std::vector<std::vector<float>>& vertices,
                           const std::vector<std::vector<int>>& faces,
                           const std::vector<std::tuple<int,int,int>>& faceColours,
                           const glm::vec3& cameraPosition,
                           float focalLength,
                           std::vector<std::vector<float>>& depthBuffer) {
    // ... 函数体内容不变 ...
    for (size_t f = 0; f < faces.size(); f++) {  // <- 使用索引循环以便同时访问faces和faceColours
        const auto& face = faces[f];
        std::vector<CanvasPoint> projectedVertices;
        for (size_t i = 0; i < face.size(); ++i) {
            const auto& vertex3D = vertices[face[i] - 1];  // OBJ indices start from 1
            projectedVertices.push_back(getCanvasIntersection(cameraPosition, glm::vec3(vertex3D[0], vertex3D[1], vertex3D[2]),cameraOrientation, focalLength));
        }

        auto colourTuple = faceColours[f];
        Colour fillColour(std::get<0>(colourTuple), std::get<1>(colourTuple), std::get<2>(colourTuple));  // <- 使用MTL文件中的颜色
        Colour outlineColour = fillColour;  // <- 将轮廓颜色设置为与填充颜色相同

        CanvasTriangle triangle(projectedVertices[0], projectedVertices[1], projectedVertices[2]);
        fillTriangle(triangle, fillColour, outlineColour, window, depthBuffer);
    }
}


//void handleEvent(SDL_Event event, DrawingWindow &window) {
//    if (event.type == SDL_KEYDOWN) {
//        if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
//        else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
//        else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
//        else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;
//    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
//        window.savePPM("output.ppm");
//        window.saveBMP("output.bmp");
//    }
//
//}


// processInput函数
void handleevent(SDL_Event &event, DrawingWindow &window, glm::vec3 &cameraPosition,glm::mat3 &cameraOrientation, float delta,bool &autoRotate) {
    // 根据event处理输入
    switch (event.type) {
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {

                case SDLK_w: cameraPosition.z -= delta; break;
                case SDLK_s: cameraPosition.z += delta; break;
                case SDLK_d: cameraPosition.x -= delta; break;  // 向右移动
                case SDLK_a: cameraPosition.x += delta; break;  // 向左移动
                case SDLK_q: { // 绕Y轴旋转 - 左转
                    float angle = glm::radians(-1.0f); // 逆时针为负角度
                    glm::mat3 rotateY = glm::mat3(
                            glm::cos(angle), 0.0f, glm::sin(angle),
                            0.0f, 1.0f, 0.0f,
                            -glm::sin(angle), 0.0f, glm::cos(angle)
                    );
                    cameraOrientation = rotateY * cameraOrientation;
                    break;
                }
                case SDLK_e: { // 绕Y轴旋转 - 右转
                    float angle = glm::radians(1.0f); // 顺时针为正角度
                    glm::mat3 rotateY = glm::mat3(
                            glm::cos(angle), 0.0f, glm::sin(angle),
                            0.0f, 1.0f, 0.0f,
                            -glm::sin(angle), 0.0f, glm::cos(angle)
                    );
                    cameraOrientation = rotateY * cameraOrientation;
                    break;
                }
                case SDLK_r: { // 绕X轴旋转 - 向上倾斜
                    float angle = glm::radians(-1.0f); // 向上为负角度
                    glm::mat3 rotateX = glm::mat3(
                            1.0f, 0.0f, 0.0f,
                            0.0f, glm::cos(angle), -glm::sin(angle),
                            0.0f, glm::sin(angle), glm::cos(angle)
                    );
                    cameraOrientation = rotateX * cameraOrientation;
                    break;
                }
                case SDLK_f: { // 绕X轴旋转 - 向下倾斜
                    float angle = glm::radians(1.0f); // 向下为正角度
                    glm::mat3 rotateX = glm::mat3(
                            1.0f, 0.0f, 0.0f,
                            0.0f, glm::cos(angle), -glm::sin(angle),
                            0.0f, glm::sin(angle), glm::cos(angle)
                    );
                    cameraOrientation = rotateX * cameraOrientation;
                    break;
                }
                case SDLK_SPACE:
                    autoRotate = !autoRotate;
                    break;

            }


            cameraPosition.x = std::min(cameraPosition.x, static_cast<float>(window.width));
            cameraPosition.y = std::min(cameraPosition.y, static_cast<float>(window.height));

            break;

    }
}


int main() {
    float scalingFactor = 0.35;
    glm::vec3 cameraPosition(0.0, 0.0, 4.0);
    float focallength = 2.0;
    float delta = 0.1f;  // 移动或旋转的步长
    float orbitAngle = 0.0f;  // 相机围绕轨道中心旋转的当前角度
    float orbitRadius = 4.0f; // 相机离轨道中心的距离
    glm::vec3 orbitCenter(0.0f, 0.0f, 0.0f); // 假设轨道中心在世界坐标的原点

    std::vector<std::vector<float>> vertices;
    std::vector<std::vector<int>> faces;
    std::vector<std::tuple<int, int, int>> faceColours;

    auto materialMap = loadMTL("/Users/lil/Desktop/CG2023-main/RedNoise/cornell-box.mtl");
    loadOBJ("/Users/lil/Desktop/CG2023-main/RedNoise/cornell-box.obj", scalingFactor, vertices, faces, faceColours,
            materialMap);

    // Printing the triangle vertices
    for (const auto &face: faces) {
        std::cout << "Triangle:" << std::endl;
        for (size_t i = 0; i < face.size(); ++i) {
            const auto &vertex3D = vertices[face[i] - 1];  // OBJ indices start from 1
            std::cout << "Vertex " << i << " in 3D: [" << vertex3D[0] << ", " << vertex3D[1] << ", " << vertex3D[2]
                      << "]" << std::endl;

            CanvasPoint projectedPoint = getCanvasIntersection(cameraPosition,
                                                               glm::vec3(vertex3D[0], vertex3D[1], vertex3D[2]),
                                                               cameraOrientation, focallength);
            std::cout << "Vertex " << i << " projected in 2D: [" << projectedPoint.x << ", " << projectedPoint.y << "]"
                      << std::endl;
        }
        std::cout << std::endl;
    }
    uint32_t whiteColour = (255 << 16) + (255 << 8) + 255;  // RGB for white

    DrawingWindow window(WIDTH, HEIGHT, false);
    std::vector<std::vector<float>> depthBuffer(window.height, std::vector<float>(window.width,
                                                                                  std::numeric_limits<float>::infinity()));
    bool shouldRedraw = true;
    bool autoRotate = false;
    Uint32 lastTime = SDL_GetTicks();


    // 主循环，处理事件和更新渲染
    while (true) {
        // 处理所有事件
        SDL_Event event;
        while (window.pollForInputEvents(event)) {
            handleevent(event, window, cameraPosition, cameraOrientation, delta,autoRotate);
            shouldRedraw = true; // 事件处理可能更改了相机的位置或其他因素，因此我们设置重绘标志
        }

        // 自动更新旋转逻辑
        if (autoRotate) {
            Uint32 currentTime = SDL_GetTicks();
            float deltaTime = (currentTime - lastTime) / 1000.0f; // 转换为秒
            lastTime = currentTime;

            orbitAngle += deltaTime * 0.5f; // 每秒旋转0.5弧度
            cameraPosition.x = orbitCenter.x + orbitRadius * cos(orbitAngle);
            cameraPosition.z = orbitCenter.z + orbitRadius * sin(orbitAngle);

            // 基于新的cameraPosition重新计算cameraOrientation
            cameraOrientation = glm::mat3(glm::lookAt(cameraPosition, orbitCenter, glm::vec3(0, 1, 0)));

            // 设置重绘标志
            shouldRedraw = true;
        }


        // 如果发生了任何更改，需要重绘
        if (shouldRedraw) {
            window.clearPixels();
            depthBuffer = std::vector<std::vector<float>>(window.height, std::vector<float>(window.width,
                                                                                            std::numeric_limits<float>::infinity()));
            renderFilledTriangles(window, vertices, faces, faceColours, cameraPosition, focallength, depthBuffer);
            window.renderFrame();
            shouldRedraw = false; // 重置重绘标志
        }

        // 控制帧率，假设我们想要大约60fps的更新率
        SDL_Delay(16);
    }
}
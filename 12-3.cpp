/* 과제1번
#include <iostream>
#include "opencv2/opencv.hpp"
using namespace std;
using namespace cv;
int main(){
    Mat img = imread("beta.png", IMREAD_COLOR);
    if (img.empty()) {cerr << "Image load failed!" << endl;return -1;}
    Mat gray, bin;
    cvtColor(img, gray, COLOR_BGR2GRAY);// 그레이스케일 변환
    threshold(gray, bin, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);// 이진화
    vector<vector<Point>> contours;
    findContours(bin, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);// 외곽선 검출
    cvtColor(bin, img, COLOR_GRAY2BGR);
    for (const auto& contour : contours) {// 바운딩 박스, 최소 면적 사각형, 최소 면적 원 그리기
        int minX = INT_MAX, minY = INT_MAX, maxX = 0, maxY = 0;
        for (const auto& point : contour) {// 빨간색 바운딩 박스 그리기
            minX = min(minX, point.x);
            minY = min(minY, point.y);
            maxX = max(maxX, point.x);
            maxY = max(maxY, point.y);
        }
        rectangle(img, Point(minX, minY), Point(maxX, maxY), Scalar(0, 0, 255), 2);
        RotatedRect minRect = minAreaRect(contour);
        Point2f rectPoints[4];
        minRect.points(rectPoints);// 최소 면적 사각형 그리기 (파란색)
        for (int i = 0; i < 4; ++i) {line(img, rectPoints[i], rectPoints[(i + 1) % 4], Scalar(255, 0, 0), 2);}
        Point2f center;
        float radius;
        minEnclosingCircle(contour, center, radius);
        circle(img, center, cvRound(radius), Scalar(0, 255, 255), 2);// 최소 면적 원 그리기 (노란색)
    }
    imshow("Contours", img);
    waitKey(0);
    destroyAllWindows();
    return 0;
}
/*
beta.png 영 상 에 서 다음 처럼 바운딩박스(빨간색),
최소면적사각형(파란색, 70~72페이지 참고), 최소면적원(노란색)을 각각그리시오.

1. 이진화된 이미지에서 외곽선을 찾습니다
2. 각 외곽선에 대해 바운딩 박스, 최소 면적 사각형, 최소 면적 원을 그려줍니다.
3. 바운딩 박스는 외곽선을 감싸는 사각형을 그립니다. 이때 사각형의 모서리는 빨간색으로 표시됩니다.
4. 최소 면적 사각형은 외곽선을 가장 잘 표현하는 최소 크기의 회전된 사각형을 그립니다. 이때 사각형은 파란색으로 표시됩니다.
5. 최소 면적 원은 외곽선을 감싸는 최소 크기의 원을 그립니다. 이때 원은 노란색으로 표시됩니다.
*/
/* 과제2번
#include <iostream>
#include "opencv2/opencv.hpp"
using namespace std;
using namespace cv;
int main() {
    Mat img = imread("rotateda.bmp", IMREAD_COLOR);
    if (img.empty()) {cerr << "Image load failed!" << endl;return -1;}
    Mat gray, bin, rotated, cropped;
    cvtColor(img, gray, COLOR_BGR2GRAY);// 그레이스케일로 변환
    threshold(gray, bin, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);// 이진화 처리
    vector<vector<Point>> contours;
    findContours(bin, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);// 외곽선 찾기
    vector<Point> largestContour;
    double maxArea = 0;
    for (const auto& contour : contours) {// 가장 큰 외곽선 찾기
        double area = contourArea(contour);
        if (area > maxArea) {maxArea = area;largestContour = contour;}
    }
    RotatedRect minRect = minAreaRect(largestContour);// 최소 영역의 회전 사각형 계산
    Point2f center(img.cols / 2.0F, img.rows / 2.0F);// 원본 이미지의 중심점 계산
    Mat rot_mat = getRotationMatrix2D(center, minRect.angle, 1);// 중심점을 기준으로 회전 행렬 계산
    warpAffine(img, rotated, rot_mat, img.size(), INTER_CUBIC, BORDER_REPLICATE);// 전체 이미지를 회전하고 경계 부분을 복제
    getRectSubPix(rotated, img.size(), center, cropped);// 원본 크기로 결과 이미지를 잘라내기
    imshow("Original Image", img);
    imshow("Rotated Image", cropped);
    waitKey(0);
    destroyAllWindows();
    return 0;
}
/*
 rotateda.bmp 영상에서 문자의 기울어진 각도를 찾아 똑바로
보이도록수정하여출력하시오.
minAreaRect 함수와 RotatedRect 클래스(교재 70~72 페이지) ,
회전변환을이용하시오.

1. 이진화된 이미지에서 외곽선을 찾습니다.
2. 가장 큰 외곽선을 찾고, 해당 외곽선을 기준으로 이미지를 회전합니다.
3. 회전된 이미지를 원본 크기로 자릅니다.
4. 원본 이미지와 회전된 이미지를 화면에 표시합니다.

*/
/* 과제3번
#include <iostream>
#include "opencv2/opencv.hpp"
using namespace std;
using namespace cv;
int main() {
    Mat img = imread("shapes.png", IMREAD_COLOR);
    if (img.empty()) { cerr << "Image load failed!" << endl; return -1; }
    Mat gray, bin;
    cvtColor(img, gray, COLOR_BGR2GRAY);
    threshold(gray, bin, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);
    vector<vector<Point>> contours;
    findContours(bin, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);// 외곽선 검출
    int triangleCount = 0, rectangleCount = 0, pentagonCount = 0, hexagonCount = 0, circleCount = 0;
    for (const auto& contour : contours) {
        vector<Point> approx;
        approxPolyDP(contour, approx, arcLength(contour, true) * 0.02, true);// 외곽선 근사화 /  꼭지점 수
        int vtc = (int)approx.size(); // 근사화된 점의 갯수
        if (vtc == 3) triangleCount++;// 도형 구분 및 개수 카운트
        else if (vtc == 4) rectangleCount++;
        else if (vtc == 5) pentagonCount++;
        else if (vtc == 6) hexagonCount++;
        else {// 근사화된 결과가 원에 가까움
            double area = contourArea(contour);
            double perimeter = arcLength(contour, true);
            double circularity = 4 * CV_PI * area / (perimeter * perimeter);
            if (circularity > 0.85) { circleCount++; }
        }
        drawContours(img, vector<vector<Point>>{contour}, -1, Scalar(0, 0, 255), 2);// 외곽선 그리기
    }
    cout << "삼각형의 갯수: " << triangleCount << endl;
    cout << "사각형의 갯수: " << rectangleCount << endl;
    cout << "오각형의 갯수: " << pentagonCount << endl;
    cout << "육각형의 갯수: " << hexagonCount << endl;
    cout << "원의 갯수: " << circleCount << endl;
    imshow("img", img);
    waitKey(0);
    destroyAllWindows();
    return 0;
}
/*
 shapes.png 파일에서 삼각형, 사각형, 오각형, 육각형, 원의 개수를
각각구하여콘솔창에출력하고모든도형의외곽선을그리시오..

1. 이진화된 이미지에서 외곽선을 찾습니다.
2. 각 외곽선을 근사화하여 꼭지점 수를 계산합니다.
3. 근사화된 외곽선의 꼭지점 수에 따라 도형을 구분하고, 해당 도형의 개수를 세어줍니다.
꼭지점 수가 3이면 삼각형/ 4이면 사각형/ 5이면 오각형/ 6이면 육각형
그 외의 경우에는 원으로 판단하고, 원의 둘레와 면적을 이용하여 원을 구분합니다.
4.각 도형의 개수를 출력하고, 이미지에 외곽선을 그려서 표시합니다.
*/
/* 과제4번
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
int main() {
    Mat img = imread("polygon2.bmp", IMREAD_COLOR);
    if (img.empty()) {cerr << "이미지 로드 실패!" << endl;return -1;}
    Mat gray, bin;
    cvtColor(img, gray, COLOR_BGR2GRAY);// 그레이스케일로 변환
    threshold(gray, bin, 0, 255, THRESH_BINARY_INV | THRESH_OTSU); // 이진화 처리
    vector<vector<Point>> contours;
    findContours(bin, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);// 외곽선 찾기
    vector<Point> circleContour, squareContour, triangleContour;
    double circleArea = 0, squareArea = 0, triangleArea = 0;
    double circlePerimeter = 0, squarePerimeter = 0, trianglePerimeter = 0;
    for (const auto& contour : contours) {// 각 도형의 면적과 둘레 계산 및 외곽선 그리기
        double area = contourArea(contour);
        double perimeter = arcLength(contour, true);
        if (area > 1000) {  // 너무 작은 노이즈 제외
            RotatedRect minRect = minAreaRect(contour);
            Point2f rect_points[4];
            minRect.points(rect_points);
            vector<Point> approx;
            approxPolyDP(contour, approx, 0.04 * perimeter, true);
            if (approx.size() == 3) {// 도형 구분 및 정보 저장
                triangleContour = contour;
                triangleArea = area;
                trianglePerimeter = perimeter;
            }
            else if (approx.size() == 4) {
                squareContour = contour;
                squareArea = area;
                squarePerimeter = perimeter;
            }
            else {
                circleContour = contour;
                circleArea = area;
                circlePerimeter = perimeter;
            }
        }
    }
    drawContours(img, vector<vector<Point>>{circleContour}, -1, Scalar(0, 0, 255), 2);// 외곽선을 빨간색으로 그림
    drawContours(img, vector<vector<Point>>{squareContour}, -1, Scalar(0, 0, 255), 2);
    drawContours(img, vector<vector<Point>>{triangleContour}, -1, Scalar(0, 0, 255), 2);
    cout << "원의 둘레 길이: " << circlePerimeter << endl;
    cout << "원의 면적: " << circleArea << endl;
    cout << "삼각형의 둘레 길이: " << trianglePerimeter << endl;
    cout << "삼각형의 면적: " << triangleArea << endl;
    cout << "사각형의 둘레 길이: " << squarePerimeter << endl;
    cout << "사각형의 면적: " << squareArea << endl;
    imshow("Image with Contours", img);
    waitKey(0);
    destroyAllWindows();
    return 0;
}
/*
polygon2.bmp 파일에서 사각형, 원, 삼각형의 면적과 외곽선의
길이를구하여화면에출력하시오.

1. 이진화된 이미지에서 외곽선을 찾습니다.
2. 찾은 외곽선 중 너무 작은 노이즈는 제외하고, 나머지 도형의 면적과 둘레를 계산합니다.
3. 근사화된 외곽선의 꼭지점 수를 확인하여 도형을 구분하고, 해당 도형의 면적과 둘레를 저장합니다.
4. 각 도형의 외곽선을 빨간색으로 그리고, 각 도형의 면적과 둘레를 출력합니다.
5. 결과 이미지를 화면에 표시합니다.
*/
/*과제5번-1
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
int main() {
    Mat img = imread("ellipse.png", IMREAD_COLOR);
    if (img.empty()) {cerr << "이미지 로드 실패!" << endl;return -1;}
    Mat gray, bin;
    cvtColor(img, gray, COLOR_BGR2GRAY);// 그레이스케일로 변환
    threshold(gray, bin, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);// 이진화 처리
    vector<vector<Point>> contours;
    findContours(bin, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); // 외곽선 찾기
    drawContours(img, contours, -1, Scalar(0, 0, 255), 2);// 외곽선 그리기(빨간색)
    vector<Point> approx;
    approxPolyDP(contours[0], approx, arcLength(contours[0], true) * 0.02, true);// 외곽선 근사화
    double approxLength = arcLength(approx, true);// 근사화한 외곽선의 길이와 면적 계산
    double approxArea = contourArea(approx);
    vector<Point> approx_draw;
    for (size_t i = 0; i < approx.size(); i++) {approx_draw.push_back(Point(approx[i].x, approx[i].y));}// 근사화된 외곽선 그리기(파란색)
    polylines(img, approx_draw, true, Scalar(255, 0, 0), 2);
    cout << "외곽선의 길이:" << arcLength(contours[0], true) << endl << "외곽선의 면적:" << contourArea(contours[0]) << endl;
    cout << "근사화된 외곽선 길이:" << approxLength << endl << "근사화된 외곽선 면적:" << approxArea << endl;
    imshow("Contours", img);
    waitKey(0);
    destroyAllWindows();
    return 0;
}
/*
 ellipse.png 파일에서 외곽선을 검출하고 빨간색으로 그리시오.
그리고 외곽선을 근사화한 후 근사화한 외곽선은 파란색으로
그리시오. 외곽선의길이,면적을콘솔창에출력하시오.
 타원에더근접하게근사화하려면어떻게해야하는가?

1.이진화된 이미지에서 외곽선을 찾습니다.
2.외곽선을 빨간색으로 그립니다.
3.근사화하여 근사화된 외곽선을 파란색으로 그립니다.
원본 외곽선과 근사화된 외곽선의 길이와 면적을 계산하여 출력합니다.
결과 이미지를 화면에 표시합니다.
/* 과제5번-2*/
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
int main() {
    Mat img = imread("ellipse.png", IMREAD_COLOR);
    if (img.empty()) {cerr << "이미지 로드 실패!" << endl;return -1;}
    Mat gray, bin;
    cvtColor(img, gray, COLOR_BGR2GRAY);// 그레이스케일로 변환
    threshold(gray, bin, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);// 이진화 처리
    vector<vector<Point>> contours;
    findContours(bin, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);// 외곽선 찾기
    drawContours(img, contours, -1, Scalar(0, 0, 255), 2);// 외곽선 그리기(빨간색)
    vector<Point> approx;
    double epsilon = 0.005 * arcLength(contours[0], true); // 더 작은 epsilon 값 / 외곽선 근사화
    approxPolyDP(contours[0], approx, epsilon, true);
    double approxLength = arcLength(approx, true);// 근사화한 외곽선의 길이와 면적 계산
    double approxArea = contourArea(approx);
    vector<Point> approx_draw;
    for (size_t i = 0; i < approx.size(); i++) { approx_draw.push_back(Point(approx[i].x, approx[i].y)); }// 근사화된 외곽선 그리기(파란색)
    polylines(img, approx_draw, true, Scalar(255, 0, 0), 2);
    cout << "외곽선의 길이:" << arcLength(contours[0], true) << endl << "외곽선의 면적:" << contourArea(contours[0]) << endl;
    cout << "근사화된 외곽선 길이:" << approxLength << endl << "근사화된 외곽선 면적:" << approxArea << endl;
    imshow("Contours", img);
    waitKey(0);
    destroyAllWindows();
    return 0;
}
/*
더 작은 epsilon 값으로 타원에 더 근접하게 근사화 하였다.
*/
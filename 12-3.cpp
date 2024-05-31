/* ����1��
#include <iostream>
#include "opencv2/opencv.hpp"
using namespace std;
using namespace cv;
int main(){
    Mat img = imread("beta.png", IMREAD_COLOR);
    if (img.empty()) {cerr << "Image load failed!" << endl;return -1;}
    Mat gray, bin;
    cvtColor(img, gray, COLOR_BGR2GRAY);// �׷��̽����� ��ȯ
    threshold(gray, bin, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);// ����ȭ
    vector<vector<Point>> contours;
    findContours(bin, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);// �ܰ��� ����
    cvtColor(bin, img, COLOR_GRAY2BGR);
    for (const auto& contour : contours) {// �ٿ�� �ڽ�, �ּ� ���� �簢��, �ּ� ���� �� �׸���
        int minX = INT_MAX, minY = INT_MAX, maxX = 0, maxY = 0;
        for (const auto& point : contour) {// ������ �ٿ�� �ڽ� �׸���
            minX = min(minX, point.x);
            minY = min(minY, point.y);
            maxX = max(maxX, point.x);
            maxY = max(maxY, point.y);
        }
        rectangle(img, Point(minX, minY), Point(maxX, maxY), Scalar(0, 0, 255), 2);
        RotatedRect minRect = minAreaRect(contour);
        Point2f rectPoints[4];
        minRect.points(rectPoints);// �ּ� ���� �簢�� �׸��� (�Ķ���)
        for (int i = 0; i < 4; ++i) {line(img, rectPoints[i], rectPoints[(i + 1) % 4], Scalar(255, 0, 0), 2);}
        Point2f center;
        float radius;
        minEnclosingCircle(contour, center, radius);
        circle(img, center, cvRound(radius), Scalar(0, 255, 255), 2);// �ּ� ���� �� �׸��� (�����)
    }
    imshow("Contours", img);
    waitKey(0);
    destroyAllWindows();
    return 0;
}
/*
beta.png �� �� �� �� ���� ó�� �ٿ���ڽ�(������),
�ּҸ����簢��(�Ķ���, 70~72������ ����), �ּҸ�����(�����)�� �����׸��ÿ�.

1. ����ȭ�� �̹������� �ܰ����� ã���ϴ�
2. �� �ܰ����� ���� �ٿ�� �ڽ�, �ּ� ���� �簢��, �ּ� ���� ���� �׷��ݴϴ�.
3. �ٿ�� �ڽ��� �ܰ����� ���δ� �簢���� �׸��ϴ�. �̶� �簢���� �𼭸��� ���������� ǥ�õ˴ϴ�.
4. �ּ� ���� �簢���� �ܰ����� ���� �� ǥ���ϴ� �ּ� ũ���� ȸ���� �簢���� �׸��ϴ�. �̶� �簢���� �Ķ������� ǥ�õ˴ϴ�.
5. �ּ� ���� ���� �ܰ����� ���δ� �ּ� ũ���� ���� �׸��ϴ�. �̶� ���� ��������� ǥ�õ˴ϴ�.
*/
/* ����2��
#include <iostream>
#include "opencv2/opencv.hpp"
using namespace std;
using namespace cv;
int main() {
    Mat img = imread("rotateda.bmp", IMREAD_COLOR);
    if (img.empty()) {cerr << "Image load failed!" << endl;return -1;}
    Mat gray, bin, rotated, cropped;
    cvtColor(img, gray, COLOR_BGR2GRAY);// �׷��̽����Ϸ� ��ȯ
    threshold(gray, bin, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);// ����ȭ ó��
    vector<vector<Point>> contours;
    findContours(bin, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);// �ܰ��� ã��
    vector<Point> largestContour;
    double maxArea = 0;
    for (const auto& contour : contours) {// ���� ū �ܰ��� ã��
        double area = contourArea(contour);
        if (area > maxArea) {maxArea = area;largestContour = contour;}
    }
    RotatedRect minRect = minAreaRect(largestContour);// �ּ� ������ ȸ�� �簢�� ���
    Point2f center(img.cols / 2.0F, img.rows / 2.0F);// ���� �̹����� �߽��� ���
    Mat rot_mat = getRotationMatrix2D(center, minRect.angle, 1);// �߽����� �������� ȸ�� ��� ���
    warpAffine(img, rotated, rot_mat, img.size(), INTER_CUBIC, BORDER_REPLICATE);// ��ü �̹����� ȸ���ϰ� ��� �κ��� ����
    getRectSubPix(rotated, img.size(), center, cropped);// ���� ũ��� ��� �̹����� �߶󳻱�
    imshow("Original Image", img);
    imshow("Rotated Image", cropped);
    waitKey(0);
    destroyAllWindows();
    return 0;
}
/*
 rotateda.bmp ���󿡼� ������ ������ ������ ã�� �ȹٷ�
���̵��ϼ����Ͽ�����Ͻÿ�.
minAreaRect �Լ��� RotatedRect Ŭ����(���� 70~72 ������) ,
ȸ����ȯ���̿��Ͻÿ�.

1. ����ȭ�� �̹������� �ܰ����� ã���ϴ�.
2. ���� ū �ܰ����� ã��, �ش� �ܰ����� �������� �̹����� ȸ���մϴ�.
3. ȸ���� �̹����� ���� ũ��� �ڸ��ϴ�.
4. ���� �̹����� ȸ���� �̹����� ȭ�鿡 ǥ���մϴ�.

*/
/* ����3��
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
    findContours(bin, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);// �ܰ��� ����
    int triangleCount = 0, rectangleCount = 0, pentagonCount = 0, hexagonCount = 0, circleCount = 0;
    for (const auto& contour : contours) {
        vector<Point> approx;
        approxPolyDP(contour, approx, arcLength(contour, true) * 0.02, true);// �ܰ��� �ٻ�ȭ /  ������ ��
        int vtc = (int)approx.size(); // �ٻ�ȭ�� ���� ����
        if (vtc == 3) triangleCount++;// ���� ���� �� ���� ī��Ʈ
        else if (vtc == 4) rectangleCount++;
        else if (vtc == 5) pentagonCount++;
        else if (vtc == 6) hexagonCount++;
        else {// �ٻ�ȭ�� ����� ���� �����
            double area = contourArea(contour);
            double perimeter = arcLength(contour, true);
            double circularity = 4 * CV_PI * area / (perimeter * perimeter);
            if (circularity > 0.85) { circleCount++; }
        }
        drawContours(img, vector<vector<Point>>{contour}, -1, Scalar(0, 0, 255), 2);// �ܰ��� �׸���
    }
    cout << "�ﰢ���� ����: " << triangleCount << endl;
    cout << "�簢���� ����: " << rectangleCount << endl;
    cout << "�������� ����: " << pentagonCount << endl;
    cout << "�������� ����: " << hexagonCount << endl;
    cout << "���� ����: " << circleCount << endl;
    imshow("img", img);
    waitKey(0);
    destroyAllWindows();
    return 0;
}
/*
 shapes.png ���Ͽ��� �ﰢ��, �簢��, ������, ������, ���� ������
�������Ͽ��ܼ�â������ϰ��絵���ǿܰ������׸��ÿ�..

1. ����ȭ�� �̹������� �ܰ����� ã���ϴ�.
2. �� �ܰ����� �ٻ�ȭ�Ͽ� ������ ���� ����մϴ�.
3. �ٻ�ȭ�� �ܰ����� ������ ���� ���� ������ �����ϰ�, �ش� ������ ������ �����ݴϴ�.
������ ���� 3�̸� �ﰢ��/ 4�̸� �簢��/ 5�̸� ������/ 6�̸� ������
�� ���� ��쿡�� ������ �Ǵ��ϰ�, ���� �ѷ��� ������ �̿��Ͽ� ���� �����մϴ�.
4.�� ������ ������ ����ϰ�, �̹����� �ܰ����� �׷��� ǥ���մϴ�.
*/
/* ����4��
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
int main() {
    Mat img = imread("polygon2.bmp", IMREAD_COLOR);
    if (img.empty()) {cerr << "�̹��� �ε� ����!" << endl;return -1;}
    Mat gray, bin;
    cvtColor(img, gray, COLOR_BGR2GRAY);// �׷��̽����Ϸ� ��ȯ
    threshold(gray, bin, 0, 255, THRESH_BINARY_INV | THRESH_OTSU); // ����ȭ ó��
    vector<vector<Point>> contours;
    findContours(bin, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);// �ܰ��� ã��
    vector<Point> circleContour, squareContour, triangleContour;
    double circleArea = 0, squareArea = 0, triangleArea = 0;
    double circlePerimeter = 0, squarePerimeter = 0, trianglePerimeter = 0;
    for (const auto& contour : contours) {// �� ������ ������ �ѷ� ��� �� �ܰ��� �׸���
        double area = contourArea(contour);
        double perimeter = arcLength(contour, true);
        if (area > 1000) {  // �ʹ� ���� ������ ����
            RotatedRect minRect = minAreaRect(contour);
            Point2f rect_points[4];
            minRect.points(rect_points);
            vector<Point> approx;
            approxPolyDP(contour, approx, 0.04 * perimeter, true);
            if (approx.size() == 3) {// ���� ���� �� ���� ����
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
    drawContours(img, vector<vector<Point>>{circleContour}, -1, Scalar(0, 0, 255), 2);// �ܰ����� ���������� �׸�
    drawContours(img, vector<vector<Point>>{squareContour}, -1, Scalar(0, 0, 255), 2);
    drawContours(img, vector<vector<Point>>{triangleContour}, -1, Scalar(0, 0, 255), 2);
    cout << "���� �ѷ� ����: " << circlePerimeter << endl;
    cout << "���� ����: " << circleArea << endl;
    cout << "�ﰢ���� �ѷ� ����: " << trianglePerimeter << endl;
    cout << "�ﰢ���� ����: " << triangleArea << endl;
    cout << "�簢���� �ѷ� ����: " << squarePerimeter << endl;
    cout << "�簢���� ����: " << squareArea << endl;
    imshow("Image with Contours", img);
    waitKey(0);
    destroyAllWindows();
    return 0;
}
/*
polygon2.bmp ���Ͽ��� �簢��, ��, �ﰢ���� ������ �ܰ�����
���̸����Ͽ�ȭ�鿡����Ͻÿ�.

1. ����ȭ�� �̹������� �ܰ����� ã���ϴ�.
2. ã�� �ܰ��� �� �ʹ� ���� ������� �����ϰ�, ������ ������ ������ �ѷ��� ����մϴ�.
3. �ٻ�ȭ�� �ܰ����� ������ ���� Ȯ���Ͽ� ������ �����ϰ�, �ش� ������ ������ �ѷ��� �����մϴ�.
4. �� ������ �ܰ����� ���������� �׸���, �� ������ ������ �ѷ��� ����մϴ�.
5. ��� �̹����� ȭ�鿡 ǥ���մϴ�.
*/
/*����5��-1
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
int main() {
    Mat img = imread("ellipse.png", IMREAD_COLOR);
    if (img.empty()) {cerr << "�̹��� �ε� ����!" << endl;return -1;}
    Mat gray, bin;
    cvtColor(img, gray, COLOR_BGR2GRAY);// �׷��̽����Ϸ� ��ȯ
    threshold(gray, bin, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);// ����ȭ ó��
    vector<vector<Point>> contours;
    findContours(bin, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); // �ܰ��� ã��
    drawContours(img, contours, -1, Scalar(0, 0, 255), 2);// �ܰ��� �׸���(������)
    vector<Point> approx;
    approxPolyDP(contours[0], approx, arcLength(contours[0], true) * 0.02, true);// �ܰ��� �ٻ�ȭ
    double approxLength = arcLength(approx, true);// �ٻ�ȭ�� �ܰ����� ���̿� ���� ���
    double approxArea = contourArea(approx);
    vector<Point> approx_draw;
    for (size_t i = 0; i < approx.size(); i++) {approx_draw.push_back(Point(approx[i].x, approx[i].y));}// �ٻ�ȭ�� �ܰ��� �׸���(�Ķ���)
    polylines(img, approx_draw, true, Scalar(255, 0, 0), 2);
    cout << "�ܰ����� ����:" << arcLength(contours[0], true) << endl << "�ܰ����� ����:" << contourArea(contours[0]) << endl;
    cout << "�ٻ�ȭ�� �ܰ��� ����:" << approxLength << endl << "�ٻ�ȭ�� �ܰ��� ����:" << approxArea << endl;
    imshow("Contours", img);
    waitKey(0);
    destroyAllWindows();
    return 0;
}
/*
 ellipse.png ���Ͽ��� �ܰ����� �����ϰ� ���������� �׸��ÿ�.
�׸��� �ܰ����� �ٻ�ȭ�� �� �ٻ�ȭ�� �ܰ����� �Ķ�������
�׸��ÿ�. �ܰ����Ǳ���,�������ܼ�â������Ͻÿ�.
 Ÿ�����������ϰԱٻ�ȭ�Ϸ������ؾ��ϴ°�?

1.����ȭ�� �̹������� �ܰ����� ã���ϴ�.
2.�ܰ����� ���������� �׸��ϴ�.
3.�ٻ�ȭ�Ͽ� �ٻ�ȭ�� �ܰ����� �Ķ������� �׸��ϴ�.
���� �ܰ����� �ٻ�ȭ�� �ܰ����� ���̿� ������ ����Ͽ� ����մϴ�.
��� �̹����� ȭ�鿡 ǥ���մϴ�.
/* ����5��-2*/
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
int main() {
    Mat img = imread("ellipse.png", IMREAD_COLOR);
    if (img.empty()) {cerr << "�̹��� �ε� ����!" << endl;return -1;}
    Mat gray, bin;
    cvtColor(img, gray, COLOR_BGR2GRAY);// �׷��̽����Ϸ� ��ȯ
    threshold(gray, bin, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);// ����ȭ ó��
    vector<vector<Point>> contours;
    findContours(bin, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);// �ܰ��� ã��
    drawContours(img, contours, -1, Scalar(0, 0, 255), 2);// �ܰ��� �׸���(������)
    vector<Point> approx;
    double epsilon = 0.005 * arcLength(contours[0], true); // �� ���� epsilon �� / �ܰ��� �ٻ�ȭ
    approxPolyDP(contours[0], approx, epsilon, true);
    double approxLength = arcLength(approx, true);// �ٻ�ȭ�� �ܰ����� ���̿� ���� ���
    double approxArea = contourArea(approx);
    vector<Point> approx_draw;
    for (size_t i = 0; i < approx.size(); i++) { approx_draw.push_back(Point(approx[i].x, approx[i].y)); }// �ٻ�ȭ�� �ܰ��� �׸���(�Ķ���)
    polylines(img, approx_draw, true, Scalar(255, 0, 0), 2);
    cout << "�ܰ����� ����:" << arcLength(contours[0], true) << endl << "�ܰ����� ����:" << contourArea(contours[0]) << endl;
    cout << "�ٻ�ȭ�� �ܰ��� ����:" << approxLength << endl << "�ٻ�ȭ�� �ܰ��� ����:" << approxArea << endl;
    imshow("Contours", img);
    waitKey(0);
    destroyAllWindows();
    return 0;
}
/*
�� ���� epsilon ������ Ÿ���� �� �����ϰ� �ٻ�ȭ �Ͽ���.
*/
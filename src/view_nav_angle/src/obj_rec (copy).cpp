#include <iostream>

int main(int argc, char**argv) {

    std::system("./darknet detect cfg/yolov3.cfg yolov3.weights data/dog.jpg");

  return 0;
}

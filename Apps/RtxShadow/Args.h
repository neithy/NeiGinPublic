#pragma once
#include <string>


struct Args {
public:
  void init(int argc, char** argv);

  int w = 1920;
  int h = 1080;
  std::string model = "models/sponza.obj";
  std::string flythrough = "models/sponza_fly.txt";
  float speed = 2;
  int frames = 0;
  float renderScale = 1;
  int avgFrames = 1;
  int bvh = 0;
  std::string log;
};
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <array>
#include <matplotlibcpp.h>
#include <Eigen/Dense>
#include <OsqpEigen/OsqpEigen.h>
#include <OsqpEigen/Solver.hpp>

namespace plt = matplotlibcpp;

typedef std::pair<double, double> POINT;
typedef std::vector<std::array<double, 3>> REF_LINE;
typedef std::vector<POINT> LINE;


LINE data_from_file(std::string path) {
  std::ifstream file(path);
  std::string line;
  LINE ans;

  if (file.is_open()) {
    if (std::getline(file, line)) {
      line = line.substr(1, line.size() - 3);
      std::stringstream ss(line);
      // 使用一变量过滤文件中的[]符号
      char ignore;
      while (ss >> ignore) {
        double first, second;
        if (ss >> first >> ignore >> second >> ignore) {
          ans.emplace_back(std::make_pair(first, second));
        }
        ss >> ignore;
      }
    }
    file.close();
  } else {
    std::cerr << "文件无法打开" << std::endl;
  }
  //  for (auto data : ans) {
  //    std::cout << data.first << " " << data.second << std::endl;
  //  }
  return ans;
}

LINE line_interpolation(const LINE &line_raw) {
  LINE interpolated_line;
  interpolated_line.emplace_back(line_raw.front());
  for (size_t i = 1; i < line_raw.size() - 2; ++i) {
    const auto &p1 = line_raw[i - 1];
    const auto &p2 = line_raw[i];
    const auto &p3 = line_raw[i + 1];
    interpolated_line.emplace_back((p1.first+p2.first+p3.first)/3.0, (p1.second+p2.second+p3.second)/3.0);
  }
  interpolated_line.emplace_back(line_raw.back());
  return interpolated_line;
}


void line_diff_viz(const LINE &line_raw1,
                   const LINE &line2,
                   std::string line_shape = "r-",
                   std::string line_shape2 = "b-") {
  std::vector<double> x1, y1, x2, y2;
  for (auto point : line_raw1) {
    x1.emplace_back(point.first);
    y1.emplace_back(point.second);
  }
  for (auto point : line2) {
    x2.emplace_back(point.first);
    y2.emplace_back(point.second);
  }

  plt::plot(x1, y1, line_shape);
  plt::plot(x2, y2, line_shape2);
  plt::show();
}


int main() {
  //插值步长
  double ds = 0.1;
  //平滑参数
  double dx = 1;
  double dy = 1;
  double dtheta = 0.1;

  std::string path1 = "../route1.txt";
  std::string path2 = "../route2.txt";
  std::string tarj1 = "../points1.txt";
  std::string tarj2 = "../points2.txt";

  // LINE line_raw1 = data_from_file(path1);
  // LINE line_raw2 = data_from_file(path2);
  LINE traj_raw1 = data_from_file(tarj1);
  LINE traj_raw2 = data_from_file(tarj2);

  LINE traj_inter1 = line_interpolation(traj_raw1);
  LINE traj_inter2 = line_interpolation(traj_raw2);

  line_diff_viz(traj_raw1,traj_inter1);
  line_diff_viz(traj_raw2,traj_inter2);
  return 0;
}
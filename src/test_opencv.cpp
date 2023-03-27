#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <boost/filesystem.hpp>
#include <opencv2/opencv.hpp>
using namespace std;
namespace fs = boost::filesystem;
// 计算两个Mat对象的像素值之差的绝对值的平均值
double calc_mat_diff(const cv::Mat& mat1, const cv::Mat& mat2)
{
    cv::Mat diff;
    cv::absdiff(mat1, mat2, diff);
    cv::Scalar diff_mean = cv::mean(diff);
    double diff_mean_val = (diff_mean[0] + diff_mean[1] + diff_mean[2]) / 3;
    return diff_mean_val;
}
// 判断两个图片文件是否相似
bool is_image_similar(const string& file_path1, const string& file_path2,
                      double threshold = 10.0)
{
    cv::Mat img1 = cv::imread(file_path1);
    cv::Mat img2 = cv::imread(file_path2);
    if (img1.empty() || img2.empty()) {
        cerr << "Failed to read image file: " << file_path1 << " or " << file_path2 << endl;
        return false;
    }
    double diff_val = calc_mat_diff(img1, img2);
    return (diff_val < threshold);
}
int main(int argc, char* argv[])
{
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " folder_path" << endl;
        return 1;
    }
    string folder_path = argv[1];
    // 遍历文件夹中的所有图片文件，并比较其像素值
    unordered_map<string, string> hash_map;
    vector<string> dup_files;
    for (fs::recursive_directory_iterator it(folder_path);
         it != fs::recursive_directory_iterator(); ++it) {
        if (fs::is_regular_file(*it) && it->path().extension() == ".jpg") {
            string file_path = it->path().string();
            bool is_dup = false;
            for (const auto& kv : hash_map) {
                if (is_image_similar(file_path, kv.second)) {
                    // 如果两个图片文件相似，则说明该图片文件与之前的某个图片文件重复
                    is_dup = true;
                    dup_files.push_back(file_path);
                    break;
                }
            }
            if (!is_dup) {
                // 否则，将该图片文件的文件名添加到哈希表中
                hash_map.emplace(file_path, file_path);
            }
        }
    }
    // 输出重复的图片文件名
    if (!dup_files.empty()) {
        cout << "Duplicate files:" << endl;
        for (const auto& file_path : dup_files) {
            cout << file_path << endl;
        }
    } else {
        cout << "No duplicate files found." << endl;
    }
    return 0;
}

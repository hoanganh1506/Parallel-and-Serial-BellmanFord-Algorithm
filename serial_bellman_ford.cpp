/*
g++ -std=c++11 -o serial_bellman_ford serial_bellman_ford.cpp
./serial_bellman_ford <input file>
*/

#include <string>
#include <cassert>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sys/time.h>

using std::string;
using std::cout;
using std::endl;

#define INF 1000000

/*
 * utils is a namespace for utility functions
 * bao gồm I/O (đọc tệp đầu vào và in kết quả) và hàm chuyển đổi kích thước ma trận (2D->1D) 
 */
namespace utils {
    int N;  //số lượng đỉnh 
    int *mat; //ma trận kề 

    void abort_with_error_message(string msg) {
        std::cerr << msg << endl;
        abort();
    }

    //chuyển đổi tọa độ 2 chiều thành tọa độ 1 chiều
    int convert_dimension_2D_1D(int x, int y, int n) {
        return x * n + y;
    }

    int read_file(string filename) {
        std::ifstream inputf(filename, std::ifstream::in);
        if (!inputf.good()) {
            abort_with_error_message("ERROR OCCURRED WHILE READING INPUT FILE");
        }
        inputf >> N;
        // Ma trận đầu vào nên có kích thước nhỏ hơn 20MB * 20MB (400MB) để tránh sử dụng quá nhiều bộ nhớ trên nhiều bộ xử lý.
        assert(N < (1024 * 1024 * 20));
        mat = (int *) malloc(N * N * sizeof(int));
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++) {
                inputf >> mat[convert_dimension_2D_1D(i, j, N)];
            }
        return 0;
    }

    int print_result(bool has_negative_cycle, int *dist) {
        std::ofstream outputf("output.txt", std::ofstream::out);
        if (!has_negative_cycle) {
            for (int i = 0; i < N; i++) {
                if (dist[i] > INF)
                    dist[i] = INF;
                outputf << dist[i] << '\n';
            }
            outputf.flush();
        } else {
            outputf << "FOUND NEGATIVE CYCLE!" << endl;
        }
        outputf.close();
        return 0;
    }
}//namespace utils

/*
 * Thuật toán Bellman-Ford. `has_shortest_path` sẽ được đặt thành false nếu phát hiện chu trình âm
 */
void bellman_ford(int n, int *mat, int *dist, bool *has_negative_cycle) {
    // Khởi tạo kết quả
    *has_negative_cycle = false;
    for (int i = 0; i < n; i++) {
        dist[i] = INF;
    }
    // Đỉnh gốc luôn có khoảng cách là 0
    dist[0] = 0;
    // has_change check xem có thay đổi khoảng cách nào trong vòng lặp này hay không
    bool has_change;
    for (int i = 0; i < n - 1; i++) {
        has_change = false;
        for (int u = 0; u < n; u++) {
            for (int v = 0; v < n; v++) {
                int weight = mat[utils::convert_dimension_2D_1D(u, v, n)];
                if (weight < INF) { 
                    //Kiểm tra xem có cạnh giữa đỉnh u và v không
                    if (dist[u] + weight < dist[v]) {
                        has_change = true;
                        dist[v] = dist[u] + weight;
                    }
                }
            }
        }
        // Nếu không có sự thay đổi trong vòng lặp
        if(!has_change) {
            return;
        }
    }
    // Thực hiện thêm một lần lặp để kiểm tra chu trình âm
    for (int u = 0; u < n; u++) {
        for (int v = 0; v < n; v++) {
            int weight = mat[utils::convert_dimension_2D_1D(u, v, n)];
            if (weight < INF) {
                if (dist[u] + weight < dist[v]) { // if we can relax one more step /*Nếu chúng ta có thể thư giãn thêm một bước, thì chúng ta đã tìm thấy một chu trình âm*/
                    *has_negative_cycle = true;
                    return;
                }
            }
        }
    }
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        utils::abort_with_error_message("INPUT FILE WAS NOT FOUND!");
    }
    string filename = argv[1];
    assert(utils::read_file(filename) == 0);

    int *dist;
    bool has_negative_cycle;

    dist = (int *) malloc(sizeof(int) * utils::N);

    //Khởi tạo bộ đếm thời gian
    timeval start_wall_time_t, end_wall_time_t;
    float ms_wall;

    //Bắt đầu đếm
    gettimeofday(&start_wall_time_t, nullptr);

    //bellman ford algorithm
    bellman_ford(utils::N, utils::mat, dist, &has_negative_cycle);

    //Kết thúc đếm
    gettimeofday(&end_wall_time_t, nullptr);
    ms_wall = ((end_wall_time_t.tv_sec - start_wall_time_t.tv_sec) * 1000 * 1000
               + end_wall_time_t.tv_usec - start_wall_time_t.tv_usec) / 1000.0;

    std::cerr << "Time(s): " << ms_wall/1000.0 << endl;
    utils::print_result(has_negative_cycle, dist);

    free(dist);
    free(utils::mat);
    return 0;
}
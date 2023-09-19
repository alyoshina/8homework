#pragma once

#include <map>
#include <vector>
#include <mutex>
#include <thread>

namespace MultiThread {

using Counter = std::map<std::string, std::size_t>;

struct Result {
    size_t us;
    Counter freq_dict;
    std::mutex freq_dict_mutex;
};

class FilesVector {

public:
    FilesVector();
    ~FilesVector();

    bool get_next_name(std::string& file_name, std::size_t& pos);
    std::size_t get_size();
    void push_back(const char* file_name);
    void reset_pos();
    void reset_error();
    bool is_error();
    void print_error();
    void set_error(const int pos, std::string& error_str);

private:
    std::vector<std::string> files;
    std::mutex files_mutex;
    std::size_t cur_pos {0};

    std::map<int, std::vector<std::string>> error;
    std::mutex error_mutex;
    std::string get_file_name(const std::size_t pos);
};

void count_words(FilesVector& files_vector, Result& result);
std::string tolower(const std::string &str);
bool run(FilesVector& files_vector, Result& result, int thread_number);

};
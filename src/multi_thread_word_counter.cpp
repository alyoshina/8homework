#include "multi_thread_word_counter.h"

#include <fstream>
#include <iostream>
#include <iterator>
#include <algorithm>

namespace MultiThread {

std::vector<std::thread> threads;

FilesVector::FilesVector() {

}

FilesVector::~FilesVector() {
    files.clear();
    error.clear();
}

bool FilesVector::get_next_name(std::string& file_name, std::size_t &pos) {
    std::lock_guard<std::mutex> guard(files_mutex);
    if (cur_pos < files.size()) {
        file_name = files.at(cur_pos);
        pos = cur_pos;
        cur_pos++;
        return true;
    }
    return false;
}

std::size_t FilesVector::get_size() {
    std::lock_guard<std::mutex> guard(files_mutex);
    return files.size();
}

void FilesVector::push_back(const char* file_name) {
    std::lock_guard<std::mutex> guard(files_mutex);
    files.emplace_back(file_name);
}

void FilesVector::reset_pos() {
    std::lock_guard<std::mutex> guard(files_mutex);
    cur_pos = 0;
}

void FilesVector::reset_error() {
    std::lock_guard<std::mutex> guard(error_mutex);
    error.clear();
}

void FilesVector::set_error(const int pos, std::string& error_str) {
    std::lock_guard<std::mutex> guard(error_mutex);
    auto it = error.find(pos);
    if (it == error.end()) {
        error[pos] = std::vector<std::string> {error_str};
    } else {
        it->second.emplace_back(error_str);
    }    
}

bool FilesVector::is_error() {
    std::lock_guard<std::mutex> guard(error_mutex);
    return error.size() != 0;
}

void FilesVector::print_error() {
    std::lock_guard<std::mutex> guard(error_mutex);
    for (const auto& [key, value] : error) {
        std::cout << get_file_name(key) << ":\n";
        for (auto& str : value) {
            std::cout << str << std::endl;
        }
    }
}

std::string FilesVector::get_file_name(std::size_t pos) {
    std::string name;
    std::lock_guard<std::mutex> guard(files_mutex);
    if (pos < files.size()) {
        name = files.at(pos);
    }
    return name;
}

void count_words(FilesVector& files_vector, Result& result) {
    Counter counter;
    std::string file_name;
    std::size_t pos;
    while (files_vector.get_next_name(file_name, pos)) {
        std::ifstream input{file_name};
        if (!input.is_open()) {
            std::string error_str("Failed to open file ");
            error_str += file_name;
            files_vector.set_error(pos, error_str);
            return;
        }
        //count
        std::for_each(std::istream_iterator<std::string>(input),
                  std::istream_iterator<std::string>(),
                  [&counter](const std::string &s) { ++counter[tolower(s)]; });  
    }

    //merge
    {
        std::lock_guard<std::mutex> guard(result.freq_dict_mutex);
        for (auto& it: counter) {
            result.freq_dict[it.first] = result.freq_dict[it.first] + it.second;
        }
    }
}

std::string tolower(const std::string &str) {
    std::string lower_str;
    std::transform(std::cbegin(str), std::cend(str),
                   std::back_inserter(lower_str),
                   [](unsigned char ch) { return std::tolower(ch); });
    return lower_str;
};

bool run(FilesVector& files_vector, Result& result, const int thread_number) {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < thread_number; ++i) {
        threads.emplace_back(count_words, std::ref(files_vector), std::ref(result));
    }
    for (auto& thread: threads) {
        thread.join();
    }
    threads.clear();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    result.us = elapsed_ms.count();

    return true;
}
};
#include "multi_thread_word_counter.h"

#include <iostream>
#include <iterator>
#include <algorithm>
#include <iomanip>

const size_t TOPK = 10;
const size_t STARTS_NUMBER = 5;
const size_t THREADS_NUMBER = 3;

using MultiThread::Counter;

void print_topk(std::ostream& stream, const Counter& counter, const size_t k) {
    std::vector<Counter::const_iterator> words;
    words.reserve(counter.size());
    for (auto it = std::cbegin(counter); it != std::cend(counter); ++it) {
        words.push_back(it);
    }

    std::partial_sort(
        std::begin(words), std::begin(words) + k, std::end(words),
        [](auto lhs, auto &rhs) { return lhs->second > rhs->second; });

    std::for_each(
        std::begin(words), std::begin(words) + k,
        [&stream](const Counter::const_iterator &pair) {
            stream << std::setw(4) << pair->second << " " << pair->first
                      << '\n';
        });
}

size_t thread_test(const int argc, char **argv, const int thread_number, bool& is_ok, const char *str) {
    is_ok = true;
    MultiThread::FilesVector files_vector;
    for (int i = 1; i < argc; ++i) {
        files_vector.push_back(argv[i]);
    }
    MultiThread::Result thread_result;
    MultiThread::run(files_vector, thread_result, thread_number);
    if (files_vector.is_error()) {
        files_vector.print_error();
        is_ok = false;
        return 0;
    }
    if (str) {
        print_topk(std::cout, thread_result.freq_dict, TOPK);
        std::cout << str << thread_result.us << " us" << std::endl;
    }
    return thread_result.us;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: counter [FILES...]\n";
        return EXIT_FAILURE;
    }

    std::vector<size_t> single_thread;
    std::vector<size_t> multi_thread;
    for (int i = 0; i < STARTS_NUMBER; ++i) {
        std::cout << i + 1 << " start" << std::endl;
        bool is_ok;
        single_thread.emplace_back(thread_test(argc, argv, 1, is_ok
                                        , "Elapsed time of single thread is "));
        if (!is_ok) {
            return EXIT_FAILURE;
        }
        multi_thread.emplace_back(thread_test(argc, argv, THREADS_NUMBER, is_ok
                                                , "Elapsed time of multi thread is "));
        if (!is_ok) {
            return EXIT_FAILURE;
        }
        std::cout << std::endl;
    }

    auto s_it = std::min_element(single_thread.begin(), single_thread.end());
    auto m_it = std::min_element(multi_thread.begin(), multi_thread.end());

    std::cout << "Number of starts is " << STARTS_NUMBER << std::endl;
    std::cout << "Minimum execution time of single thread calc is " << *s_it << " us" << std::endl;
    std::cout << "Minimum execution time of multi thread (" << THREADS_NUMBER
                    << " threads) calc is " << *m_it << " us" << std::endl;
}

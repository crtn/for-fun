#include <iostream>
#include <iomanip>
#include <random>
#include <chrono>
#include <vector>
#include <algorithm>
#include <iterator>
#include <functional>

template<typename RandomAccessIterator>
void insertionSort(RandomAccessIterator begin, RandomAccessIterator end) {
    for(auto it0 = begin; it0 < end; ++it0) {
        for(auto it1 = it0; it1 > begin; --it1) {
            if(*it1 < *(it1 - 1)) {
                using std::swap;
                swap(*it1, *(it1 - 1));
            } else {
                break;
            }
        }
    }
}

template<typename RandomAccessIterator>
void selectionSort(RandomAccessIterator begin, RandomAccessIterator end) {
    for(auto it0 = begin; it0 < end; ++it0) {
        auto it_min = it0;
        for(auto it1 = it0 + 1; it1 < end; ++it1) {
            if(*it1 < *it_min) {
                it_min = it1;
            }
        }
        using std::swap;
        swap(*it0, *it_min);
    }
}

template<typename RandomAccessIterator>
void mergeSort(RandomAccessIterator begin, RandomAccessIterator end) {
    if(end <= begin + 1) return;
    auto mid = begin + (end - begin) / 2;
    mergeSort(begin, mid);
    mergeSort(mid, end);
    std::vector<typename RandomAccessIterator::value_type> buffer(end - begin);
    std::copy(begin, end, buffer.begin());
    auto p = buffer.begin(), q = buffer.begin() + (mid - begin), r = q;
    for(auto it = begin; it < end; ++it) {
        if(p >= r) *it = *q++;
        else if(q >= buffer.end()) *it = *p++;
        else if(*p <= *q) *it = *p++;
        else *it = *q++;
    }
}

template<typename RandomAccessIterator>
void quickSort(RandomAccessIterator begin, RandomAccessIterator end) {
    if(end <= begin + 1) return;
    static std::random_device rd;
    static std::mt19937 eng(rd());
    std::uniform_int_distribution<int> dist(0, end - begin - 1);
    auto pivot = begin + dist(eng);
    auto pivotValue = std::move(*pivot);
    *pivot = std::move(*begin);
    pivot = begin;
    for(auto it = begin + 1; it < end; ++it) {
        if(*it < pivotValue) {
            *pivot = std::move(*it);
            *it = std::move(*++pivot);
        }
    }
    *pivot = std::move(pivotValue);
    quickSort(begin, pivot);
    quickSort(pivot + 1, end);
}

template<typename ForwardIterator>
bool isAscending(ForwardIterator begin, ForwardIterator end) {
    if(begin == end) return true;
    auto it0 = begin, it1 = begin;
    ++it1;
    while(it1 != end) {
        if(*it1 < *it0) {
            return false;
        }
        ++it0;
        ++it1;
    }
    return true;
}

std::vector<double> generate(unsigned n) {
    std::vector<double> samples;
    std::random_device rd;
    std::mt19937 eng(rd());
    std::normal_distribution<double> dist(5.0, 3.0);    // mu(5.0, 3.0)
    std::generate_n(std::back_inserter(samples), n, std::bind(dist, std::ref(eng)));
    return samples;
}

class Profiler {
public:
    Profiler(long &val=Profiler::dummy) : output(val) {
        t0 = std::chrono::high_resolution_clock::now();
    }
    ~Profiler() {
        auto t1 = std::chrono::high_resolution_clock::now();
        if(&output == &dummy) {
            std::cout << "time elapsed: " << std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count()
                    << " ns" << std::endl;
        } else {
            output = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
        }
    }
private:
    long &output;
    static long dummy;
    std::chrono::high_resolution_clock::time_point t0;
};

long Profiler::dummy = 0;

void benchmark() {
    std::cout << std::setw(8) << std::left << "size"
            << std::setw(15) << std::left << "insertion"
            << std::setw(15) << std::left << "selection"
            << std::setw(15) << std::left << "merge"
            << std::setw(15) << std::left << "quick" << std::endl;
    for(unsigned n = 1; n < (2 << 16); n = int(n * 1.8) + 1) {
        auto samples = generate(n);
        auto x = samples; auto y = samples; auto u = samples; auto v = samples;
        long tx = 0, ty = 0, tu = 0, tv = 0;
        {
            Profiler _(tx);
            insertionSort(x.begin(), x.end());
        }
        {
            Profiler _(ty);
            selectionSort(y.begin(), y.end());
        }
        {
            Profiler _(tu);
            mergeSort(u.begin(), u.end());
        }
        {
            Profiler _(tv);
            quickSort(v.begin(), v.end());
        }
        std::cout << std::setw(8) << std::left << n
                << std::setw(15) << std::left << tx
                << std::setw(15) << std::left << ty
                << std::setw(15) << std::left << tu
                << std::setw(15) << std::left << tv << std::endl;
    }
}

int main() {
    benchmark();

    return 0;
}
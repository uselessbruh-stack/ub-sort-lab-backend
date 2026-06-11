#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <cstring>
#include <algorithm>
#include <random>
#include <iomanip>
#include <sstream>

const int MAX_ELEMENTS = 50;
const int MAX_VALUE = 1000;



class SortingAnalyzer {
public:
    std::string algorithm;
    std::vector<uint32_t> original_arr;
    std::vector<uint32_t> arr;
    long long comparisons = 0;
    long long swaps = 0;


    SortingAnalyzer(const std::string& algo, const std::vector<uint32_t>& input)
        : algorithm(algo), original_arr(input), arr(input) {}



    void compare() { comparisons++; }
    void swap(int i, int j) {
        std::swap(arr[i], arr[j]);
        swaps++;
    }

    void bubbleSort() {
        int n = arr.size();
        for (int i = 0; i < n; i++) {
            bool swapped = false;
            for (int j = 0; j < n - i - 1; j++) {
                compare();
                if (arr[j] > arr[j + 1]) {
                    swap(j, j + 1);
                    swapped = true;
                }
            }
            if (!swapped) break;
        }
    }

    void selectionSort() {
        int n = arr.size();
        for (int i = 0; i < n; i++) {
            int minIdx = i;
            for (int j = i + 1; j < n; j++) {
                compare();
                if (arr[j] < arr[minIdx]) {
                    minIdx = j;
                }
            }
            swap(i, minIdx);
        }
    }

    void insertionSort() {
        for (int i = 1; i < arr.size(); i++) {
            uint32_t key = arr[i];
            int j = i - 1;
            while (j >= 0) {
                compare();
                if (arr[j] > key) {
                    arr[j + 1] = arr[j];
                    swaps++;
                    j--;
                } else {
                    break;
                }
            }
            arr[j + 1] = key;
        }
    }

    void mergeSortHelper(std::vector<uint32_t>& temp, int left, int right) {
        if (left >= right) return;
        int mid = left + (right - left) / 2;
        mergeSortHelper(temp, left, mid);
        mergeSortHelper(temp, mid + 1, right);
        merge(temp, left, mid, right);
    }

    void merge(std::vector<uint32_t>& temp, int left, int mid, int right) {
        std::vector<uint32_t> merged;
        int i = left, j = mid + 1;
        while (i <= mid && j <= right) {
            compare();
            if (arr[i] <= arr[j]) {
                merged.push_back(arr[i++]);
            } else {
                merged.push_back(arr[j++]);
            }
            swaps++;
        }
        while (i <= mid) {
            merged.push_back(arr[i++]);
            swaps++;
        }
        while (j <= right) {
            merged.push_back(arr[j++]);
            swaps++;
        }
        for (int i = 0; i < merged.size(); i++) {
            arr[left + i] = merged[i];
        }
    }

    void mergeSort() {
        if (arr.size() > 0) {
            std::vector<uint32_t> temp = arr;
            mergeSortHelper(temp, 0, arr.size() - 1);
        }
    }

    int partition(int low, int high) {
        uint32_t pivot = arr[high];
        int i = low - 1;
        for (int j = low; j < high; j++) {
            compare();
            if (arr[j] < pivot) {
                i++;
                swap(i, j);
            }
        }
        swap(i + 1, high);
        return i + 1;
    }

    void quickSortHelper(int low, int high) {
        if (low < high) {
            int pi = partition(low, high);
            quickSortHelper(low, pi - 1);
            quickSortHelper(pi + 1, high);
        }
    }

    void quickSort() {
        if (arr.size() > 0) {
            quickSortHelper(0, arr.size() - 1);
        }
    }

    void heapify(int i, int n) {
        while (true) {
            int largest = i;
            int left = 2 * i + 1;
            int right = 2 * i + 2;

            if (left < n) {
                compare();
                if (arr[left] > arr[largest]) {
                    largest = left;
                }
            }
            if (right < n) {
                compare();
                if (arr[right] > arr[largest]) {
                    largest = right;
                }
            }
            if (largest != i) {
                swap(i, largest);
                i = largest;
            } else {
                break;
            }
        }
    }

    void heapSort() {
        int n = arr.size();
        for (int i = n / 2 - 1; i >= 0; i--) {
            heapify(i, n);
        }
        for (int i = n - 1; i > 0; i--) {
            swap(0, i);
            heapify(0, i);
        }
    }

    void countingSort() {
        if (arr.empty()) return;
        uint32_t maxVal = arr[0], minVal = arr[0];
        for (uint32_t v : arr) {
            if (v > maxVal) maxVal = v;
            if (v < minVal) minVal = v;
        }

        std::vector<int> count(maxVal - minVal + 1, 0);
        for (uint32_t v : arr) {
            count[v - minVal]++;
            compare();
        }

        int idx = 0;
        for (int i = 0; i < count.size(); i++) {
            for (int j = 0; j < count[i]; j++) {
                arr[idx++] = minVal + i;
                swaps++;
            }
        }
    }

    void radixSort() {
        if (arr.empty()) return;
        uint32_t maxVal = arr[0];
        for (uint32_t v : arr) {
            if (v > maxVal) maxVal = v;
        }

        for (uint32_t exp = 1; maxVal / exp > 0; exp *= 10) {
            countingSortRadix(exp);
        }
    }

    void countingSortRadix(uint32_t exp) {
        std::vector<int> count(10, 0);
        for (uint32_t v : arr) {
            count[(v / exp) % 10]++;
            compare();
        }

        for (int i = 1; i < 10; i++) {
            count[i] += count[i - 1];
        }

        std::vector<uint32_t> output(arr.size());
        for (int i = arr.size() - 1; i >= 0; i--) {
            uint32_t idx = (arr[i] / exp) % 10;
            output[count[idx] - 1] = arr[i];
            count[idx]--;
            swaps++;
        }
        arr = output;
    }

    void bucketSort() {
        if (arr.empty()) return;
        uint32_t maxVal = arr[0], minVal = arr[0];
        for (uint32_t v : arr) {
            if (v > maxVal) maxVal = v;
            if (v < minVal) minVal = v;
        }

        int bucketCount = arr.size();
        std::vector<std::vector<uint32_t>> buckets(bucketCount);

        for (uint32_t v : arr) {
            int idx;
            if (maxVal == minVal) {
                idx = 0;
            } else {
                idx = (int)((v - minVal) / (double)(maxVal - minVal) * (bucketCount - 1));
            }
            buckets[idx].push_back(v);
            compare();
        }

        int idx = 0;
        for (auto& bucket : buckets) {
            insertionSortBucket(bucket);
            for (uint32_t v : bucket) {
                arr[idx++] = v;
                swaps++;
            }
        }
    }

    void insertionSortBucket(std::vector<uint32_t>& arr) {
        for (int i = 1; i < arr.size(); i++) {
            uint32_t key = arr[i];
            int j = i - 1;
            while (j >= 0 && arr[j] > key) {
                arr[j + 1] = arr[j];
                j--;
            }
            arr[j + 1] = key;
        }
    }

    void timInsertionSort(int left, int right) {
        for (int i = left + 1; i <= right; i++) {
            uint32_t key = arr[i];
            int j = i - 1;
            while (j >= left) {
                compare();
                if (arr[j] > key) {
                    arr[j + 1] = arr[j];
                    swaps++;
                    j--;
                } else {
                    break;
                }
            }
            arr[j + 1] = key;
        }
    }

    void timMerge(int left, int mid, int right) {
        std::vector<uint32_t> leftArr(arr.begin() + left, arr.begin() + mid + 1);
        std::vector<uint32_t> rightArr(arr.begin() + mid + 1, arr.begin() + right + 1);
        int i = 0, j = 0, k = left;
        while (i < leftArr.size() && j < rightArr.size()) {
            compare();
            if (leftArr[i] <= rightArr[j]) {
                arr[k] = leftArr[i++];
            } else {
                arr[k] = rightArr[j++];
            }
            swaps++;
            k++;
        }
        while (i < leftArr.size()) {
            arr[k++] = leftArr[i++];
            swaps++;
        }
        while (j < rightArr.size()) {
            arr[k++] = rightArr[j++];
            swaps++;
        }
    }

    void timSort() {
        int minRun = 32;
        int n = arr.size();
        for (int i = 0; i < n; i += minRun) {
            int right = std::min(i + minRun - 1, n - 1);
            timInsertionSort(i, right);
        }
        for (int size = minRun; size < n; size *= 2) {
            for (int start = 0; start < n; start += size * 2) {
                int mid = start + size - 1;
                int right = std::min(start + size * 2 - 1, n - 1);
                if (mid < right) {
                    timMerge(start, mid, right);
                }
            }
        }
    }

    void introSortHelper(int low, int high, int maxDepth) {
        if (high - low <= 1) return;
        if (maxDepth == 0) {
            insertionSort();
            return;
        }

        int mid = low + (high - low) / 2;
        uint32_t pivot = arr[mid];
        std::vector<uint32_t> left, middle, right;

        for (int i = low; i < high; i++) {
            compare();
            if (arr[i] < pivot) {
                left.push_back(arr[i]);
            } else if (arr[i] == pivot) {
                middle.push_back(arr[i]);
            } else {
                right.push_back(arr[i]);
            }
            swaps++;
        }

        int idx = low;
        for (uint32_t v : left) {
            arr[idx++] = v;
        }
        for (uint32_t v : middle) {
            arr[idx++] = v;
        }
        for (uint32_t v : right) {
            arr[idx++] = v;
        }

        introSortHelper(low, low + left.size(), maxDepth - 1);
        introSortHelper(low + left.size() + middle.size(), high, maxDepth - 1);
    }

    void introSort() {
        int maxDepth = 2 * (int)std::log2(arr.size());
        introSortHelper(0, arr.size(), maxDepth);
    }

    void executeSort() {
        if (algorithm == "bubble") {
            bubbleSort();
        } else if (algorithm == "selection") {
            selectionSort();
        } else if (algorithm == "insertion") {
            insertionSort();
        } else if (algorithm == "merge") {
            mergeSort();
        } else if (algorithm == "quick") {
            quickSort();
        } else if (algorithm == "heap") {
            heapSort();
        } else if (algorithm == "counting") {
            countingSort();
        } else if (algorithm == "radix") {
            radixSort();
        } else if (algorithm == "bucket") {
            bucketSort();
        } else if (algorithm == "tim") {
            timSort();
        } else if (algorithm == "intro") {
            introSort();
        }
    }

    bool isSorted() {
        for (int i = 0; i < arr.size() - 1; i++) {
            if (arr[i] > arr[i + 1]) {
                return false;
            }
        }
        return true;
    }

    std::string arrayToJsonString(const std::vector<uint32_t>& v) {
        std::stringstream ss;
        ss << "[";
        for (int i = 0; i < v.size(); i++) {
            ss << v[i];
            if (i < v.size() - 1) ss << ",";
        }
        ss << "]";
        return ss.str();
    }

    std::string sort() {
        arr = original_arr;
        comparisons = 0;
        swaps = 0;

        auto start = std::chrono::high_resolution_clock::now();
        executeSort();
        auto end = std::chrono::high_resolution_clock::now();

        double elapsed_ms =
            std::chrono::duration<double, std::milli>(end - start).count();

        std::stringstream ss;
        ss << "{\"algorithm\":\"" << algorithm << "\",\"n\":" << original_arr.size()
           << ",\"input\":" << arrayToJsonString(original_arr)
           << ",\"output\":" << arrayToJsonString(arr)
           << ",\"actual_time_ms\":" << std::fixed << std::setprecision(6) << elapsed_ms
           << ",\"comparisons\":" << comparisons
           << ",\"swaps\":" << swaps << ",\"is_sorted\":" << (isSorted() ? "true" : "false")
           << "}";

        return ss.str();
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <algorithm> [value1 value2 ...]\n";
        std::cerr << "Algorithms: bubble, selection, insertion, merge, quick, heap, counting, "
                     "radix, bucket, tim, intro\n";
        return 1;
    }

    std::string algorithm = argv[1];
    std::vector<uint32_t> arr;

    if (argc > 2) {
        for (int i = 2; i < argc; i++) {
            try {
                uint32_t val = std::stoul(argv[i]);
                arr.push_back(val);
            } catch (...) {
                std::cerr << "Error: Invalid number format\n";
                return 1;
            }
        }
    } else {
        std::random_device rd;
        std::mt19937 gen(42);
        std::uniform_int_distribution<> dis(0, MAX_VALUE);
        for (int i = 0; i < MAX_ELEMENTS; i++) {
            arr.push_back(dis(gen));
        }
    }

    SortingAnalyzer analyzer(algorithm, arr);
    std::cout << analyzer.sort() << std::endl;

    return 0;
}

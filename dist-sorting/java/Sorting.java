import java.util.*;
import java.util.stream.Collectors;

class SortingAnalyzer {
    private String algorithm;
    private List<Integer> original_arr;
    private List<Integer> arr;
    private long comparisons = 0;
    private long swaps = 0;

    private static final int MAX_VALUE = 1000;

    SortingAnalyzer(String algorithm, List<Integer> input) {
        this.algorithm = algorithm;
        this.original_arr = new ArrayList<>(input);
        this.arr = new ArrayList<>(input);
    }

    private void compare() {
        comparisons++;
    }

    private void swap(int i, int j) {
        Integer temp = arr.get(i);
        arr.set(i, arr.get(j));
        arr.set(j, temp);
        swaps++;
    }

    private void bubbleSort() {
        int n = arr.size();
        for (int i = 0; i < n; i++) {
            boolean swapped = false;
            for (int j = 0; j < n - i - 1; j++) {
                compare();
                if (arr.get(j) > arr.get(j + 1)) {
                    swap(j, j + 1);
                    swapped = true;
                }
            }
            if (!swapped) break;
        }
    }

    private void selectionSort() {
        int n = arr.size();
        for (int i = 0; i < n; i++) {
            int minIdx = i;
            for (int j = i + 1; j < n; j++) {
                compare();
                if (arr.get(j) < arr.get(minIdx)) {
                    minIdx = j;
                }
            }
            swap(i, minIdx);
        }
    }

    private void insertionSort() {
        for (int i = 1; i < arr.size(); i++) {
            int key = arr.get(i);
            int j = i - 1;
            while (j >= 0) {
                compare();
                if (arr.get(j) > key) {
                    arr.set(j + 1, arr.get(j));
                    swaps++;
                    j--;
                } else {
                    break;
                }
            }
            arr.set(j + 1, key);
        }
    }

    private void mergeSortHelper(int left, int right) {
        if (left >= right) return;
        int mid = left + (right - left) / 2;
        mergeSortHelper(left, mid);
        mergeSortHelper(mid + 1, right);
        merge(left, mid, right);
    }

    private void merge(int left, int mid, int right) {
        List<Integer> merged = new ArrayList<>();
        int i = left, j = mid + 1;
        while (i <= mid && j <= right) {
            compare();
            if (arr.get(i) <= arr.get(j)) {
                merged.add(arr.get(i++));
            } else {
                merged.add(arr.get(j++));
            }
            swaps++;
        }
        while (i <= mid) {
            merged.add(arr.get(i++));
            swaps++;
        }
        while (j <= right) {
            merged.add(arr.get(j++));
            swaps++;
        }
        for (int k = 0; k < merged.size(); k++) {
            arr.set(left + k, merged.get(k));
        }
    }

    private void mergeSort() {
        if (arr.size() > 0) {
            mergeSortHelper(0, arr.size() - 1);
        }
    }

    private int partition(int low, int high) {
        int pivot = arr.get(high);
        int i = low - 1;
        for (int j = low; j < high; j++) {
            compare();
            if (arr.get(j) < pivot) {
                i++;
                swap(i, j);
            }
        }
        swap(i + 1, high);
        return i + 1;
    }

    private void quickSortHelper(int low, int high) {
        if (low < high) {
            int pi = partition(low, high);
            quickSortHelper(low, pi - 1);
            quickSortHelper(pi + 1, high);
        }
    }

    private void quickSort() {
        if (arr.size() > 0) {
            quickSortHelper(0, arr.size() - 1);
        }
    }

    private void heapify(int i, int n) {
        while (true) {
            int largest = i;
            int left = 2 * i + 1;
            int right = 2 * i + 2;

            if (left < n) {
                compare();
                if (arr.get(left) > arr.get(largest)) {
                    largest = left;
                }
            }
            if (right < n) {
                compare();
                if (arr.get(right) > arr.get(largest)) {
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

    private void heapSort() {
        int n = arr.size();
        for (int i = n / 2 - 1; i >= 0; i--) {
            heapify(i, n);
        }
        for (int i = n - 1; i > 0; i--) {
            swap(0, i);
            heapify(0, i);
        }
    }

    private void countingSort() {
        if (arr.isEmpty()) return;
        int maxVal = arr.stream().mapToInt(Integer::intValue).max().orElse(0);
        int minVal = arr.stream().mapToInt(Integer::intValue).min().orElse(0);

        int[] count = new int[maxVal - minVal + 1];
        for (int v : arr) {
            count[v - minVal]++;
            compare();
        }

        int idx = 0;
        for (int i = 0; i < count.length; i++) {
            for (int j = 0; j < count[i]; j++) {
                arr.set(idx++, minVal + i);
                swaps++;
            }
        }
    }

    private void radixSort() {
        if (arr.isEmpty()) return;
        int maxVal = arr.stream().mapToInt(Integer::intValue).max().orElse(0);

        for (int exp = 1; maxVal / exp > 0; exp *= 10) {
            countingSortRadix(exp);
        }
    }

    private void countingSortRadix(int exp) {
        int[] count = new int[10];
        for (int v : arr) {
            count[(v / exp) % 10]++;
            compare();
        }

        for (int i = 1; i < 10; i++) {
            count[i] += count[i - 1];
        }

        List<Integer> output = new ArrayList<>(Collections.nCopies(arr.size(), 0));
        for (int i = arr.size() - 1; i >= 0; i--) {
            int idx = (arr.get(i) / exp) % 10;
            output.set(count[idx] - 1, arr.get(i));
            count[idx]--;
            swaps++;
        }
        arr = output;
    }

    private void bucketSort() {
        if (arr.isEmpty()) return;
        int maxVal = arr.stream().mapToInt(Integer::intValue).max().orElse(0);
        int minVal = arr.stream().mapToInt(Integer::intValue).min().orElse(0);

        int bucketCount = arr.size();
        List<List<Integer>> buckets = new ArrayList<>();
        for (int i = 0; i < bucketCount; i++) {
            buckets.add(new ArrayList<>());
        }

        for (int v : arr) {
            int idx;
            if (maxVal == minVal) {
                idx = 0;
            } else {
                idx = (int)((v - minVal) / (double)(maxVal - minVal) * (bucketCount - 1));
            }
            buckets.get(idx).add(v);
            compare();
        }

        int idx = 0;
        for (List<Integer> bucket : buckets) {
            insertionSortBucket(bucket);
            for (int v : bucket) {
                arr.set(idx++, v);
                swaps++;
            }
        }
    }

    private void insertionSortBucket(List<Integer> arr) {
        for (int i = 1; i < arr.size(); i++) {
            int key = arr.get(i);
            int j = i - 1;
            while (j >= 0 && arr.get(j) > key) {
                arr.set(j + 1, arr.get(j));
                j--;
            }
            arr.set(j + 1, key);
        }
    }

    private static final int MIN_RUN = 32;

    private void timInsertionSort(int left, int right) {
        for (int i = left + 1; i <= right; i++) {
            int key = arr.get(i);
            int j = i - 1;
            while (j >= left) {
                compare();
                if (arr.get(j) > key) {
                    arr.set(j + 1, arr.get(j));
                    swaps++;
                    j--;
                } else {
                    break;
                }
            }
            arr.set(j + 1, key);
        }
    }

    private void timMerge(int left, int mid, int right) {
        List<Integer> leftArr = new ArrayList<>(arr.subList(left, mid + 1));
        List<Integer> rightArr = new ArrayList<>(arr.subList(mid + 1, right + 1));
        int i = 0, j = 0, k = left;
        while (i < leftArr.size() && j < rightArr.size()) {
            compare();
            if (leftArr.get(i) <= rightArr.get(j)) {
                arr.set(k, leftArr.get(i++));
            } else {
                arr.set(k, rightArr.get(j++));
            }
            swaps++;
            k++;
        }
        while (i < leftArr.size()) {
            arr.set(k++, leftArr.get(i++));
            swaps++;
        }
        while (j < rightArr.size()) {
            arr.set(k++, rightArr.get(j++));
            swaps++;
        }
    }

    private void timSort() {
        int n = arr.size();
        for (int i = 0; i < n; i += MIN_RUN) {
            int right = Math.min(i + MIN_RUN - 1, n - 1);
            timInsertionSort(i, right);
        }
        for (int size = MIN_RUN; size < n; size *= 2) {
            for (int start = 0; start < n; start += size * 2) {
                int mid = start + size - 1;
                int right = Math.min(start + size * 2 - 1, n - 1);
                if (mid < right) {
                    timMerge(start, mid, right);
                }
            }
        }
    }

    private void introSortHelper(int low, int high, int maxDepth) {
        if (high - low <= 1) return;
        if (maxDepth == 0) {
            insertionSort();
            return;
        }

        int mid = low + (high - low) / 2;
        int pivot = arr.get(mid);
        List<Integer> left = new ArrayList<>(), middle = new ArrayList<>(), right = new ArrayList<>();

        for (int i = low; i < high; i++) {
            compare();
            if (arr.get(i) < pivot) {
                left.add(arr.get(i));
            } else if (arr.get(i) == pivot) {
                middle.add(arr.get(i));
            } else {
                right.add(arr.get(i));
            }
            swaps++;
        }

        int idx = low;
        for (int v : left) {
            arr.set(idx++, v);
        }
        for (int v : middle) {
            arr.set(idx++, v);
        }
        for (int v : right) {
            arr.set(idx++, v);
        }

        introSortHelper(low, low + left.size(), maxDepth - 1);
        introSortHelper(low + left.size() + middle.size(), high, maxDepth - 1);
    }

    private void introSort() {
        int maxDepth = 2 * (int)(Math.log(arr.size()) / Math.log(2));
        introSortHelper(0, arr.size(), maxDepth);
    }

    private void executeSort() {
        switch (algorithm) {
            case "bubble":
                bubbleSort();
                break;
            case "selection":
                selectionSort();
                break;
            case "insertion":
                insertionSort();
                break;
            case "merge":
                mergeSort();
                break;
            case "quick":
                quickSort();
                break;
            case "heap":
                heapSort();
                break;
            case "counting":
                countingSort();
                break;
            case "radix":
                radixSort();
                break;
            case "bucket":
                bucketSort();
                break;
            case "tim":
                timSort();
                break;
            case "intro":
                introSort();
                break;
        }
    }

    private boolean isSorted() {
        for (int i = 0; i < arr.size() - 1; i++) {
            if (arr.get(i) > arr.get(i + 1)) {
                return false;
            }
        }
        return true;
    }

    private String arrayToJsonString(List<Integer> list) {
        return "[" + list.stream().map(String::valueOf).collect(Collectors.joining(",")) + "]";
    }

    public String sort() {
        arr = new ArrayList<>(original_arr);
        comparisons = 0;
        swaps = 0;

        long start = System.nanoTime();
        executeSort();
        long end = System.nanoTime();

        double elapsed_ms = (end - start) / 1_000_000.0;

        StringBuilder sb = new StringBuilder();
        sb.append("{\"algorithm\":\"").append(algorithm).append("\",\"n\":").append(original_arr.size())
          .append(",\"input\":").append(arrayToJsonString(original_arr))
          .append(",\"output\":").append(arrayToJsonString(arr))
          .append(",\"actual_time_ms\":").append(String.format("%.6f", elapsed_ms))
          .append(",\"comparisons\":").append(comparisons)
          .append(",\"swaps\":").append(swaps)
          .append(",\"is_sorted\":").append(isSorted() ? "true" : "false")
          .append("}");

        return sb.toString();
    }
}

public class Sorting {
    private static final int MAX_ELEMENTS = 50;
    private static final int MAX_VALUE = 1000;

    public static void main(String[] args) {
        if (args.length < 1) {
            System.err.println("Usage: java Sorting <algorithm> [value1 value2 ...]");
            System.err.println("Algorithms: bubble, selection, insertion, merge, quick, heap, counting, radix, bucket, tim, intro");
            System.exit(1);
        }

        String algorithm = args[0];
        List<Integer> arr = new ArrayList<>();

        if (args.length > 1) {
            if (args.length - 1 > MAX_ELEMENTS) {
                System.err.println("Error: Maximum 50 elements allowed");
                System.exit(1);
            }
            for (int i = 1; i < args.length; i++) {
                try {
                    int val = Integer.parseInt(args[i]);
                    if (val < 0 || val > MAX_VALUE) {
                        System.err.println("Error: Element values must be between 0 and " + MAX_VALUE);
                        System.exit(1);
                    }
                    arr.add(val);
                } catch (NumberFormatException e) {
                    System.err.println("Error: Invalid number format");
                    System.exit(1);
                }
            }
        } else {
            Random rand = new Random(42);
            for (int i = 0; i < MAX_ELEMENTS; i++) {
                arr.add(rand.nextInt(MAX_VALUE + 1));
            }
        }

        SortingAnalyzer analyzer = new SortingAnalyzer(algorithm, arr);
        System.out.println(analyzer.sort());
    }
}

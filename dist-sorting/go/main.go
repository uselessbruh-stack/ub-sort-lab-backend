package main

import (
	"encoding/json"
	"fmt"
	"math"
	"math/rand"
	"os"
	"strconv"
	"time"
)

func getHighPrecisionTimeMs() float64 {
	return float64(time.Now().UnixNano()) / 1e6
}

const (
	MAX_ELEMENTS = 50
	MAX_VALUE    = 1000
)

type SortingAnalyzer struct {
	Algorithm   string
	OriginalArr []uint
	Arr         []uint
	Comparisons int64
	Swaps       int64
}

func NewSortingAnalyzer(algorithm string, arr []uint) *SortingAnalyzer {
	arrCopy := make([]uint, len(arr))
	copy(arrCopy, arr)
	return &SortingAnalyzer{
		Algorithm:   algorithm,
		OriginalArr: arrCopy,
		Arr:         arrCopy,
	}
}

func (sa *SortingAnalyzer) compare() {
	sa.Comparisons++
}

func (sa *SortingAnalyzer) swap(i, j int) {
	sa.Arr[i], sa.Arr[j] = sa.Arr[j], sa.Arr[i]
	sa.Swaps++
}

func (sa *SortingAnalyzer) bubbleSort() {
	n := len(sa.Arr)
	for i := 0; i < n; i++ {
		swapped := false
		for j := 0; j < n-i-1; j++ {
			sa.compare()
			if sa.Arr[j] > sa.Arr[j+1] {
				sa.swap(j, j+1)
				swapped = true
			}
		}
		if !swapped {
			break
		}
	}
}

func (sa *SortingAnalyzer) selectionSort() {
	n := len(sa.Arr)
	for i := 0; i < n; i++ {
		minIdx := i
		for j := i + 1; j < n; j++ {
			sa.compare()
			if sa.Arr[j] < sa.Arr[minIdx] {
				minIdx = j
			}
		}
		sa.swap(i, minIdx)
	}
}

func (sa *SortingAnalyzer) insertionSort() {
	for i := 1; i < len(sa.Arr); i++ {
		key := sa.Arr[i]
		j := i - 1
		for j >= 0 {
			sa.compare()
			if sa.Arr[j] > key {
				sa.Arr[j+1] = sa.Arr[j]
				sa.Swaps++
				j--
			} else {
				break
			}
		}
		sa.Arr[j+1] = key
	}
}

func (sa *SortingAnalyzer) mergeSort() {
	sa.Arr = sa.mergeSortHelper(sa.Arr, 0, len(sa.Arr))
}

func (sa *SortingAnalyzer) mergeSortHelper(arr []uint, low, high int) []uint {
	if high <= 1 {
		return arr
	}
	mid := high / 2
	left := make([]uint, mid)
	copy(left, arr[:mid])
	left = sa.mergeSortHelper(left, 0, mid)

	right := make([]uint, high-mid)
	copy(right, arr[mid:high])
	right = sa.mergeSortHelper(right, 0, high-mid)

	return sa.merge(left, right)
}

func (sa *SortingAnalyzer) merge(left, right []uint) []uint {
	result := make([]uint, 0, len(left)+len(right))
	i, j := 0, 0
	for i < len(left) && j < len(right) {
		sa.compare()
		if left[i] <= right[j] {
			result = append(result, left[i])
			i++
		} else {
			result = append(result, right[j])
			j++
		}
		sa.Swaps++
	}
	result = append(result, left[i:]...)
	sa.Swaps += int64(len(left) - i)
	result = append(result, right[j:]...)
	sa.Swaps += int64(len(right) - j)
	return result
}

func (sa *SortingAnalyzer) quickSort() {
	if len(sa.Arr) > 0 {
		sa.quickSortHelper(0, len(sa.Arr)-1)
	}
}

func (sa *SortingAnalyzer) quickSortHelper(low, high int) {
	if low < high {
		pi := sa.partition(low, high)
		sa.quickSortHelper(low, pi-1)
		sa.quickSortHelper(pi+1, high)
	}
}

func (sa *SortingAnalyzer) partition(low, high int) int {
	pivot := sa.Arr[high]
	i := low - 1
	for j := low; j < high; j++ {
		sa.compare()
		if sa.Arr[j] < pivot {
			i++
			sa.swap(i, j)
		}
	}
	sa.swap(i+1, high)
	return i + 1
}

func (sa *SortingAnalyzer) heapSort() {
	n := len(sa.Arr)
	for i := n/2 - 1; i >= 0; i-- {
		sa.heapify(i, n)
	}
	for i := n - 1; i > 0; i-- {
		sa.swap(0, i)
		sa.heapify(0, i)
	}
}

func (sa *SortingAnalyzer) heapify(i, n int) {
	for {
		largest := i
		left := 2*i + 1
		right := 2*i + 2

		if left < n {
			sa.compare()
			if sa.Arr[left] > sa.Arr[largest] {
				largest = left
			}
		}
		if right < n {
			sa.compare()
			if sa.Arr[right] > sa.Arr[largest] {
				largest = right
			}
		}
		if largest != i {
			sa.swap(i, largest)
			i = largest
		} else {
			break
		}
	}
}

func (sa *SortingAnalyzer) countingSort() {
	if len(sa.Arr) == 0 {
		return
	}
	maxVal := sa.Arr[0]
	minVal := sa.Arr[0]
	for _, v := range sa.Arr {
		if v > maxVal {
			maxVal = v
		}
		if v < minVal {
			minVal = v
		}
	}

	count := make([]int, maxVal-minVal+1)
	for _, v := range sa.Arr {
		count[v-minVal]++
		sa.compare()
	}

	idx := 0
	for i := 0; i < len(count); i++ {
		for j := 0; j < count[i]; j++ {
			sa.Arr[idx] = minVal + uint(i)
			idx++
			sa.Swaps++
		}
	}
}

func (sa *SortingAnalyzer) radixSort() {
	if len(sa.Arr) == 0 {
		return
	}
	maxVal := sa.Arr[0]
	for _, v := range sa.Arr {
		if v > maxVal {
			maxVal = v
		}
	}

	for exp := uint(1); maxVal/exp > 0; exp *= 10 {
		sa.countingSortRadix(exp)
	}
}

func (sa *SortingAnalyzer) countingSortRadix(exp uint) {
	count := make([]int, 10)
	for _, v := range sa.Arr {
		idx := (v / exp) % 10
		count[idx]++
		sa.compare()
	}

	for i := 1; i < 10; i++ {
		count[i] += count[i-1]
	}

	output := make([]uint, len(sa.Arr))
	for i := len(sa.Arr) - 1; i >= 0; i-- {
		idx := (sa.Arr[i] / exp) % 10
		count[idx]--
		output[count[idx]] = sa.Arr[i]
		sa.Swaps++
	}
	copy(sa.Arr, output)
}

func (sa *SortingAnalyzer) bucketSort() {
	if len(sa.Arr) == 0 {
		return
	}
	maxVal := sa.Arr[0]
	minVal := sa.Arr[0]
	for _, v := range sa.Arr {
		if v > maxVal {
			maxVal = v
		}
		if v < minVal {
			minVal = v
		}
	}

	bucketCount := len(sa.Arr)
	buckets := make([][]uint, bucketCount)

	for _, v := range sa.Arr {
		var idx int
		if maxVal == minVal {
			idx = 0
		} else {
			idx = int(float64(v-minVal) / float64(maxVal-minVal) * float64(bucketCount-1))
		}
		buckets[idx] = append(buckets[idx], v)
		sa.compare()
	}

	idx := 0
	for _, bucket := range buckets {
		sort := make([]uint, len(bucket))
		copy(sort, bucket)
		sa.insertionSortBucket(sort)
		for _, v := range sort {
			sa.Arr[idx] = v
			idx++
			sa.Swaps++
		}
	}
}

func (sa *SortingAnalyzer) insertionSortBucket(arr []uint) {
	for i := 1; i < len(arr); i++ {
		key := arr[i]
		j := i - 1
		for j >= 0 && arr[j] > key {
			arr[j+1] = arr[j]
			j--
		}
		arr[j+1] = key
	}
}

const MIN_RUN = 32

func (sa *SortingAnalyzer) timInsertionSort(left, right int) {
	for i := left + 1; i <= right; i++ {
		key := sa.Arr[i]
		j := i - 1
		for j >= left {
			sa.compare()
			if sa.Arr[j] > key {
				sa.Arr[j+1] = sa.Arr[j]
				sa.Swaps++
				j--
			} else {
				break
			}
		}
		sa.Arr[j+1] = key
	}
}

func (sa *SortingAnalyzer) timMerge(left, mid, right int) {
	leftArr := make([]uint, mid-left+1)
	rightArr := make([]uint, right-mid)
	copy(leftArr, sa.Arr[left:mid+1])
	copy(rightArr, sa.Arr[mid+1:right+1])

	i, j, k := 0, 0, left
	for i < len(leftArr) && j < len(rightArr) {
		sa.compare()
		if leftArr[i] <= rightArr[j] {
			sa.Arr[k] = leftArr[i]
			i++
		} else {
			sa.Arr[k] = rightArr[j]
			j++
		}
		sa.Swaps++
		k++
	}
	for i < len(leftArr) {
		sa.Arr[k] = leftArr[i]
		sa.Swaps++
		i++
		k++
	}
	for j < len(rightArr) {
		sa.Arr[k] = rightArr[j]
		sa.Swaps++
		j++
		k++
	}
}

func (sa *SortingAnalyzer) timSort() {
	n := len(sa.Arr)
	for i := 0; i < n; i += MIN_RUN {
		right := i + MIN_RUN - 1
		if right >= n {
			right = n - 1
		}
		sa.timInsertionSort(i, right)
	}
	for size := MIN_RUN; size < n; size *= 2 {
		for start := 0; start < n; start += size * 2 {
			mid := start + size - 1
			right := start + size*2 - 1
			if right >= n {
				right = n - 1
			}
			if mid < right {
				sa.timMerge(start, mid, right)
			}
		}
	}
}

func (sa *SortingAnalyzer) introSort() {
	maxDepth := 2 * int(math.Log2(float64(len(sa.Arr))))
	sa.introSortHelper(0, len(sa.Arr), maxDepth)
}

func (sa *SortingAnalyzer) introSortHelper(low, high, maxDepth int) {
	if high-low <= 1 {
		return
	}
	if maxDepth == 0 {
		sa.simpleSort()
		return
	}

	mid := low + (high-low)/2
	pivot := sa.Arr[mid]
	left := make([]uint, 0)
	middle := make([]uint, 0)
	right := make([]uint, 0)

	for i := low; i < high; i++ {
		sa.compare()
		if sa.Arr[i] < pivot {
			left = append(left, sa.Arr[i])
		} else if sa.Arr[i] == pivot {
			middle = append(middle, sa.Arr[i])
		} else {
			right = append(right, sa.Arr[i])
		}
		sa.Swaps++
	}

	idx := low
	for _, v := range left {
		sa.Arr[idx] = v
		idx++
	}
	for _, v := range middle {
		sa.Arr[idx] = v
		idx++
	}
	for _, v := range right {
		sa.Arr[idx] = v
		idx++
	}

	sa.introSortHelper(low, low+len(left), maxDepth-1)
	sa.introSortHelper(low+len(left)+len(middle), high, maxDepth-1)
}

func (sa *SortingAnalyzer) simpleSort() {
	sa.insertionSort()
}

func (sa *SortingAnalyzer) isSorted() bool {
	for i := 0; i < len(sa.Arr)-1; i++ {
		if sa.Arr[i] > sa.Arr[i+1] {
			return false
		}
	}
	return true
}

func (sa *SortingAnalyzer) executeSort() error {
	switch sa.Algorithm {
	case "bubble":
		sa.bubbleSort()
	case "selection":
		sa.selectionSort()
	case "insertion":
		sa.insertionSort()
	case "merge":
		sa.mergeSort()
	case "quick":
		sa.quickSort()
	case "heap":
		sa.heapSort()
	case "counting":
		sa.countingSort()
	case "radix":
		sa.radixSort()
	case "bucket":
		sa.bucketSort()
	case "tim":
		sa.timSort()
	case "intro":
		sa.introSort()
	default:
		return fmt.Errorf("unknown algorithm: %s", sa.Algorithm)
	}
	return nil
}

func (sa *SortingAnalyzer) sort() (string, error) {
	sa.Arr = make([]uint, len(sa.OriginalArr))
	copy(sa.Arr, sa.OriginalArr)
	sa.Comparisons = 0
	sa.Swaps = 0

	startMs := getHighPrecisionTimeMs()
	if err := sa.executeSort(); err != nil {
		return "", err
	}
	elapsedMs := getHighPrecisionTimeMs() - startMs

	// Build JSON manually to control float formatting precision
	inputJson, _ := json.Marshal(sa.OriginalArr)
	outputJson, _ := json.Marshal(sa.Arr)

	result := fmt.Sprintf(
		`{"algorithm":"%s","n":%d,"input":%s,"output":%s,"actual_time_ms":%.6f,"comparisons":%d,"swaps":%d,"is_sorted":%t}`,
		sa.Algorithm,
		len(sa.OriginalArr),
		string(inputJson),
		string(outputJson),
		elapsedMs,
		sa.Comparisons,
		sa.Swaps,
		sa.isSorted(),
	)

	return result, nil
}

func main() {
	if len(os.Args) < 2 {
		fmt.Fprintf(os.Stderr, "Usage: %s <algorithm> [value1 value2 ...]\n", os.Args[0])
		fmt.Fprintf(os.Stderr, "Algorithms: bubble, selection, insertion, merge, quick, heap, counting, radix, bucket, tim, intro\n")
		os.Exit(1)
	}

	algorithm := os.Args[1]

	if len(os.Args) > 2 {
		// Mode 1: Custom values
		var arr []uint
		for _, arg := range os.Args[2:] {
			val, err := strconv.ParseUint(arg, 10, 32)
			if err != nil {
				fmt.Fprintf(os.Stderr, "Error: Invalid number format\n")
				os.Exit(1)
			}
			arr = append(arr, uint(val))
		}

		analyzer := NewSortingAnalyzer(algorithm, arr)
		result, err := analyzer.sort()
		if err != nil {
			fmt.Fprintf(os.Stderr, "Error: %v\n", err)
			os.Exit(1)
		}
		fmt.Println(result)
	} else {
		// Mode 2: Performance testing with 50 random elements
		rand.Seed(time.Now().UnixNano())
		arr := make([]uint, MAX_ELEMENTS)
		for i := 0; i < MAX_ELEMENTS; i++ {
			arr[i] = uint(rand.Intn(MAX_VALUE + 1))
		}

		analyzer := NewSortingAnalyzer(algorithm, arr)
		result, err := analyzer.sort()
		if err != nil {
			fmt.Fprintf(os.Stderr, "Error: %v\n", err)
			os.Exit(1)
		}
		fmt.Println(result)
	}
}

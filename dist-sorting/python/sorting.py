#!/usr/bin/env python3
"""
Sorting Algorithm Performance Analyzer
Implements 11 sorting algorithms with performance metrics and high-precision timing.
"""

import json
import sys
import time
import random
from copy import deepcopy
from math import log2

MAX_ELEMENTS = 50
MAX_VALUE = 1000

class SortingAnalyzer:
    def __init__(self, algorithm, arr):
        self.algorithm = algorithm
        self.original_arr = arr.copy()
        self.arr = arr.copy()
        self.comparisons = 0
        self.swaps = 0
        self.n = len(arr)
    
    def swap(self, i, j):
        """Swap elements and track swaps"""
        self.arr[i], self.arr[j] = self.arr[j], self.arr[i]
        self.swaps += 1
    
    def compare(self):
        """Track comparisons"""
        self.comparisons += 1
    
    def _bubble_sort(self):
        """Bubble Sort - O(n²)"""
        n = len(self.arr)
        for i in range(n):
            swapped = False
            for j in range(0, n - i - 1):
                self.compare()
                if self.arr[j] > self.arr[j + 1]:
                    self.swap(j, j + 1)
                    swapped = True
            if not swapped:
                break
    
    def _selection_sort(self):
        """Selection Sort - O(n²)"""
        n = len(self.arr)
        for i in range(n):
            min_idx = i
            for j in range(i + 1, n):
                self.compare()
                if self.arr[j] < self.arr[min_idx]:
                    min_idx = j
            self.swap(i, min_idx)
    
    def _insertion_sort(self):
        """Insertion Sort - O(n²)"""
        n = len(self.arr)
        for i in range(1, n):
            key = self.arr[i]
            j = i - 1
            while j >= 0:
                self.compare()
                if self.arr[j] > key:
                    self.arr[j + 1] = self.arr[j]
                    self.swaps += 1
                    j -= 1
                else:
                    break
            self.arr[j + 1] = key
    
    def _merge_sort(self):
        """Merge Sort - O(n log n)"""
        self.arr = self._merge_sort_helper(self.arr)
    
    def _merge_sort_helper(self, arr):
        """Recursive merge sort helper"""
        if len(arr) <= 1:
            return arr
        
        mid = len(arr) // 2
        left = self._merge_sort_helper(arr[:mid])
        right = self._merge_sort_helper(arr[mid:])
        return self._merge(left, right)
    
    def _merge(self, left, right):
        """Merge two sorted arrays"""
        result = []
        i = j = 0
        while i < len(left) and j < len(right):
            self.compare()
            if left[i] <= right[j]:
                result.append(left[i])
                i += 1
            else:
                result.append(right[j])
                j += 1
            self.swaps += 1
        result.extend(left[i:])
        self.swaps += len(left) - i
        result.extend(right[j:])
        self.swaps += len(right) - j
        return result
    
    def _quick_sort(self):
        """Quick Sort - O(n log n) average"""
        self.arr = self._quick_sort_helper(self.arr, 0, len(self.arr) - 1)
    
    def _quick_sort_helper(self, arr, low, high):
        """Recursive quick sort helper"""
        if low < high:
            pi = self._partition(arr, low, high)
            arr = self._quick_sort_helper(arr, low, pi - 1)
            arr = self._quick_sort_helper(arr, pi + 1, high)
        return arr
    
    def _partition(self, arr, low, high):
        """Partition for quick sort"""
        pivot = arr[high]
        i = low - 1
        for j in range(low, high):
            self.compare()
            if arr[j] < pivot:
                i += 1
                arr[i], arr[j] = arr[j], arr[i]
                self.swaps += 1
        arr[i + 1], arr[high] = arr[high], arr[i + 1]
        self.swaps += 1
        return i + 1
    
    def _heap_sort(self):
        """Heap Sort - O(n log n)"""
        n = len(self.arr)
        for i in range(n // 2 - 1, -1, -1):
            self._heapify(i, n)
        
        for i in range(n - 1, 0, -1):
            self.arr[0], self.arr[i] = self.arr[i], self.arr[0]
            self.swaps += 1
            self._heapify(0, i)
    
    def _heapify(self, i, n):
        """Heapify subtree rooted at index i"""
        largest = i
        left = 2 * i + 1
        right = 2 * i + 2
        
        if left < n:
            self.compare()
            if self.arr[left] > self.arr[largest]:
                largest = left
        
        if right < n:
            self.compare()
            if self.arr[right] > self.arr[largest]:
                largest = right
        
        if largest != i:
            self.arr[i], self.arr[largest] = self.arr[largest], self.arr[i]
            self.swaps += 1
            self._heapify(largest, n)
    
    def _counting_sort(self):
        """Counting Sort - O(n + k)"""
        if len(self.arr) == 0:
            return
        
        max_val = max(self.arr)
        min_val = min(self.arr)
        range_val = max_val - min_val + 1
        count = [0] * range_val
        
        for num in self.arr:
            count[num - min_val] += 1
            self.compare()
        
        idx = 0
        for i in range(range_val):
            while count[i] > 0:
                self.arr[idx] = i + min_val
                self.swaps += 1
                idx += 1
                count[i] -= 1
    
    def _radix_sort(self):
        """Radix Sort - O(nk)"""
        if len(self.arr) == 0:
            return
        
        max_val = max(self.arr)
        exp = 1
        
        while max_val // exp > 0:
            self._counting_sort_radix(exp)
            exp *= 10
    
    def _counting_sort_radix(self, exp):
        """Counting sort for radix sort"""
        n = len(self.arr)
        output = [0] * n
        count = [0] * 10
        
        for i in range(n):
            index = (self.arr[i] // exp) % 10
            count[index] += 1
            self.compare()
        
        for i in range(1, 10):
            count[i] += count[i - 1]
        
        for i in range(n - 1, -1, -1):
            index = (self.arr[i] // exp) % 10
            output[count[index] - 1] = self.arr[i]
            count[index] -= 1
            self.swaps += 1
        
        for i in range(n):
            self.arr[i] = output[i]
    
    def _bucket_sort(self):
        """Bucket Sort - O(n + k)"""
        if len(self.arr) == 0:
            return
        
        max_val = max(self.arr)
        min_val = min(self.arr)
        bucket_count = len(self.arr)
        bucket_range = (max_val - min_val) / bucket_count if bucket_count > 0 else 1
        
        buckets = [[] for _ in range(bucket_count)]
        
        for num in self.arr:
            if max_val == min_val:
                index = 0
            else:
                index = int((num - min_val) / bucket_range) if bucket_range > 0 else 0
            index = min(index, bucket_count - 1)
            buckets[index].append(num)
            self.compare()
        
        self.arr = []
        for bucket in buckets:
            bucket.sort()
            self.arr.extend(bucket)
            self.swaps += len(bucket)
    
    MIN_RUN = 32
    
    def _tim_insertion_sort(self, left, right):
        """Insertion sort for TimSort runs"""
        for i in range(left + 1, right + 1):
            key = self.arr[i]
            j = i - 1
            while j >= left:
                self.compare()
                if self.arr[j] > key:
                    self.arr[j + 1] = self.arr[j]
                    self.swaps += 1
                    j -= 1
                else:
                    break
            self.arr[j + 1] = key
    
    def _tim_merge(self, left, mid, right):
        """Merge for TimSort"""
        left_arr = self.arr[left:mid + 1]
        right_arr = self.arr[mid + 1:right + 1]
        i = j = 0
        k = left
        while i < len(left_arr) and j < len(right_arr):
            self.compare()
            if left_arr[i] <= right_arr[j]:
                self.arr[k] = left_arr[i]
                i += 1
            else:
                self.arr[k] = right_arr[j]
                j += 1
            self.swaps += 1
            k += 1
        while i < len(left_arr):
            self.arr[k] = left_arr[i]
            self.swaps += 1
            i += 1
            k += 1
        while j < len(right_arr):
            self.arr[k] = right_arr[j]
            self.swaps += 1
            j += 1
            k += 1
    
    def _tim_sort(self):
        """Tim Sort - O(n log n)"""
        n = len(self.arr)
        for i in range(0, n, self.MIN_RUN):
            right = min(i + self.MIN_RUN - 1, n - 1)
            self._tim_insertion_sort(i, right)
        
        size = self.MIN_RUN
        while size < n:
            for start in range(0, n, size * 2):
                mid = start + size - 1
                right = min(start + size * 2 - 1, n - 1)
                if mid < right:
                    self._tim_merge(start, mid, right)
            size *= 2
    
    def _intro_sort(self):
        """Intro Sort - O(n log n)"""
        def intro_sort_helper(arr, max_depth):
            if len(arr) <= 1:
                return arr
            if max_depth == 0:
                return sorted(arr)  # Switch to heap sort
            
            pivot = arr[len(arr) // 2]
            left = [x for x in arr if x < pivot]
            mid = [x for x in arr if x == pivot]
            right = [x for x in arr if x > pivot]
            
            for _ in arr:
                self.compare()
            
            for item in left + mid + right:
                self.swaps += 1
            
            return intro_sort_helper(left, max_depth - 1) + mid + intro_sort_helper(right, max_depth - 1)
        
        max_depth = int(log2(len(self.arr))) * 2 if len(self.arr) > 0 else 0
        self.arr = intro_sort_helper(self.arr, max_depth)
    
    def is_sorted(self):
        """Check if array is sorted"""
        for i in range(len(self.arr) - 1):
            if self.arr[i] > self.arr[i + 1]:
                return False
        return True
    
    def sort(self):
        """Execute the selected sorting algorithm"""
        start_time = time.perf_counter()
        self._execute_sort()
        total_time = (time.perf_counter() - start_time) * 1000
        
        return {
            "algorithm": self.algorithm,
            "n": self.n,
            "input": self.original_arr,
            "output": self.arr,
            "actual_time_ms": round(total_time, 6),
            "comparisons": self.comparisons,
            "swaps": self.swaps,
            "is_sorted": self.is_sorted()
        }
    
    def _execute_sort(self):
        """Execute the selected sorting algorithm (helper method)"""
        if self.algorithm == 'bubble':
            self._bubble_sort()
        elif self.algorithm == 'selection':
            self._selection_sort()
        elif self.algorithm == 'insertion':
            self._insertion_sort()
        elif self.algorithm == 'merge':
            self._merge_sort()
        elif self.algorithm == 'quick':
            self._quick_sort()
        elif self.algorithm == 'heap':
            self._heap_sort()
        elif self.algorithm == 'counting':
            self._counting_sort()
        elif self.algorithm == 'radix':
            self._radix_sort()
        elif self.algorithm == 'bucket':
            self._bucket_sort()
        elif self.algorithm == 'tim':
            self._tim_sort()
        elif self.algorithm == 'intro':
            self._intro_sort()


def validate_input(arr):
    """Validate array constraints"""
    if len(arr) > MAX_ELEMENTS:
        print(f"Error: Maximum {MAX_ELEMENTS} elements allowed")
        sys.exit(1)
    
    for val in arr:
        if val < 0 or val > MAX_VALUE:
            print(f"Error: Element values must be between 0 and {MAX_VALUE}")
            sys.exit(1)


def mode_1(algorithm, values):
    """Mode 1: Sort custom values"""
    arr = [int(v) for v in values]
    validate_input(arr)
    
    analyzer = SortingAnalyzer(algorithm, arr)
    result = analyzer.sort()
    
    if result is None:
        print(f"Unknown algorithm: {algorithm}")
        sys.exit(1)
    
    print(json.dumps(result))


def mode_2(algorithm):
    """Mode 2: Performance testing with 50 random elements"""
    arr = [random.randint(0, MAX_VALUE) for _ in range(MAX_ELEMENTS)]
    
    analyzer = SortingAnalyzer(algorithm, arr)
    result = analyzer.sort()
    
    if result is None:
        print(f"Unknown algorithm: {algorithm}")
        sys.exit(1)
    
    print(json.dumps(result))


def main():
    """Main entry point"""
    if len(sys.argv) < 2:
        print("Usage: python sorting.py <algorithm> [value1 value2 ...]")
        print("Algorithms: bubble, selection, insertion, merge, quick, heap, counting, radix, bucket, tim, intro")
        sys.exit(1)
    
    algorithm = sys.argv[1]
    
    if len(sys.argv) > 2:
        # Mode 1: Custom values
        mode_1(algorithm, sys.argv[2:])
    else:
        # Mode 2: Performance testing
        mode_2(algorithm)


if __name__ == "__main__":
    main()

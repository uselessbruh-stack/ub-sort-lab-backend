using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text.Json;
using System.Text.Json.Serialization;

// Top-level statements (MUST come first in C#)
var cmdArgs = Environment.GetCommandLineArgs().Skip(1).ToArray();

if (cmdArgs.Length < 1)
{
    Console.WriteLine("Usage: SortingCSharp <algorithm> [value1 value2 ...]");
    Console.WriteLine("Algorithms: bubble, selection, insertion, merge, quick, heap, counting, radix, bucket, tim, intro");
    Environment.Exit(1);
}

var algorithm = cmdArgs[0];

if (cmdArgs.Length > 1)
{
    // Mode 1: Custom values
    try
    {
        var arr = cmdArgs.Skip(1).Select(v => uint.Parse(v)).ToArray();
        var analyzer = new SortingAnalyzer(algorithm, arr);
        var result = analyzer.Sort();
        Console.WriteLine(result);
    }
    catch (FormatException)
    {
        Console.Error.WriteLine("Error: Invalid number format");
        Environment.Exit(1);
    }
    catch (OperationCanceledException ex)
    {
        Console.Error.WriteLine(ex.Message);
        Environment.Exit(1);
    }
}
else
{
    // Mode 2: Performance testing with 50 random elements
    var rand = new Random(42);
    var arr = Enumerable.Range(0, Constants.MAX_ELEMENTS)
        .Select(_ => (uint)rand.Next(0, Constants.MAX_VALUE + 1))
        .ToArray();

    var analyzer = new SortingAnalyzer(algorithm, arr);
    var result = analyzer.Sort();
    Console.WriteLine(result);
}

// Class definitions come AFTER top-level statements
public static class Constants
{
    public const int MAX_ELEMENTS = 50;
    public const int MAX_VALUE = 1000;
}



public class SortingAnalyzer
{
    private string algorithm;
    private uint[] originalArr;
    private uint[] arr;
    private long comparisons;
    private long swaps;

    public SortingAnalyzer(string algorithm, uint[] arr)
    {
        this.algorithm = algorithm;
        this.originalArr = (uint[])arr.Clone();
        this.arr = (uint[])arr.Clone();
        this.comparisons = 0;
        this.swaps = 0;
    }

    private void Compare() => comparisons++;
    private void Swap(int i, int j)
    {
        var temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
        swaps++;
    }

    private void BubbleSort()
    {
        int n = arr.Length;
        for (int i = 0; i < n; i++)
        {
            bool swapped = false;
            for (int j = 0; j < n - i - 1; j++)
            {
                Compare();
                if (arr[j] > arr[j + 1])
                {
                    Swap(j, j + 1);
                    swapped = true;
                }
            }
            if (!swapped) break;
        }
    }

    private void SelectionSort()
    {
        int n = arr.Length;
        for (int i = 0; i < n; i++)
        {
            int minIdx = i;
            for (int j = i + 1; j < n; j++)
            {
                Compare();
                if (arr[j] < arr[minIdx])
                    minIdx = j;
            }
            Swap(i, minIdx);
        }
    }

    private void InsertionSort()
    {
        for (int i = 1; i < arr.Length; i++)
        {
            uint key = arr[i];
            int j = i - 1;
            while (j >= 0)
            {
                Compare();
                if (arr[j] > key)
                {
                    arr[j + 1] = arr[j];
                    swaps++;
                    j--;
                }
                else break;
            }
            arr[j + 1] = key;
        }
    }

    private void MergeSort()
    {
        arr = MergeSortHelper(arr, 0, arr.Length);
    }

    private uint[] MergeSortHelper(uint[] arr, int low, int high)
    {
        if (high <= 1) return arr;

        int mid = high / 2;
        var left = MergeSortHelper(arr.Take(mid).ToArray(), 0, mid);
        var right = MergeSortHelper(arr.Skip(mid).ToArray(), 0, high - mid);
        return Merge(left, right);
    }

    private uint[] Merge(uint[] left, uint[] right)
    {
        var result = new List<uint>();
        int i = 0, j = 0;

        while (i < left.Length && j < right.Length)
        {
            Compare();
            if (left[i] <= right[j])
            {
                result.Add(left[i++]);
            }
            else
            {
                result.Add(right[j++]);
            }
            swaps++;
        }

        for (int ii = i; ii < left.Length; ii++) { result.Add(left[ii]); swaps++; }
        for (int jj = j; jj < right.Length; jj++) { result.Add(right[jj]); swaps++; }
        return result.ToArray();
    }

    private void QuickSort()
    {
        if (arr.Length > 0)
            QuickSortHelper(0, arr.Length - 1);
    }

    private void QuickSortHelper(int low, int high)
    {
        if (low < high)
        {
            int pi = Partition(low, high);
            QuickSortHelper(low, pi - 1);
            QuickSortHelper(pi + 1, high);
        }
    }

    private int Partition(int low, int high)
    {
        uint pivot = arr[high];
        int i = low - 1;

        for (int j = low; j < high; j++)
        {
            Compare();
            if (arr[j] < pivot)
            {
                i++;
                Swap(i, j);
            }
        }
        Swap(i + 1, high);
        return i + 1;
    }

    private void HeapSort()
    {
        int n = arr.Length;
        for (int i = n / 2 - 1; i >= 0; i--)
            Heapify(i, n);

        for (int i = n - 1; i > 0; i--)
        {
            Swap(0, i);
            Heapify(0, i);
        }
    }

    private void Heapify(int i, int n)
    {
        while (true)
        {
            int largest = i;
            int left = 2 * i + 1;
            int right = 2 * i + 2;

            if (left < n)
            {
                Compare();
                if (arr[left] > arr[largest])
                    largest = left;
            }

            if (right < n)
            {
                Compare();
                if (arr[right] > arr[largest])
                    largest = right;
            }

            if (largest != i)
            {
                Swap(i, largest);
                i = largest;
            }
            else break;
        }
    }

    private void CountingSort()
    {
        if (arr.Length == 0) return;

        uint maxVal = arr.Max();
        uint minVal = arr.Min();
        int range = (int)(maxVal - minVal + 1);
        var count = new long[range];

        foreach (uint num in arr)
        {
            count[num - minVal]++;
            Compare();
        }

        int idx = 0;
        for (int i = 0; i < range; i++)
        {
            for (long j = 0; j < count[i]; j++)
            {
                arr[idx++] = (uint)(i + minVal);
                swaps++;
            }
        }
    }

    private void RadixSort()
    {
        if (arr.Length == 0) return;

        uint maxVal = arr.Max();
        uint exp = 1;

        while (maxVal / exp > 0)
        {
            CountingSortRadix(exp);
            exp *= 10;
        }
    }

    private void CountingSortRadix(uint exp)
    {
        int n = arr.Length;
        var output = new uint[n];
        var count = new long[10];

        foreach (uint num in arr)
        {
            int index = (int)((num / exp) % 10);
            count[index]++;
            Compare();
        }

        for (int i = 1; i < 10; i++)
            count[i] += count[i - 1];

        for (int i = n - 1; i >= 0; i--)
        {
            int index = (int)((arr[i] / exp) % 10);
            output[count[index] - 1] = arr[i];
            count[index]--;
            swaps++;
        }

        Array.Copy(output, arr, n);
    }

    private void BucketSort()
    {
        if (arr.Length == 0) return;

        uint maxVal = arr.Max();
        uint minVal = arr.Min();
        int bucketCount = arr.Length;
        double bucketRange = maxVal == minVal ? 1 : (maxVal - minVal) / (double)bucketCount;

        var buckets = new List<uint>[bucketCount];
        for (int i = 0; i < bucketCount; i++)
            buckets[i] = new List<uint>();

        foreach (uint num in arr)
        {
            int index = bucketRange > 0 ? (int)((num - minVal) / bucketRange) : 0;
            index = Math.Min(index, bucketCount - 1);
            buckets[index].Add(num);
            Compare();
        }

        int idx2 = 0;
        foreach (var bucket in buckets)
        {
            bucket.Sort();
            foreach (uint num in bucket)
            {
                arr[idx2++] = num;
                swaps++;
            }
        }
    }

    private const int MIN_RUN = 32;

    private void TimInsertionSort(int left, int right)
    {
        for (int i = left + 1; i <= right; i++)
        {
            uint key = arr[i];
            int j = i - 1;
            while (j >= left)
            {
                Compare();
                if (arr[j] > key)
                {
                    arr[j + 1] = arr[j];
                    swaps++;
                    j--;
                }
                else break;
            }
            arr[j + 1] = key;
        }
    }

    private void TimMerge(int left, int mid, int right)
    {
        var leftArr = arr[left..(mid + 1)];
        var rightArr = arr[(mid + 1)..(right + 1)];
        int i = 0, j = 0, k = left;
        while (i < leftArr.Length && j < rightArr.Length)
        {
            Compare();
            if (leftArr[i] <= rightArr[j])
                arr[k] = leftArr[i++];
            else
                arr[k] = rightArr[j++];
            swaps++;
            k++;
        }
        while (i < leftArr.Length) { arr[k++] = leftArr[i++]; swaps++; }
        while (j < rightArr.Length) { arr[k++] = rightArr[j++]; swaps++; }
    }

    private void TimSort()
    {
        int n = arr.Length;
        for (int i = 0; i < n; i += MIN_RUN)
        {
            int right = Math.Min(i + MIN_RUN - 1, n - 1);
            TimInsertionSort(i, right);
        }
        for (int size = MIN_RUN; size < n; size *= 2)
        {
            for (int start = 0; start < n; start += size * 2)
            {
                int mid = start + size - 1;
                int right = Math.Min(start + size * 2 - 1, n - 1);
                if (mid < right)
                    TimMerge(start, mid, right);
            }
        }
    }

    private void IntroSort()
    {
        int maxDepth = arr.Length > 0 ? (int)(Math.Log2(arr.Length)) * 2 : 0;
        arr = IntroSortHelper(arr, maxDepth);
    }

    private uint[] IntroSortHelper(uint[] arr, int maxDepth)
    {
        if (arr.Length <= 1) return arr;
        if (maxDepth == 0)
        {
            Array.Sort(arr);
            return arr;
        }

        uint pivot = arr[arr.Length / 2];
        var left = new List<uint>();
        var mid = new List<uint>();
        var right = new List<uint>();

        foreach (uint num in arr)
        {
            Compare();
            if (num < pivot) left.Add(num);
            else if (num == pivot) mid.Add(num);
            else right.Add(num);
            swaps++;
        }

        var leftArr = IntroSortHelper(left.ToArray(), maxDepth - 1);
        var rightArr = IntroSortHelper(right.ToArray(), maxDepth - 1);

        var result = new List<uint>();
        result.AddRange(leftArr);
        result.AddRange(mid);
        result.AddRange(rightArr);
        return result.ToArray();
    }

    private bool IsSorted()
    {
        for (int i = 0; i < arr.Length - 1; i++)
            if (arr[i] > arr[i + 1]) return false;
        return true;
    }

    private void ExecuteSort()
    {
        switch (algorithm)
        {
            case "bubble": BubbleSort(); break;
            case "selection": SelectionSort(); break;
            case "insertion": InsertionSort(); break;
            case "merge": MergeSort(); break;
            case "quick": QuickSort(); break;
            case "heap": HeapSort(); break;
            case "counting": CountingSort(); break;
            case "radix": RadixSort(); break;
            case "bucket": BucketSort(); break;
            case "tim": TimSort(); break;
            case "intro": IntroSort(); break;
            default: throw new ArgumentException($"Unknown algorithm: {algorithm}");
        }
    }

    public string Sort()
    {
        arr = (uint[])originalArr.Clone();
        comparisons = 0;
        swaps = 0;

        var sw = Stopwatch.StartNew();
        ExecuteSort();
        sw.Stop();

        double elapsed = sw.Elapsed.TotalMilliseconds;

        var options = new JsonSerializerOptions
        {
            WriteIndented = false
        };

        var output = new
        {
            algorithm = algorithm,
            n = originalArr.Length,
            input = originalArr,
            output = arr,
            actual_time_ms = Math.Round(elapsed, 6),
            comparisons = comparisons,
            swaps = swaps,
            is_sorted = IsSorted()
        };

        return JsonSerializer.Serialize(output, options);
    }
}

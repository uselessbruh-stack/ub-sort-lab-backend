use std::time::Instant;

const MAX_ELEMENTS: usize = 50;
const MAX_VALUE: u32 = 1000;

struct SortingAnalyzer {
    algorithm: String,
    original_arr: Vec<u32>,
    arr: Vec<u32>,
    comparisons: usize,
    swaps: usize,
}

impl SortingAnalyzer {
    fn new(algorithm: String, arr: Vec<u32>) -> Self {
        let original_arr = arr.clone();
        SortingAnalyzer {
            algorithm,
            original_arr,
            arr,
            comparisons: 0,
            swaps: 0,
        }
    }

    fn swap(&mut self, i: usize, j: usize) {
        self.arr.swap(i, j);
        self.swaps += 1;
    }

    fn compare(&mut self) {
        self.comparisons += 1;
    }

    fn bubble_sort(&mut self) {
        let n = self.arr.len();
        for i in 0..n {
            let mut swapped = false;
            for j in 0..n - i - 1 {
                self.compare();
                if self.arr[j] > self.arr[j + 1] {
                    self.swap(j, j + 1);
                    swapped = true;
                }
            }
            if !swapped {
                break;
            }
        }
    }

    fn selection_sort(&mut self) {
        let n = self.arr.len();
        for i in 0..n {
            let mut min_idx = i;
            for j in (i + 1)..n {
                self.compare();
                if self.arr[j] < self.arr[min_idx] {
                    min_idx = j;
                }
            }
            self.swap(i, min_idx);
        }
    }

    fn insertion_sort(&mut self) {
        let n = self.arr.len();
        for i in 1..n {
            let key = self.arr[i];
            let mut j = i as i32 - 1;
            while j >= 0 {
                self.compare();
                if self.arr[j as usize] > key {
                    self.arr[(j + 1) as usize] = self.arr[j as usize];
                    self.swaps += 1;
                    j -= 1;
                } else {
                    break;
                }
            }
            self.arr[(j + 1) as usize] = key;
        }
    }

    fn merge_sort(&mut self) {
        let n = self.arr.len();
        self.arr = self.merge_sort_helper(self.arr.clone(), 0, n);
    }

    fn merge_sort_helper(&mut self, mut arr: Vec<u32>, _low: usize, high: usize) -> Vec<u32> {
        if high <= 1 {
            return arr;
        }

        let mid = high / 2;
        let (left_arr, right_arr) = arr.split_at_mut(mid);
        let left = self.merge_sort_helper(left_arr.to_vec(), 0, left_arr.len());
        let right = self.merge_sort_helper(right_arr.to_vec(), 0, right_arr.len());
        self.merge(&left, &right)
    }

    fn merge(&mut self, left: &[u32], right: &[u32]) -> Vec<u32> {
        let mut result = Vec::new();
        let mut i = 0;
        let mut j = 0;

        while i < left.len() && j < right.len() {
            self.compare();
            if left[i] <= right[j] {
                result.push(left[i]);
                i += 1;
            } else {
                result.push(right[j]);
                j += 1;
            }
            self.swaps += 1;
        }

        result.extend_from_slice(&left[i..]);
        self.swaps += left.len() - i;
        result.extend_from_slice(&right[j..]);
        self.swaps += right.len() - j;
        result
    }

    fn quick_sort(&mut self) {
        let n = self.arr.len();
        if n > 0 {
            self.quick_sort_helper(0, (n - 1) as i32);
        }
    }

    fn quick_sort_helper(&mut self, low: i32, high: i32) {
        if low < high {
            let pi = self.partition(low, high);
            self.quick_sort_helper(low, pi - 1);
            self.quick_sort_helper(pi + 1, high);
        }
    }

    fn partition(&mut self, low: i32, high: i32) -> i32 {
        let pivot = self.arr[high as usize];
        let mut i = low - 1;

        for j in low..high {
            self.compare();
            if self.arr[j as usize] < pivot {
                i += 1;
                let i_idx = i as usize;
                let j_idx = j as usize;
                self.arr.swap(i_idx, j_idx);
                self.swaps += 1;
            }
        }

        let i_idx = (i + 1) as usize;
        let high_idx = high as usize;
        self.arr.swap(i_idx, high_idx);
        self.swaps += 1;
        i + 1
    }

    fn heap_sort(&mut self) {
        let n = self.arr.len();
        
        for i in (0..n / 2).rev() {
            self.heapify(i, n);
        }

        for i in (1..n).rev() {
            self.arr.swap(0, i);
            self.swaps += 1;
            self.heapify(0, i);
        }
    }

    fn heapify(&mut self, mut i: usize, n: usize) {
        loop {
            let mut largest = i;
            let left = 2 * i + 1;
            let right = 2 * i + 2;

            if left < n {
                self.compare();
                if self.arr[left] > self.arr[largest] {
                    largest = left;
                }
            }

            if right < n {
                self.compare();
                if self.arr[right] > self.arr[largest] {
                    largest = right;
                }
            }

            if largest != i {
                self.arr.swap(i, largest);
                self.swaps += 1;
                i = largest;
            } else {
                break;
            }
        }
    }

    fn counting_sort(&mut self) {
        if self.arr.is_empty() {
            return;
        }

        let max_val = *self.arr.iter().max().unwrap() as usize;
        let min_val = *self.arr.iter().min().unwrap() as usize;
        let range = max_val - min_val + 1;
        let mut count = vec![0; range];
        let arr_copy = self.arr.clone();

        for &num in &arr_copy {
            count[(num as usize) - min_val] += 1;
            self.compare();
        }

        let mut idx = 0;
        for i in 0..range {
            for _ in 0..count[i] {
                self.arr[idx] = (i + min_val) as u32;
                self.swaps += 1;
                idx += 1;
            }
        }
    }

    fn radix_sort(&mut self) {
        if self.arr.is_empty() {
            return;
        }

        let max_val = *self.arr.iter().max().unwrap();
        let mut exp = 1u32;

        while max_val / exp > 0 {
            self.counting_sort_radix(exp);
            exp *= 10;
        }
    }

    fn counting_sort_radix(&mut self, exp: u32) {
        let n = self.arr.len();
        let mut output = vec![0u32; n];
        let mut count = vec![0; 10];
        let arr_copy = self.arr.clone();

        for &num in &arr_copy {
            let index = ((num / exp) % 10) as usize;
            count[index] += 1;
            self.compare();
        }

        for i in 1..10 {
            count[i] += count[i - 1];
        }

        for i in (0..n).rev() {
            let index = ((arr_copy[i] / exp) % 10) as usize;
            output[count[index] - 1] = arr_copy[i];
            count[index] -= 1;
            self.swaps += 1;
        }

        self.arr = output;
    }

    fn bucket_sort(&mut self) {
        if self.arr.is_empty() {
            return;
        }

        let max_val = *self.arr.iter().max().unwrap() as f64;
        let min_val = *self.arr.iter().min().unwrap() as f64;
        let bucket_count = self.arr.len();
        let bucket_range = if max_val == min_val {
            1.0
        } else {
            (max_val - min_val) / bucket_count as f64
        };

        let mut buckets: Vec<Vec<u32>> = vec![Vec::new(); bucket_count];
        let arr_copy = self.arr.clone();

        for &num in &arr_copy {
            let num_f = num as f64;
            let index = if bucket_range > 0.0 {
                ((num_f - min_val) / bucket_range) as usize
            } else {
                0
            };
            let index = index.min(bucket_count - 1);
            buckets[index].push(num);
            self.compare();
        }

        self.arr.clear();
        for bucket in buckets.iter_mut() {
            bucket.sort();
            self.arr.extend_from_slice(bucket);
            self.swaps += bucket.len();
        }
    }

    fn tim_sort(&mut self) {
        let min_run = 32;
        let n = self.arr.len();
        
        // Sort individual runs with insertion sort
        let mut i = 0;
        while i < n {
            let right = if i + min_run - 1 < n { i + min_run - 1 } else { n - 1 };
            // Insertion sort for this run
            for j in (i + 1)..=right {
                let key = self.arr[j];
                let mut k = j as i32 - 1;
                while k >= i as i32 {
                    self.comparisons += 1;
                    if self.arr[k as usize] > key {
                        self.arr[(k + 1) as usize] = self.arr[k as usize];
                        self.swaps += 1;
                        k -= 1;
                    } else {
                        break;
                    }
                }
                self.arr[(k + 1) as usize] = key;
            }
            i += min_run;
        }
        
        // Merge runs
        let mut size = min_run;
        while size < n {
            let mut start = 0;
            while start < n {
                let mid = if start + size - 1 < n { start + size - 1 } else { n - 1 };
                let right = if start + size * 2 - 1 < n { start + size * 2 - 1 } else { n - 1 };
                if mid < right {
                    // Merge
                    let left_arr: Vec<u32> = self.arr[start..=mid].to_vec();
                    let right_arr: Vec<u32> = self.arr[mid + 1..=right].to_vec();
                    let mut i = 0;
                    let mut j = 0;
                    let mut k = start;
                    while i < left_arr.len() && j < right_arr.len() {
                        self.comparisons += 1;
                        if left_arr[i] <= right_arr[j] {
                            self.arr[k] = left_arr[i];
                            i += 1;
                        } else {
                            self.arr[k] = right_arr[j];
                            j += 1;
                        }
                        self.swaps += 1;
                        k += 1;
                    }
                    while i < left_arr.len() {
                        self.arr[k] = left_arr[i];
                        self.swaps += 1;
                        i += 1;
                        k += 1;
                    }
                    while j < right_arr.len() {
                        self.arr[k] = right_arr[j];
                        self.swaps += 1;
                        j += 1;
                        k += 1;
                    }
                }
                start += size * 2;
            }
            size *= 2;
        }
    }

    fn intro_sort(&mut self) {
        let max_depth = if self.arr.len() > 0 {
            ((self.arr.len() as f64).log2() as usize) * 2
        } else {
            0
        };
        self.arr = self.intro_sort_helper(self.arr.clone(), max_depth);
    }

    fn intro_sort_helper(&mut self, mut arr: Vec<u32>, max_depth: usize) -> Vec<u32> {
        if arr.len() <= 1 {
            return arr;
        }

        if max_depth == 0 {
            arr.sort();
            return arr;
        }

        let pivot = arr[arr.len() / 2];
        let mut left = Vec::new();
        let mut mid = Vec::new();
        let mut right = Vec::new();

        for &num in &arr {
            self.compare();
            if num < pivot {
                left.push(num);
            } else if num == pivot {
                mid.push(num);
            } else {
                right.push(num);
            }
            self.swaps += 1;
        }

        left = self.intro_sort_helper(left, max_depth - 1);
        right = self.intro_sort_helper(right, max_depth - 1);

        let mut result = left;
        result.extend_from_slice(&mid);
        result.extend_from_slice(&right);
        result
    }

    fn is_sorted(&self) -> bool {
        for i in 0..self.arr.len() - 1 {
            if self.arr[i] > self.arr[i + 1] {
                return false;
            }
        }
        true
    }

    fn execute_sort(&mut self) {
        match self.algorithm.as_str() {
            "bubble" => self.bubble_sort(),
            "selection" => self.selection_sort(),
            "insertion" => self.insertion_sort(),
            "merge" => self.merge_sort(),
            "quick" => self.quick_sort(),
            "heap" => self.heap_sort(),
            "counting" => self.counting_sort(),
            "radix" => self.radix_sort(),
            "bucket" => self.bucket_sort(),
            "tim" => self.tim_sort(),
            "intro" => self.intro_sort(),
            _ => {}
        }
    }

    fn sort(&mut self) -> String {
        // Sort the full array
        self.arr = self.original_arr.clone();
        self.comparisons = 0;
        self.swaps = 0;

        let start = Instant::now();
        self.execute_sort();
        let total_time_ms = start.elapsed().as_secs_f64() * 1000.0;

        // Build JSON output manually
        let mut json = String::from("{");
        json.push_str(&format!("\"algorithm\":\"{}\",", self.algorithm));
        json.push_str(&format!("\"n\":{},", self.original_arr.len()));
        
        json.push_str("\"input\":[");
        for (i, &v) in self.original_arr.iter().enumerate() {
            if i > 0 {
                json.push(',');
            }
            json.push_str(&v.to_string());
        }
        json.push_str("],");

        json.push_str("\"output\":[");
        for (i, &v) in self.arr.iter().enumerate() {
            if i > 0 {
                json.push(',');
            }
            json.push_str(&v.to_string());
        }
        json.push_str("],");

        let actual_time_rounded = (total_time_ms * 1_000_000.0).round() / 1_000_000.0;
        
        json.push_str(&format!("\"actual_time_ms\":{},", actual_time_rounded));
        json.push_str(&format!("\"comparisons\":{},", self.comparisons));
        json.push_str(&format!("\"swaps\":{},", self.swaps));
        json.push_str(&format!("\"is_sorted\":{}}}", if self.is_sorted() { "true" } else { "false" }));

        json
    }
}

fn validate_input(arr: &[u32]) -> Result<(), String> {
    if arr.len() > MAX_ELEMENTS {
        return Err(format!("Error: Maximum {} elements allowed", MAX_ELEMENTS));
    }

    for &val in arr {
        if val > MAX_VALUE {
            return Err(format!(
                "Error: Element values must be between 0 and {}",
                MAX_VALUE
            ));
        }
    }

    Ok(())
}

// Simple LCG pseudorandom number generator
struct SimpleRng {
    seed: u64,
}

impl SimpleRng {
    fn new(seed: u64) -> Self {
        SimpleRng { seed }
    }

    fn next(&mut self) -> u32 {
        self.seed = self.seed.wrapping_mul(1664525).wrapping_add(1013904223);
        (self.seed >> 32) as u32
    }

    fn range(&mut self, min: u32, max: u32) -> u32 {
        min + (self.next() % (max - min + 1))
    }
}

fn mode_1(algorithm: &str, values: &[String]) -> Result<(), String> {
    let arr: Result<Vec<u32>, _> = values.iter().map(|v| v.parse::<u32>()).collect();
    let arr = arr.map_err(|_| "Error: Invalid number format")?;

    validate_input(&arr)?;

    let mut analyzer = SortingAnalyzer::new(algorithm.to_string(), arr);
    let result = analyzer.sort();

    println!("{}", result);
    Ok(())
}

fn mode_2(algorithm: &str) -> Result<(), String> {
    let mut rng = SimpleRng::new(42);
    let arr: Vec<u32> = (0..MAX_ELEMENTS)
        .map(|_| rng.range(0, MAX_VALUE))
        .collect();

    let mut analyzer = SortingAnalyzer::new(algorithm.to_string(), arr);
    let result = analyzer.sort();

    println!("{}", result);
    Ok(())
}

fn main() {
    let args: Vec<String> = std::env::args().collect();

    if args.len() < 2 {
        eprintln!("Usage: sorting <algorithm> [value1 value2 ...]");
        eprintln!("Algorithms: bubble, selection, insertion, merge, quick, heap, counting, radix, bucket, tim, intro");
        std::process::exit(1);
    }

    let algorithm = &args[1];
    let result = if args.len() > 2 {
        // Mode 1: Custom values
        mode_1(algorithm, &args[2..])
    } else {
        // Mode 2: Performance testing
        mode_2(algorithm)
    };

    if let Err(e) = result {
        eprintln!("{}", e);
        std::process::exit(1);
    }
}

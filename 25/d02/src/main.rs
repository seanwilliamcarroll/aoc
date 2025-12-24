use common::read_from_file;
use std::collections::HashSet;

type Unit = usize;

struct Range {
    lower: Unit,
    upper: Unit,
}

impl Range {
    fn from_line(line: &str) -> Self {
        let (lower_str, upper_str) = line.split_once('-').expect("Bad formatting!");

        let lower = lower_str.parse::<Unit>().expect("Bad formatting!!");
        let upper = upper_str.parse::<Unit>().expect("Bad formatting!!");

        Self { lower, upper }
    }

    fn in_range(&self, value: Unit) -> bool {
        self.lower <= value && value <= self.upper
    }

    fn count_digits(value: Unit) -> Unit {
        (value as f64).log10().floor() as usize + 1
    }

    fn ten_to(exp: Unit) -> Unit {
        10u64.pow(exp as u32) as Unit
    }

    fn get_upper_half(value: Unit) -> Unit {
        let digit_count = Self::count_digits(value);
        assert!(digit_count.is_multiple_of(2));
        value / Self::ten_to(digit_count / 2)
    }

    fn next_repeated_digits<F>(value: Unit, offset_func: F) -> Unit
    where
        F: Fn(Unit) -> Unit,
    {
        let digit_count = Self::count_digits(value);
        assert!(digit_count.is_multiple_of(2));
        let each_half = value / Self::ten_to(digit_count / 2);
        let each_half = offset_func(each_half);
        (each_half * Self::ten_to(digit_count / 2)) + each_half
    }

    fn sum_invalid_ids(&self) -> usize {
        // println!("\noriginal: [{}, {}]", self.lower, self.upper);

        let mut lower_bound = self.lower;
        let mut upper_bound = self.upper;

        let lower_digit_count = Self::count_digits(self.lower);
        let upper_digit_count = Self::count_digits(self.upper);

        if lower_digit_count == upper_digit_count && lower_digit_count % 2 == 1 {
            return 0usize;
        }

        let lower_digit_count = if lower_digit_count.is_multiple_of(2) {
            lower_digit_count
        } else {
            lower_bound = Self::ten_to(lower_digit_count);
            lower_digit_count + 1
        };

        if lower_digit_count > upper_digit_count {
            return 0usize;
        }

        if upper_digit_count % 2 == 1 {
            upper_bound = Self::ten_to(upper_digit_count - 1) - 1;
        }

        // println!(
        //     "lower_bound: {} ({} digits) upper_bound: {} ({} digits)",
        //     lower_bound, lower_digit_count, upper_bound, upper_digit_count
        // );

        // Check ends
        let mut lower_end = Self::next_repeated_digits(lower_bound, |x| x);
        let mut upper_end = Self::next_repeated_digits(upper_bound, |x| x);

        // println!("[{lower_end}, {upper_end}]");

        if !self.in_range(lower_end) {
            // count += lower_end;
            lower_end = Self::next_repeated_digits(lower_bound, |x| x + 1);
        }

        if !self.in_range(upper_end) {
            upper_end = Self::next_repeated_digits(upper_bound, |x| x - 1);
            // count += upper_end;
        }
        // println!("[{lower_end}, {upper_end}]");

        if lower_end > upper_end {
            // println!("invalid");
            return 0usize;
        }

        if Self::count_digits(lower_end) != Self::count_digits(upper_end) {
            panic!("Need to handle this case");
        }

        let minimum_upper_half = Self::get_upper_half(lower_end);

        let maximum_upper_half = Self::get_upper_half(upper_end);

        // println!("minimum_upper_half: {minimum_upper_half}");
        // println!("maximum_upper_half: {maximum_upper_half}");

        let num_terms = maximum_upper_half - minimum_upper_half + 1;

        let total_sum = if num_terms == 1 {
            minimum_upper_half
        } else {
            (num_terms) * (minimum_upper_half + maximum_upper_half) / 2
        };

        let multiplier = Self::ten_to(Self::count_digits(minimum_upper_half)) + 1;

        // println!("{total_sum} * {multiplier} = {}", total_sum * multiplier);

        // println!();
        total_sum * multiplier
    }

    fn sum_all_invalid_ids(&self) -> usize {
        let num_digits = Self::count_digits(self.lower);

        let mut total_sum = 0usize;

        // println!("Num digits: {num_digits}");

        let mut seen_so_far: HashSet<Unit> = HashSet::new();

        for digit_count in num_digits..=(Self::count_digits(self.upper)) {
            for divisor in 1..=(digit_count / 2) {
                if divisor != 1 && digit_count % divisor != 0 {
                    continue;
                }
                // println!(
                //     "Try all repeaters with {} x {} digits",
                //     digit_count / divisor,
                //     divisor
                // );

                let mut current_repeated = 0;
                for _ in 0..(digit_count / divisor) {
                    current_repeated =
                        current_repeated * Self::ten_to(divisor) + Self::ten_to(divisor - 1);
                    // println!("current_repeated: {current_repeated}");
                }

                let increment = current_repeated / Self::ten_to(divisor - 1);

                // println!("current_repeated: {current_repeated} increment: {increment}");

                while current_repeated <= self.upper
                    && current_repeated <= Self::ten_to(digit_count)
                {
                    if self.in_range(current_repeated)
                        && Self::count_digits(current_repeated) % divisor == 0
                        && !seen_so_far.contains(&current_repeated)
                    {
                        // println!("{current_repeated}");
                        total_sum += current_repeated;
                        seen_so_far.insert(current_repeated);
                    }

                    current_repeated += increment;
                }
            }
        }

        total_sum
    }
}

fn main() -> std::io::Result<()> {
    let raw_lines = read_from_file("input.txt")?;

    println!("Found {} lines", raw_lines.len());

    assert!(raw_lines.len() == 1);

    let raw_line = &raw_lines[0];

    let ranges = raw_line
        .split(',')
        .map(Range::from_line)
        .collect::<Vec<Range>>();

    println!("Found {} ranges", ranges.len());

    let mut p1_solution = 0usize;
    let mut p2_solution = 0usize;
    for range in &ranges {
        let sum = range.sum_invalid_ids();
        // if sum > 0 {
        //     println!("Range has {} total", sum);
        // }
        p1_solution += sum;

        // println!("[{}, {}]", range.lower, range.upper);

        // break;
        p2_solution += range.sum_all_invalid_ids();
    }

    println!("P1 Solution: {p1_solution}");
    println!("P2 Solution: {p2_solution}");

    Ok(())
}

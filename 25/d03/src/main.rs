use common::{get_input_file, read_from_file};

fn find_max_joltage(line: &str) -> usize {
    // Find max element and it's index in first n-1 of len n str

    let mut max_value: usize = 0;

    let mut max_index: usize = 0;

    for (index, value) in line.chars().take(line.len() - 1).enumerate() {
        let value = value.to_digit(10).expect("Bad formatting!") as usize;

        if value > max_value {
            max_value = value;
            max_index = index;
        }
    }

    // find max element following that element

    let mut second_max_value: usize = 0;

    for value in line.chars().rev().take(line.len() - max_index - 1) {
        let value = value.to_digit(10).expect("Bad formatting") as usize;
        if value > second_max_value {
            second_max_value = value;
        }
    }

    max_value * 10 + second_max_value
}

fn find_super_max_joltage(line: &str) -> usize {
    // Need to find max number of length 12

    let mut joltage: usize = 0;

    // Need to find first max in first n-12
    // Then subsequence maxes in max of elements in [last_max, n-11], ....

    let mut to_skip: usize = 0;
    let mut num_to_keep: usize = 12;

    for _ in 0..12 {
        // On an iteration, want to look at all numbers after our most recent max index
        // But only want to look at those until we only have 12-iterations-1 digits remaining

        let mut max_value: usize = 0;

        let mut max_index: usize = 0;

        num_to_keep -= 1;

        for (index, value) in line
            .chars()
            .skip(to_skip)
            .take(line.len() - to_skip - num_to_keep)
            .enumerate()
        {
            let value = value.to_digit(10).expect("Bad formatting!") as usize;

            if value > max_value {
                max_value = value;
                max_index = index;
            }
        }

        joltage = joltage * 10 + max_value;

        to_skip += max_index;
        to_skip += 1;
    }

    joltage
}

fn main() -> std::io::Result<()> {
    let filepath = get_input_file()?;
    let raw_lines = read_from_file(&filepath)?;

    println!("Found {} lines", raw_lines.len());

    let p1_solution: usize = raw_lines
        .iter()
        .map(|line| find_max_joltage(line))
        .sum::<usize>();

    println!("P1 Solution: {p1_solution}");

    let p2_solution: usize = raw_lines
        .iter()
        .map(|line| find_super_max_joltage(line))
        .sum::<usize>();

    println!("P2 Solution: {p2_solution}");

    Ok(())
}

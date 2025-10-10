use std::fs::File;
use std::io::{self, BufRead};

type RawLines = Vec<String>;

fn read_from_file(filepath: &str) -> std::io::Result<RawLines> {
    let file = File::open(filepath)?;
    io::BufReader::new(file).lines().collect()
}

fn to_nums(line: &str) -> Vec<i64> {
    line.split(" ")
        .map(|num| num.parse::<i64>().expect("Bad formatting"))
        .collect::<Vec<i64>>()
}

fn main() -> std::io::Result<()> {
    let raw_lines = read_from_file("input.txt")?;

    println!("Found {} instructions", raw_lines.len());

    let mut p1_solution = 0;
    let mut p2_solution = 0;

    for line in &raw_lines {
        let mut nums = to_nums(line);
        let mut all_rows: Vec<Vec<i64>> = vec![];

        all_rows.push(nums.clone());

        while !nums.iter().fold(true, |acc, &next| acc && (next == 0)) {
            let mut next_row = vec![];
            let mut last = nums[0];
            for next in &nums[1..] {
                next_row.push(next - last);
                last = *next
            }

            nums = next_row.clone();
            all_rows.push(next_row);
        }

        all_rows.reverse();

        let mut last_new_entry = 0;
        let mut last_new_begin_entry = 0;

        for row in all_rows[1..].into_iter() {
            last_new_begin_entry = row[0] - last_new_begin_entry;
            last_new_entry += row[row.len() - 1];
        }
        p1_solution += last_new_entry;
        p2_solution += last_new_begin_entry;
    }

    println!("P1 solution: {p1_solution}");
    println!("P2 solution: {p2_solution}");

    Ok(())
}

use std::fs::File;
use std::io::{self, BufRead};

type RawLines = Vec<String>;

fn read_from_file(filepath: &str) -> std::io::Result<RawLines> {
    let file = File::open(filepath)?;
    io::BufReader::new(file).lines().collect()
}

fn parse_numbers(line: &str, initial_string: &str) -> Vec<usize> {
    line.trim_start_matches(initial_string)
        .trim_start_matches(' ')
        .split(' ')
        .filter(|line| !line.is_empty())
        .map(|line| line.parse::<usize>().expect("Should parse"))
        .collect::<Vec<usize>>()
}

fn parse_as_one_number(line: &str, initial_string: &str) -> usize {
    line.trim_start_matches(initial_string)
        .chars()
        .filter(|c| !c.is_whitespace())
        .collect::<String>()
        .parse::<usize>()
        .expect("Should parse")
}

fn calculate_score(time: usize, hold_time: usize) -> usize {
    (time - hold_time) * hold_time
}

fn find_greater(value: f64, time: usize, distance: usize) -> usize {
    let floor = value.floor() as usize;
    let ceil = value.ceil() as usize;
    if calculate_score(time, floor) > distance {
        floor
    } else {
        assert!(calculate_score(time, ceil) > distance);
        ceil
    }
}

fn find_roots(a: f64, b: f64, c: f64) -> (f64, f64) {
    let sqrt_term = (b.powf(2f64) - (4f64 * a * c)).sqrt();
    let first = (-b + sqrt_term) / (2f64 * a);
    let second = (-b - sqrt_term) / (2f64 * a);
    (first, second)
}

fn find_num_greater(time: usize, distance: usize) -> usize {
    let (first_root, second_root) = find_roots(-1f64, time as f64, -(distance as f64));
    let first_greatest = find_greater(first_root, time, distance);
    let second_greatest = find_greater(second_root, time, distance);
    second_greatest - first_greatest + 1
}

fn main() -> std::io::Result<()> {
    let raw_lines = read_from_file("input.txt")?;

    for line in &raw_lines {
        println!("{line}");
    }

    let times = parse_numbers(&raw_lines[0], "Time: ");
    let distances = parse_numbers(&raw_lines[1], "Distance: ");

    let list = times
        .iter()
        .zip(distances.iter())
        .map(|(a, b)| (*a, *b))
        .collect::<Vec<(usize, usize)>>();

    let mut p1_out = 1usize;
    for (time, distance) in list {
        p1_out *= find_num_greater(time, distance);
    }

    println!("P1 output: {p1_out}");

    let single_time = parse_as_one_number(&raw_lines[0], "Time: ");
    let single_distance = parse_as_one_number(&raw_lines[1], "Distance: ");

    println!("Time:     {single_time:>15}");
    println!("Distance: {single_distance:>15}");

    println!(
        "P2 number of ways: {}",
        find_num_greater(single_time, single_distance)
    );

    Ok(())
}

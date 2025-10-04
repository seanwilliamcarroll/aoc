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

fn simulate_scores(time: usize) -> Vec<usize> {
    let mut output = vec![];
    for held in 0..=time {
        output.push((time - held) * held);
    }
    output
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
        println!("In time {time}, cover {distance}");
        let scores = simulate_scores(time);
        let better = scores
            .iter()
            .filter(|score| **score > distance)
            .cloned()
            .collect::<Vec<usize>>()
            .len();
        p1_out *= better;
    }

    println!("P1 output: {p1_out}");

    Ok(())
}

use std::fs::File;
use std::io::{self, BufRead};

type RawLines = Vec<String>;

fn read_from_file(filepath: &str) -> std::io::Result<RawLines> {
    let file = File::open(filepath)?;
    io::BufReader::new(file).lines().collect()
}

fn hash_algorithm(line: &str) -> usize {
    line.bytes()
        .fold(0usize, |acc, num| ((acc + num as usize) * 17) % 256)
}

fn main() -> std::io::Result<()> {
    let raw_lines = read_from_file("input.txt")?;

    println!("Found {} lines", raw_lines.len());

    let raw_line = &raw_lines[0];

    let p1_solution: usize = raw_line.split(',').map(hash_algorithm).sum();

    println!("P1 solution: {p1_solution}");

    Ok(())
}

use std::fs::File;
use std::io::{self, BufRead};

type RawLines = Vec<String>;

fn read_from_file(filepath: &str) -> std::io::Result<RawLines> {
    let file = File::open(filepath)?;
    io::BufReader::new(file).lines().collect()
}

#[derive(Debug, Eq, PartialEq)]
struct Position(usize, usize);

impl Position {
    fn from_line(line: &str) -> Self {
        let (p0_str, p1_str) = line.split_once(',').expect("Bad formatting!");

        Position(
            p0_str.parse::<usize>().expect("Formatting!"),
            p1_str.parse::<usize>().expect("Formatting!"),
        )
    }
}

fn largest_rectangle(positions: Vec<Position>) -> usize {
    let mut largest = 0usize;
    for position_0 in &positions {
        for position_1 in &positions {
            if position_0 == position_1 {
                continue;
            }
            let min_x = usize::min(position_0.0, position_1.0);
            let max_x = usize::max(position_0.0, position_1.0);
            let min_y = usize::min(position_0.1, position_1.1);
            let max_y = usize::max(position_0.1, position_1.1);

            let area = (min_x.abs_diff(max_x) + 1) * (min_y.abs_diff(max_y) + 1);
            if area > largest {
                largest = area;
            }
        }
    }
    largest
}

fn main() -> std::io::Result<()> {
    let raw_lines = read_from_file("input.txt")?;

    println!("Found {} lines", raw_lines.len());

    let positions = raw_lines
        .iter()
        .map(|line| Position::from_line(line))
        .collect::<Vec<Position>>();

    // for pos in &positions {
    //     println!("{pos:?}");
    // }

    let p1_solution = largest_rectangle(positions);

    println!("P1 Solution: {p1_solution}");

    Ok(())
}

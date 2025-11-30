use std::fs::File;
use std::io::{self, BufRead};

type RawLines = Vec<String>;

fn read_from_file(filepath: &str) -> std::io::Result<RawLines> {
    let file = File::open(filepath)?;
    io::BufReader::new(file).lines().collect()
}

#[derive(Debug)]
enum Direction {
    Up,
    Right,
    Down,
    Left,
}

impl Direction {
    fn from_line(line: &str) -> Self {
        assert!(line.len() == 1);

        match line.chars().next().expect("Already checked length") {
            'U' => Self::Up,
            'R' => Self::Right,
            'D' => Self::Down,
            'L' => Self::Left,
            _ => panic!("Unknown Character!"),
        }
    }
}

type Distance = i64;

struct Instruction {
    direction: Direction,
    steps: Distance,
}

impl Instruction {
    fn from_raw_line(line: String) -> Self {
        let (direction_str, remaining) = line.split_once(' ').expect("Bad formatting!");

        let direction = Direction::from_line(direction_str);
        let (steps_str, _) = remaining.split_once(' ').expect("Bad formatting!");

        let steps = steps_str.parse::<Distance>().expect("Bad formatting");

        Self { direction, steps }
    }

    fn from_raw_hex_line(line: String) -> Self {
        let (_, remaining) = line.split_once(' ').expect("Bad formatting!");

        let (_, hex_str) = remaining.split_once(' ').expect("Bad formatting!");

        let hex_str = hex_str.trim_matches(['(', ')', '#']);

        assert!(hex_str.len() == 6);

        let (steps_str, dir_str) = hex_str.split_at(5);

        let direction = match dir_str {
            "0" => Direction::Right,
            "1" => Direction::Down,
            "2" => Direction::Left,
            "3" => Direction::Up,
            _ => panic!("Unexpected char"),
        };

        let steps = Distance::from_str_radix(steps_str, 16).expect("Bad formatting");

        Self { direction, steps }
    }

    fn from_raw_lines<F>(lines: RawLines, func: F) -> Vec<Self>
    where
        F: Fn(String) -> Self,
    {
        lines
            .into_iter()
            .map(|line| func(line))
            .collect::<Vec<Self>>()
    }
}

fn get_vertices_and_return_area(instructions: &Vec<Instruction>) -> Distance {
    // Simulate digger
    let (mut row_position, mut col_position) = (0, 0);

    let (mut min_row, mut max_row) = (Distance::MAX, Distance::MIN);
    let (mut min_col, mut max_col) = (Distance::MAX, Distance::MIN);

    let mut points: Vec<(Distance, Distance)> = vec![];

    for instruction in instructions {
        match instruction.direction {
            Direction::Up => {
                row_position += instruction.steps;
            }
            Direction::Right => {
                col_position += instruction.steps;
            }
            Direction::Down => {
                row_position -= instruction.steps;
            }
            Direction::Left => {
                col_position -= instruction.steps;
            }
        }
        min_row = Distance::min(row_position, min_row);
        max_row = Distance::max(row_position, max_row);
        min_col = Distance::min(col_position, min_col);
        max_col = Distance::max(col_position, max_col);
        points.push((row_position, col_position));
    }

    let points = points
        .iter()
        .map(|(row, col)| (row + min_row.abs(), col + min_col.abs()))
        .collect::<Vec<(Distance, Distance)>>();

    // Try trapezoid formula for simple polygons?

    let mut output: Distance = 0;
    for index in 0..points.len() {
        let (row_0, col_0) = points[index];
        let (row_1, col_1) = points[(index + 1) % points.len()];

        output += (col_0 + col_1) * (row_0 - row_1);
    }

    output = output / 2;

    let mut perimeter = 0;

    // Need to add the perimeter one more time

    for index in 0..points.len() {
        let (row_0, col_0) = points[index];
        let (row_1, col_1) = points[(index + 1) % points.len()];

        let side_length = (col_1 - col_0).abs() + (row_1 - row_0).abs();

        perimeter += side_length;
    }

    // Weird conversion of perimeter due to how we count "squares" vs normal cartesian area

    1 + output + perimeter / 2
}

fn main() -> std::io::Result<()> {
    let raw_lines = read_from_file("input.txt")?;

    println!("Found {} lines", raw_lines.len());

    let instructions = Instruction::from_raw_lines(raw_lines.clone(), Instruction::from_raw_line);

    println!("Found {} instructions", instructions.len());

    let p1_solution = get_vertices_and_return_area(&instructions);

    println!("P1 Solution: {p1_solution}");

    let instructions = Instruction::from_raw_lines(raw_lines, Instruction::from_raw_hex_line);

    println!("Found {} instructions", instructions.len());

    let p2_solution = get_vertices_and_return_area(&instructions);

    println!("P2 Solution: {p2_solution}");

    Ok(())
}

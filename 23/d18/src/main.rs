use std::collections::VecDeque;
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
    color_string: String,
}

impl Instruction {
    fn from_raw_line(line: String) -> Self {
        let (direction_str, remaining) = line.split_once(' ').expect("Bad formatting!");

        let direction = Direction::from_line(direction_str);
        let (steps_str, color_str) = remaining.split_once(' ').expect("Bad formatting!");

        let steps = steps_str.parse::<Distance>().expect("Bad formatting");

        Self {
            direction,
            steps,
            color_string: color_str.to_string(),
        }
    }

    fn from_raw_lines(lines: RawLines) -> Vec<Self> {
        lines
            .into_iter()
            .map(|line| Self::from_raw_line(line))
            .collect::<Vec<Self>>()
    }
}

fn grid_from_instructions(instructions: &Vec<Instruction>) -> Vec<Vec<char>> {
    // Simulate digger
    let (mut row_position, mut col_position) = (0, 0);

    let (mut min_row, mut max_row) = (Distance::MAX, Distance::MIN);
    let (mut min_col, mut max_col) = (Distance::MAX, Distance::MIN);

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
    }

    let mut grid: Vec<Vec<char>> =
        vec![vec!['.'; (max_col - min_col + 1) as usize]; (max_row - min_row + 1) as usize];

    row_position = min_row.abs();
    col_position = min_col.abs();

    for instruction in instructions {
        for _ in 0..instruction.steps {
            match instruction.direction {
                Direction::Up => {
                    row_position += 1;
                }
                Direction::Right => {
                    col_position += 1;
                }
                Direction::Down => {
                    row_position -= 1;
                }
                Direction::Left => {
                    col_position -= 1;
                }
            }
            grid[row_position as usize][col_position as usize] = '#';
            assert!(row_position >= 0);
            assert!(col_position >= 0);
        }
    }

    let mut next_square: VecDeque<(usize, usize)> = VecDeque::new();

    next_square.push_back((row_position as usize, (col_position + 1) as usize));

    while let Some((new_row_index, new_col_index)) = next_square.pop_front() {
        if grid[new_row_index][new_col_index] == '#' {
            continue;
        }
        grid[new_row_index][new_col_index] = '#';

        if new_row_index > 0 && grid[new_row_index - 1][new_col_index] != '#' {
            next_square.push_back((new_row_index - 1, new_col_index));
        }
        if new_col_index > 0 && grid[new_row_index][new_col_index - 1] != '#' {
            next_square.push_back((new_row_index, new_col_index - 1));
        }
        if new_row_index < grid.len() - 1 && grid[new_row_index + 1][new_col_index] != '#' {
            next_square.push_back((new_row_index + 1, new_col_index));
        }
        if new_col_index < grid[0].len() - 1 && grid[new_row_index][new_col_index + 1] != '#' {
            next_square.push_back((new_row_index, new_col_index + 1));
        }
    }
    grid
}

fn main() -> std::io::Result<()> {
    let raw_lines = read_from_file("input.txt")?;

    println!("Found {} lines", raw_lines.len());

    let instructions = Instruction::from_raw_lines(raw_lines);

    println!("Found {} instructions", instructions.len());

    let grid = grid_from_instructions(&instructions);

    let p1_solution = grid
        .iter()
        .map(|line| {
            line.iter()
                .map(|tile| (*tile == '#') as usize)
                .sum::<usize>()
        })
        .sum::<usize>();

    println!("P1 Solution: {p1_solution}");

    Ok(())
}

use common::{RawLines, read_from_file};

type Measure = usize;

const NUM_TICKS_DIAL: Measure = 100;

enum Direction {
    Left,
    Right,
}

struct Turn {
    direction: Direction,
    ticks: Measure,
}

impl Turn {
    fn from_raw_line(line: String) -> Self {
        let (first_char_str, numerical_str) = line.split_at(1);

        let direction = match first_char_str {
            "L" => Direction::Left,
            "R" => Direction::Right,
            _ => panic!("Bad formatting!"),
        };

        let ticks = numerical_str.parse::<Measure>().expect("Bad formatting!");

        Self { direction, ticks }
    }

    fn from_raw_lines(raw_lines: RawLines) -> Vec<Self> {
        raw_lines
            .into_iter()
            .map(Self::from_raw_line)
            .collect::<Vec<Self>>()
    }
}

fn count_zeros(turns: &Vec<Turn>) -> Measure {
    let mut zeros: Measure = 0;

    let mut dial_pos: Measure = 50;

    for turn in turns {
        match turn.direction {
            Direction::Left => {
                dial_pos =
                    (dial_pos + (NUM_TICKS_DIAL - (turn.ticks % NUM_TICKS_DIAL))) % NUM_TICKS_DIAL;
            }
            Direction::Right => {
                dial_pos = (dial_pos + turn.ticks) % NUM_TICKS_DIAL;
            }
        }
        if dial_pos == 0 {
            zeros += 1;
        }
    }

    zeros
}

fn count_all_zeros(turns: &Vec<Turn>) -> Measure {
    let mut zeros: Measure = 0;

    let mut dial_pos: Measure = 50;

    for turn in turns {
        let rotations = turn.ticks / NUM_TICKS_DIAL;
        let remainder = turn.ticks % NUM_TICKS_DIAL;

        match turn.direction {
            Direction::Left => {
                if dial_pos != 0 && remainder > dial_pos {
                    zeros += 1;
                }
                dial_pos = (dial_pos + (NUM_TICKS_DIAL - (remainder))) % NUM_TICKS_DIAL;
            }
            Direction::Right => {
                if remainder + dial_pos > NUM_TICKS_DIAL {
                    zeros += 1;
                }
                dial_pos = (dial_pos + remainder) % NUM_TICKS_DIAL;
            }
        }
        if dial_pos == 0 {
            zeros += 1;
        }
        zeros += rotations;
    }

    zeros
}

fn main() -> std::io::Result<()> {
    let raw_lines = read_from_file("input.txt")?;

    println!("Found {} lines", raw_lines.len());

    let turns = Turn::from_raw_lines(raw_lines);

    println!("{} turns", turns.len());

    let p1_solution = count_zeros(&turns);

    println!("P1 Solution: {p1_solution}");

    let p2_solution = count_all_zeros(&turns);

    println!("P2 Solution: {p2_solution}");

    Ok(())
}

use std::collections::{HashSet, VecDeque};
use std::fs::File;
use std::io::{self, BufRead};

type RawLines = Vec<String>;

fn read_from_file(filepath: &str) -> std::io::Result<RawLines> {
    let file = File::open(filepath)?;
    io::BufReader::new(file).lines().collect()
}

type Tile = char;

const HORIZONTAL_SPLITTER: Tile = '-';
const VERTICAL_SPLITTER: Tile = '|';
const NE_SW_MIRROR: Tile = '/';
const NW_SE_MIRROR: Tile = '\\';

type Row<T> = Vec<T>;
type Grid<T> = Vec<Row<T>>;

type Coordinate = (usize, usize);

#[derive(Eq, Hash, PartialEq, Clone)]
enum Direction {
    North,
    East,
    South,
    West,
}

impl Direction {
    fn next_directions(self, tile: Tile) -> Vec<Self> {
        match tile {
            HORIZONTAL_SPLITTER => match self {
                Direction::East | Direction::West => vec![self],
                Direction::North | Direction::South => {
                    vec![Direction::East, Direction::West]
                }
            },
            VERTICAL_SPLITTER => match self {
                Direction::North | Direction::South => {
                    vec![self]
                }
                Direction::East | Direction::West => {
                    vec![Direction::North, Direction::South]
                }
            },
            NE_SW_MIRROR => match self {
                Direction::North => {
                    vec![Direction::East]
                }
                Direction::East => {
                    vec![Direction::North]
                }
                Direction::South => {
                    vec![Direction::West]
                }
                Direction::West => {
                    vec![Direction::South]
                }
            },
            NW_SE_MIRROR => match self {
                Direction::North => {
                    vec![Direction::West]
                }
                Direction::East => {
                    vec![Direction::South]
                }
                Direction::South => {
                    vec![Direction::East]
                }
                Direction::West => {
                    vec![Direction::North]
                }
            },
            _ => vec![self],
        }
    }
}

#[derive(Eq, Hash, PartialEq, Clone)]
struct Heading {
    direction: Direction,
    coordinate: Coordinate,
}

struct MirrorField {
    grid: Grid<Tile>,
    energized: Grid<bool>,
    seen_headings: HashSet<Heading>,
}

impl MirrorField {
    fn from_raw_lines(lines: &RawLines) -> Self {
        let grid = lines
            .iter()
            .map(|line| line.chars().collect::<Row<Tile>>())
            .collect::<Grid<Tile>>();
        let energized: Grid<bool> = vec![vec![false; grid[0].len()]; grid.len()];
        Self {
            grid,
            energized,
            seen_headings: HashSet::new(),
        }
    }

    fn print_size(&self) {
        println!("{} x {} grid", self.grid.len(), self.grid[0].len());
    }

    fn next_space(&self, heading: Heading) -> Option<Heading> {
        let Heading {
            direction,
            coordinate,
        } = heading;

        let (row_index, col_index) = coordinate;

        match direction {
            Direction::North => {
                if row_index == 0 {
                    None
                } else {
                    Some(Heading {
                        direction,
                        coordinate: (row_index - 1, col_index),
                    })
                }
            }
            Direction::East => {
                if col_index == self.grid[0].len() - 1 {
                    None
                } else {
                    Some(Heading {
                        direction,
                        coordinate: (row_index, col_index + 1),
                    })
                }
            }
            Direction::South => {
                if row_index == self.grid.len() - 1 {
                    None
                } else {
                    Some(Heading {
                        direction,
                        coordinate: (row_index + 1, col_index),
                    })
                }
            }
            Direction::West => {
                if col_index == 0 {
                    None
                } else {
                    Some(Heading {
                        direction,
                        coordinate: (row_index, col_index - 1),
                    })
                }
            }
        }
    }

    fn step_light(&mut self, heading: Heading) -> Vec<Heading> {
        // Light has just entered heading
        // Need to mark cell as energized and return new headings as beam continues

        if !self.seen_headings.insert(heading.clone()) {
            return vec![];
        }

        let Heading {
            direction,
            coordinate,
        } = heading;
        let (row_index, col_index) = coordinate;

        self.energized[row_index][col_index] = true;

        // Get next paths
        let mut output = vec![];

        let tile = self.grid[row_index][col_index];

        for new_direction in direction.next_directions(tile) {
            if let Some(new_heading) = self.next_space(Heading {
                direction: new_direction,
                coordinate,
            }) {
                output.push(new_heading);
            }
        }

        output
    }

    fn count_energized(&self) -> usize {
        self.energized
            .iter()
            .map(|row| row.iter().map(|value| *value as usize).sum::<usize>())
            .sum::<usize>()
    }

    fn simulate_laser(&mut self) {
        let mut queue: VecDeque<Heading> = VecDeque::new();

        queue.push_back(Heading {
            direction: Direction::East,
            coordinate: (0, 0),
        });

        while let Some(new_heading) = queue.pop_front() {
            let new_headings = self.step_light(new_heading);
            queue.extend(new_headings.into_iter());
        }
    }
}

fn main() -> std::io::Result<()> {
    let raw_lines = read_from_file("input.txt")?;

    println!("Found {} lines", raw_lines.len());

    let mut grid = MirrorField::from_raw_lines(&raw_lines);

    grid.print_size();

    grid.simulate_laser();

    println!("P1 Solution: {}", grid.count_energized());

    Ok(())
}

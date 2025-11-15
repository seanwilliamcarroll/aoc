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
struct Pose {
    direction: Direction,
    coordinate: Coordinate,
}

#[derive(Clone)]
struct MirrorField {
    grid: Grid<Tile>,
    energized: Grid<bool>,
    seen_poses: HashSet<Pose>,
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
            seen_poses: HashSet::new(),
        }
    }

    fn print_size(&self) {
        println!("{} x {} grid", self.grid.len(), self.grid[0].len());
    }

    fn next_space(&self, pose: Pose) -> Option<Pose> {
        let Pose {
            direction,
            coordinate,
        } = pose;

        let (row_index, col_index) = coordinate;

        match direction {
            Direction::North => {
                if row_index == 0 {
                    None
                } else {
                    Some(Pose {
                        direction,
                        coordinate: (row_index - 1, col_index),
                    })
                }
            }
            Direction::East => {
                if col_index == self.grid[0].len() - 1 {
                    None
                } else {
                    Some(Pose {
                        direction,
                        coordinate: (row_index, col_index + 1),
                    })
                }
            }
            Direction::South => {
                if row_index == self.grid.len() - 1 {
                    None
                } else {
                    Some(Pose {
                        direction,
                        coordinate: (row_index + 1, col_index),
                    })
                }
            }
            Direction::West => {
                if col_index == 0 {
                    None
                } else {
                    Some(Pose {
                        direction,
                        coordinate: (row_index, col_index - 1),
                    })
                }
            }
        }
    }

    fn step_light(&mut self, pose: Pose) -> Vec<Pose> {
        // Light has just entered pose
        // Need to mark cell as energized and return new poses as beam continues

        if !self.seen_poses.insert(pose.clone()) {
            return vec![];
        }

        let Pose {
            direction,
            coordinate,
        } = pose;
        let (row_index, col_index) = coordinate;

        self.energized[row_index][col_index] = true;

        // Get next paths
        let mut output = vec![];

        let tile = self.grid[row_index][col_index];

        for new_direction in direction.next_directions(tile) {
            if let Some(new_pose) = self.next_space(Pose {
                direction: new_direction,
                coordinate,
            }) {
                output.push(new_pose);
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

    fn simulate_laser(&mut self, initial_pose: Pose) {
        let mut queue: VecDeque<Pose> = VecDeque::new();

        queue.push_back(initial_pose);

        while let Some(new_pose) = queue.pop_front() {
            let new_poses = self.step_light(new_pose);
            queue.extend(new_poses.into_iter());
        }
    }

    fn simulate_laser_top_left(&mut self) {
        self.simulate_laser(Pose {
            direction: Direction::East,
            coordinate: (0, 0),
        });
    }

    fn simulate_all_lasers(&self) -> usize {
        let mut energized = 0usize;

        for row_index in 0..self.grid.len() {
            let mut new_grid = self.clone();
            new_grid.simulate_laser(Pose {
                direction: Direction::East,
                coordinate: (row_index, 0),
            });
            energized = usize::max(new_grid.count_energized(), energized);

            new_grid = self.clone();
            new_grid.simulate_laser(Pose {
                direction: Direction::West,
                coordinate: (row_index, self.grid[0].len() - 1),
            });
            energized = usize::max(new_grid.count_energized(), energized);
        }

        for col_index in 0..self.grid[0].len() {
            let mut new_grid = self.clone();
            new_grid.simulate_laser(Pose {
                direction: Direction::South,
                coordinate: (0, col_index),
            });
            energized = usize::max(new_grid.count_energized(), energized);

            new_grid = self.clone();
            new_grid.simulate_laser(Pose {
                direction: Direction::North,
                coordinate: (self.grid.len() - 1, col_index),
            });
            energized = usize::max(new_grid.count_energized(), energized);
        }

        energized
    }
}

fn main() -> std::io::Result<()> {
    let raw_lines = read_from_file("input.txt")?;

    println!("Found {} lines", raw_lines.len());

    let mut grid = MirrorField::from_raw_lines(&raw_lines);

    grid.print_size();

    grid.simulate_laser_top_left();

    println!("P1 Solution: {}", grid.count_energized());

    grid = MirrorField::from_raw_lines(&raw_lines);

    println!("P2 Solution: {}", grid.simulate_all_lasers());

    Ok(())
}

use std::collections::{HashMap, VecDeque};
use std::fs::File;
use std::io::{self, BufRead};

type RawLines = Vec<String>;

fn read_from_file(filepath: &str) -> std::io::Result<RawLines> {
    let file = File::open(filepath)?;
    io::BufReader::new(file).lines().collect()
}

type Grid = Vec<Vec<usize>>;

fn grid_from_raw_lines(lines: &RawLines) -> Grid {
    lines
        .iter()
        .map(|line| {
            line.chars()
                .map(|tile| tile.to_digit(10).expect("Bad formatting") as usize)
                .collect::<Vec<usize>>()
        })
        .collect::<Vec<Vec<usize>>>()
}

type Position = (usize, usize);

// We've got the grid of numbers, want to find least energy use path

// Store shortest path to each tile as we navigate them, along with score to get there
// At each step, look to see if the path we took to get there was the shortest possible?

#[derive(Copy, Clone, Eq, PartialEq, Debug, Hash)]
enum Direction {
    North,
    East,
    South,
    West,
}

impl Direction {
    fn next_position(&self, position: Position, grid: &Grid) -> Option<Position> {
        let (row_index, col_index) = position;

        match *self {
            Self::North => {
                if row_index == 0 {
                    None
                } else {
                    Some((row_index - 1, col_index))
                }
            }
            Self::East => {
                if col_index == grid[0].len() - 1 {
                    None
                } else {
                    Some((row_index, col_index + 1))
                }
            }
            Self::South => {
                if row_index == grid.len() - 1 {
                    None
                } else {
                    Some((row_index + 1, col_index))
                }
            }
            Self::West => {
                if col_index == 0 {
                    None
                } else {
                    Some((row_index, col_index - 1))
                }
            }
        }
    }

    fn next_direction(self) -> Vec<Self> {
        match self {
            Self::North => vec![Self::West, Self::North, Self::East],
            Self::East => vec![Self::North, Self::East, Self::South],
            Self::South => vec![Self::East, Self::South, Self::West],
            Self::West => vec![Self::South, Self::West, Self::North],
        }
    }
}

type Step = (Direction, usize);

struct Route {
    position: Position,
    last_step: Step,
    heat_so_far: usize,
}

// Store last direction traveled and how many steps you did before coming to square and score
// If equal heat loss, store direction and distance if different

fn find_lowest_heat_loss(grid: &Grid, at_least_steps: usize, no_more_than_steps: usize) -> usize {
    // Want to have data structure that represents a point and all 4 entry points with all 3 possible steps into us

    let mut lhl_grid: Vec<Vec<HashMap<Step, usize>>> =
        vec![vec![HashMap::new(); grid[0].len()]; grid.len()];

    let mut next_attempts: VecDeque<Route> = VecDeque::new();

    next_attempts.push_back(Route {
        position: (0, 1),
        last_step: (Direction::East, 1),
        heat_so_far: 0,
    });

    next_attempts.push_back(Route {
        position: (1, 0),
        last_step: (Direction::South, 1),
        heat_so_far: 0,
    });

    while let Some(route) = next_attempts.pop_front() {
        let Route {
            position,
            last_step,
            heat_so_far,
        } = route;
        let (direction, steps) = last_step;
        // We've just entered this step
        let (row_index, col_index) = position;
        let just_added_heat_loss = grid[row_index][col_index];
        let new_total_heat_loss = heat_so_far + just_added_heat_loss;

        if let Some(heat_loss) = lhl_grid[row_index][col_index].get(&last_step) {
            // Already have one, need to check if greater than old one
            if *heat_loss <= new_total_heat_loss {
                continue;
            }
        }
        // New local low for this point with entrance vec
        lhl_grid[row_index][col_index].insert(last_step, new_total_heat_loss);

        for next_direction in direction.next_direction() {
            let new_steps = if next_direction == direction {
                if steps == no_more_than_steps {
                    continue;
                }
                steps + 1
            } else {
                if steps < at_least_steps {
                    continue;
                }
                1
            };
            if let Some(next_position) = next_direction.next_position(position, grid) {
                next_attempts.push_back(Route {
                    position: next_position,
                    last_step: (next_direction, new_steps),
                    heat_so_far: heat_so_far + just_added_heat_loss,
                });
            }
        }
    }

    let mut lowest_heat_loss = usize::MAX;
    for ((_, steps), heat_loss) in &lhl_grid[grid.len() - 1][grid[0].len() - 1] {
        if lowest_heat_loss > *heat_loss && *steps >= at_least_steps {
            lowest_heat_loss = *heat_loss;
        }
    }

    lowest_heat_loss
}

fn main() -> std::io::Result<()> {
    let raw_lines = read_from_file("input.txt")?;

    println!("Found {} lines", raw_lines.len());

    let grid = grid_from_raw_lines(&raw_lines);

    println!("{} x {} grid", grid.len(), grid[0].len());

    let p1_solution = find_lowest_heat_loss(&grid, 1, 3);

    println!("P1 Solution: {p1_solution}");

    let p2_solution = find_lowest_heat_loss(&grid, 4, 10);

    println!("P2 Solution: {p2_solution}");

    Ok(())
}

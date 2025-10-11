use std::collections::VecDeque;
use std::fs::File;
use std::io::{self, BufRead};

type RawLines = Vec<String>;

fn read_from_file(filepath: &str) -> std::io::Result<RawLines> {
    let file = File::open(filepath)?;
    io::BufReader::new(file).lines().collect()
}

type Tile = char;
type Row = Vec<Tile>;
type Grid = Vec<Row>;

type Coordinates = (usize, usize);

const START_TILE: Tile = 'S';
const VERTICAL_PIPE: Tile = '|';
const HORIZONTAL_PIPE: Tile = '-';
const NORTH_EAST_BEND: Tile = 'L';
const NORTH_WEST_BEND: Tile = 'J';
const SOUTH_WEST_BEND: Tile = '7';
const SOUTH_EAST_BEND: Tile = 'F';

fn is_legal_tile(tile: Tile) -> bool {
    tile == START_TILE
        || tile == VERTICAL_PIPE
        || tile == HORIZONTAL_PIPE
        || tile == NORTH_EAST_BEND
        || tile == NORTH_WEST_BEND
        || tile == SOUTH_WEST_BEND
        || tile == SOUTH_EAST_BEND
}

#[derive(PartialEq, Eq)]
enum Direction {
    North,
    East,
    South,
    West,
}

impl Direction {
    fn from_coordinates(current_coordinates: Coordinates, next_coordinates: Coordinates) -> Self {
        let (current_row, current_col) = current_coordinates;
        let (next_row, next_col) = next_coordinates;
        assert!(
            (current_row.abs_diff(next_row) == 1usize) ^ (current_col.abs_diff(next_col) == 1usize)
        );

        if current_row.abs_diff(next_row) == 1usize {
            if current_row > next_row {
                Self::North
            } else {
                Self::South
            }
        } else {
            if current_col > next_col {
                Self::West
            } else {
                Self::East
            }
        }
    }
}

fn can_enter_tile_from_direction(tile: Tile, direction: &Direction) -> bool {
    match tile {
        START_TILE => true,
        VERTICAL_PIPE => *direction == Direction::North || *direction == Direction::South,
        HORIZONTAL_PIPE => *direction == Direction::East || *direction == Direction::West,
        NORTH_EAST_BEND => *direction == Direction::West || *direction == Direction::South,
        NORTH_WEST_BEND => *direction == Direction::East || *direction == Direction::South,
        SOUTH_WEST_BEND => *direction == Direction::East || *direction == Direction::North,
        SOUTH_EAST_BEND => *direction == Direction::West || *direction == Direction::North,
        _ => panic!("Illegal tile!"),
    }
}

fn can_exit_tile_from_direction(tile: Tile, direction: &Direction) -> bool {
    match tile {
        START_TILE => true,
        VERTICAL_PIPE => *direction == Direction::North || *direction == Direction::South,
        HORIZONTAL_PIPE => *direction == Direction::East || *direction == Direction::West,
        NORTH_EAST_BEND => *direction == Direction::East || *direction == Direction::North,
        NORTH_WEST_BEND => *direction == Direction::West || *direction == Direction::North,
        SOUTH_WEST_BEND => *direction == Direction::West || *direction == Direction::South,
        SOUTH_EAST_BEND => *direction == Direction::East || *direction == Direction::South,
        _ => panic!("Illegal tile!"),
    }
}

struct Maze {
    grid: Grid,
    start: Coordinates,
}

struct Path {
    path: Vec<Coordinates>,
    current_coordinates: Coordinates,
}

impl Path {
    fn from_parts(path: Vec<Coordinates>, current_coordinates: Coordinates) -> Self {
        Self {
            path,
            current_coordinates,
        }
    }

    fn last_position(&self) -> Coordinates {
        assert!(!self.path.is_empty());
        self.path[self.path.len() - 1]
    }
}

impl Maze {
    fn from_lines(lines: &RawLines) -> Self {
        let grid = lines
            .iter()
            .map(|line| line.chars().collect::<Row>())
            .collect::<Grid>();

        let mut start_row = 0;
        let mut start_col = 0;

        'outer_loop: for (row_index, row) in grid.iter().enumerate() {
            for (col_index, character) in row.iter().enumerate() {
                if *character == START_TILE {
                    start_row = row_index;
                    start_col = col_index;
                    break 'outer_loop;
                }
            }
        }

        let start = (start_row, start_col);

        Self { grid, start }
    }

    fn tile_at(&self, coordinates: Coordinates) -> Tile {
        let (row_index, col_index) = coordinates;
        assert!(row_index < self.num_rows());
        assert!(col_index < self.num_cols());
        self.grid[row_index][col_index]
    }

    fn num_rows(&self) -> usize {
        self.grid.len()
    }

    fn num_cols(&self) -> usize {
        self.grid[0].len()
    }

    fn can_enter_tile(
        &self,
        current_coordinates: Coordinates,
        next_coordinates: Coordinates,
    ) -> bool {
        let (current_row, current_col) = current_coordinates;
        let (next_row, next_col) = next_coordinates;
        let current_tile = self.grid[current_row][current_col];
        let next_tile = self.grid[next_row][next_col];

        let direction = Direction::from_coordinates(current_coordinates, next_coordinates);

        if !is_legal_tile(next_tile) {
            false
        } else {
            can_exit_tile_from_direction(current_tile, &direction)
                && can_enter_tile_from_direction(next_tile, &direction)
        }
    }

    fn coordinates_adjacent_to(&self, coordinates: Coordinates) -> Vec<Coordinates> {
        let mut output = vec![];

        let (row_index, col_index) = coordinates;

        if row_index > 0 {
            output.push((row_index - 1, col_index));
        }

        if row_index < self.num_rows() - 1 {
            output.push((row_index + 1, col_index));
        }

        if col_index > 0 {
            output.push((row_index, col_index - 1));
        }

        if col_index < self.num_cols() - 1 {
            output.push((row_index, col_index + 1));
        }

        output
    }

    fn coordinates_reachable_from(&self, coordinates: Coordinates) -> Vec<Coordinates> {
        self.coordinates_adjacent_to(coordinates)
            .into_iter()
            .filter(|new_coords| self.can_enter_tile(coordinates, *new_coords))
            .collect::<Vec<Coordinates>>()
    }

    fn find_loop_from_start(&self) -> Vec<Coordinates> {
        // Want to do breadth first search
        // Create queue of next possible paths, with path taken to reach that point
        // At fork, clone paths and add each branch to the queue
        // Repeat until either a path can no longer move
        // Or we hit start again

        let mut next_paths_to_try = self
            .coordinates_reachable_from(self.start)
            .into_iter()
            .map(|coordinates| Path::from_parts(vec![self.start], coordinates))
            .collect::<VecDeque<Path>>();

        loop {
            if let Some(path) = next_paths_to_try.pop_front() {
                let last_coordinates = path.last_position();
                let next_coordinates = path.current_coordinates;

                let mut new_path = path.path.clone();
                new_path.push(next_coordinates);

                if self.tile_at(next_coordinates) == START_TILE {
                    return new_path;
                }

                let new_coordinates_to_visit = self.coordinates_reachable_from(next_coordinates);
                for new_coordinate_to_visit in new_coordinates_to_visit {
                    if new_coordinate_to_visit == last_coordinates {
                        continue;
                    }
                    next_paths_to_try
                        .push_back(Path::from_parts(new_path.clone(), new_coordinate_to_visit));
                }
            }
        }
    }

    fn p1_solution(&self) -> usize {
        let loop_from_start = self.find_loop_from_start();
        (loop_from_start.len() - 2) / 2 + 1
    }
}

fn main() -> std::io::Result<()> {
    let raw_lines = read_from_file("input.txt")?;

    let maze = Maze::from_lines(&raw_lines);

    println!(
        "Maze start: {:?} Maze dims: {} x {}",
        maze.start,
        maze.num_rows(),
        maze.num_cols()
    );

    println!("P1 solution: {}", maze.p1_solution());

    Ok(())
}

use std::collections::{HashSet, VecDeque};
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

fn print_grid(grid: &Grid) {
    for row in grid {
        for character in row {
            print!("{character}");
        }
        println!();
    }
}

type Coordinates = (usize, usize);

const START_TILE: Tile = 'S';
const VERTICAL_PIPE: Tile = '|';
const HORIZONTAL_PIPE: Tile = '-';
const NORTH_EAST_BEND: Tile = 'L';
const NORTH_WEST_BEND: Tile = 'J';
const SOUTH_WEST_BEND: Tile = '7';
const SOUTH_EAST_BEND: Tile = 'F';
const EMPTY_TILE: Tile = '.';

fn is_legal_tile(tile: Tile) -> bool {
    tile == START_TILE
        || tile == VERTICAL_PIPE
        || tile == HORIZONTAL_PIPE
        || tile == NORTH_EAST_BEND
        || tile == NORTH_WEST_BEND
        || tile == SOUTH_WEST_BEND
        || tile == SOUTH_EAST_BEND
}

#[derive(PartialEq, Eq, Hash, Debug, Clone)]
enum Direction {
    North,
    East,
    South,
    West,
    NorthWest,
    NorthEast,
    SouthWest,
    SouthEast,
}

impl Direction {
    fn from_coordinates(current_coordinates: Coordinates, next_coordinates: Coordinates) -> Self {
        let (current_row, current_col) = current_coordinates;
        let (next_row, next_col) = next_coordinates;

        if current_row.abs_diff(next_row) == 1usize && current_col.abs_diff(next_col) == 1usize {
            if current_row > next_row && current_col > next_col {
                Self::NorthWest
            } else if current_row > next_row {
                Self::NorthEast
            } else if current_col > next_col {
                Self::SouthWest
            } else {
                Self::SouthEast
            }
        } else if current_row.abs_diff(next_row) == 1usize {
            if current_row > next_row {
                Self::North
            } else {
                Self::South
            }
        } else if current_col > next_col {
            Self::West
        } else {
            Self::East
        }
    }

    fn to_the_right(&self) -> Self {
        match self {
            Self::North => Self::East,
            Self::East => Self::South,
            Self::South => Self::West,
            Self::West => Self::North,
            _ => panic!("Not allowed!"),
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
    connected_loop: Vec<Coordinates>,
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

        Self {
            grid,
            start,
            connected_loop: vec![],
        }
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

        is_legal_tile(next_tile)
            && can_exit_tile_from_direction(current_tile, &direction)
            && can_enter_tile_from_direction(next_tile, &direction)
    }

    fn try_move_direction(
        &self,
        coordinates: Coordinates,
        direction: &Direction,
    ) -> Option<Coordinates> {
        let (row_index, col_index) = coordinates;
        match *direction {
            Direction::North => {
                if row_index == 0 {
                    None
                } else {
                    Some((row_index - 1, col_index))
                }
            }
            Direction::East => {
                if col_index == self.grid[0].len() - 1 {
                    None
                } else {
                    Some((row_index, col_index + 1))
                }
            }
            Direction::South => {
                if row_index == self.grid.len() - 1 {
                    None
                } else {
                    Some((row_index + 1, col_index))
                }
            }
            Direction::West => {
                if col_index == 0 {
                    None
                } else {
                    Some((row_index, col_index - 1))
                }
            }
            Direction::NorthEast => {
                if row_index == 0 || col_index == self.grid[0].len() - 1 {
                    None
                } else {
                    Some((row_index - 1, col_index + 1))
                }
            }
            Direction::NorthWest => {
                if row_index == 0 || col_index == 0 {
                    None
                } else {
                    Some((row_index - 1, col_index - 1))
                }
            }
            Direction::SouthEast => {
                if row_index == self.grid.len() - 1 || col_index == self.grid[0].len() - 1 {
                    None
                } else {
                    Some((row_index + 1, col_index + 1))
                }
            }
            Direction::SouthWest => {
                if row_index == self.grid.len() - 1 || col_index == 0 {
                    None
                } else {
                    Some((row_index + 1, col_index - 1))
                }
            }
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

    fn coordinates_adjacent_to_with_diagonals(&self, coordinates: Coordinates) -> Vec<Coordinates> {
        let mut output = self.coordinates_adjacent_to(coordinates);

        let (row_index, col_index) = coordinates;

        if row_index > 0 && col_index > 0 {
            output.push((row_index - 1, col_index - 1));
        }

        if row_index < self.num_rows() - 1 && col_index > 0 {
            output.push((row_index + 1, col_index - 1));
        }

        if row_index > 0 && col_index < self.num_cols() - 1 {
            output.push((row_index - 1, col_index + 1));
        }

        if row_index < self.num_rows() - 1 && col_index < self.num_cols() - 1 {
            output.push((row_index + 1, col_index + 1));
        }

        output
    }

    fn coordinates_reachable_from(&self, coordinates: Coordinates) -> Vec<Coordinates> {
        self.coordinates_adjacent_to(coordinates)
            .into_iter()
            .filter(|new_coords| self.can_enter_tile(coordinates, *new_coords))
            .collect::<Vec<Coordinates>>()
    }

    fn find_loop_from_start(&self) -> Self {
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

        while let Some(path) = next_paths_to_try.pop_front() {
            let last_coordinates = path.last_position();
            let next_coordinates = path.current_coordinates;

            let mut new_path = path.path.clone();
            new_path.push(next_coordinates);

            if self.tile_at(next_coordinates) == START_TILE {
                return Self {
                    start: self.start,
                    grid: self.grid.clone(),
                    connected_loop: new_path,
                };
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
        panic!("Path not found!");
    }

    fn p1_solution(&self) -> usize {
        let loop_from_start = self.find_loop_from_start().connected_loop;
        (loop_from_start.len() - 2) / 2 + 1
    }

    fn count_inner_tiles(&self) -> usize {
        // Get the loop and all positions on the loop
        let loop_from_start = self.find_loop_from_start().connected_loop;

        let positions_on_loop: HashSet<Coordinates> = loop_from_start
            .clone()
            .into_iter()
            .collect::<HashSet<Coordinates>>();

        // Modify grid to only leave pipes in the loop on the grid

        let mut new_grid = self.grid.clone();

        for (row_index, row) in new_grid.iter_mut().enumerate() {
            for (col_index, tile) in row.iter_mut().enumerate() {
                if !positions_on_loop.contains(&(row_index, col_index)) {
                    *tile = EMPTY_TILE;
                }
            }
        }

        let last_coordinates = loop_from_start[0];
        println!("Starting coordinates: {last_coordinates:?}");
        let mut current_index = 1;
        let mut current_coordinates = loop_from_start[current_index];

        let mut current_direction =
            Direction::from_coordinates(last_coordinates, current_coordinates);

        let mut right_hand_coordinates = HashSet::new();

        while current_coordinates != loop_from_start[loop_from_start.len() - 1] {
            let direction_to_right = current_direction.clone().to_the_right();
            if let Some((row_index, col_index)) =
                self.try_move_direction(current_coordinates, &direction_to_right)
                && new_grid[row_index][col_index] == EMPTY_TILE
            {
                new_grid[row_index][col_index] = 'R';
                right_hand_coordinates.insert((row_index, col_index));
            }
            // Try move forward
            if let Some(coordinates) =
                self.try_move_direction(current_coordinates, &current_direction)
                && coordinates == loop_from_start[current_index + 1]
            {
                current_index += 1;
                current_coordinates = loop_from_start[current_index];
            } else if self.tile_at(current_coordinates) == NORTH_EAST_BEND {
                // else turn
                if current_direction == Direction::South {
                    current_direction = Direction::East;
                } else {
                    current_direction = Direction::North;
                }
            } else if self.tile_at(current_coordinates) == NORTH_WEST_BEND {
                if current_direction == Direction::South {
                    current_direction = Direction::West;
                } else {
                    current_direction = Direction::North;
                }
            } else if self.tile_at(current_coordinates) == SOUTH_EAST_BEND {
                if current_direction == Direction::North {
                    current_direction = Direction::East;
                } else {
                    current_direction = Direction::South;
                }
            } else if self.tile_at(current_coordinates) == SOUTH_WEST_BEND {
                if current_direction == Direction::North {
                    current_direction = Direction::West;
                } else {
                    current_direction = Direction::South;
                }
            }
        }

        // Walk over all Rs, visit other empty tiles touching with BFS
        let mut bfs_queue: VecDeque<Coordinates> = right_hand_coordinates
            .clone()
            .into_iter()
            .collect::<VecDeque<Coordinates>>();

        while !bfs_queue.is_empty() {
            // Get all adjacent coordinates that are empty tiles
            let coordinates = bfs_queue.pop_front().expect("Already checked empty");
            let next_to_try = self.coordinates_adjacent_to_with_diagonals(coordinates);

            for new_coordinates in &next_to_try {
                if right_hand_coordinates.contains(new_coordinates) {
                    continue;
                }
                if positions_on_loop.contains(new_coordinates) {
                    continue;
                }
                right_hand_coordinates.insert(*new_coordinates);
                bfs_queue.push_back(*new_coordinates);
                let (row_index, col_index) = *new_coordinates;
                new_grid[row_index][col_index] = 'R';
            }
        }

        print_grid(&new_grid);

        if right_hand_coordinates.contains(&(0, 0)) {
            // Want to return count of EMPTY_TILEs in new_grid
            new_grid
                .into_iter()
                .map(|row| {
                    row.into_iter()
                        .map(|character| usize::from(character == EMPTY_TILE))
                        .sum::<usize>()
                })
                .sum::<usize>()
        } else {
            right_hand_coordinates.len()
        }
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

    let inner_cells = maze.count_inner_tiles();
    println!("P1 solution: {inner_cells}");

    Ok(())
}

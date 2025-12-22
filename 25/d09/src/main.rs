use std::collections::{HashMap, HashSet, VecDeque};
use std::fs::File;
use std::io::{self, BufRead};

type RawLines = Vec<String>;

fn read_from_file(filepath: &str) -> std::io::Result<RawLines> {
    let file = File::open(filepath)?;
    io::BufReader::new(file).lines().collect()
}

#[derive(Debug, Eq, PartialEq, Clone, Hash, Copy)]
struct Position(usize, usize);

impl Position {
    fn from_line(line: &str) -> Self {
        let (p0_str, p1_str) = line.split_once(',').expect("Bad formatting!");

        Self(
            p0_str.parse::<usize>().expect("Formatting!"),
            p1_str.parse::<usize>().expect("Formatting!"),
        )
    }

    fn area(&self, other: &Position) -> usize {
        let min_x = usize::min(self.0, other.0);
        let max_x = usize::max(self.0, other.0);
        let min_y = usize::min(self.1, other.1);
        let max_y = usize::max(self.1, other.1);

        (min_x.abs_diff(max_x) + 1) * (min_y.abs_diff(max_y) + 1)
    }
}

fn largest_rectangle(positions: &Vec<Position>) -> usize {
    let mut largest = 0usize;
    for position_0 in positions {
        for position_1 in positions {
            if position_0 == position_1 {
                continue;
            }
            let area = position_0.area(&position_1);
            if area > largest {
                largest = area;
            }
        }
    }
    largest
}

struct CompressedGrid {
    compressed_to_original: HashMap<Position, Position>,
    grid: Vec<Vec<char>>,
}

impl CompressedGrid {
    fn from_points(points: Vec<Position>) -> Self {
        let mut unique_values = points
            .clone()
            .into_iter()
            .flat_map(|Position(a, b)| [a, b])
            .collect::<HashSet<usize>>()
            .into_iter()
            .collect::<Vec<usize>>();
        unique_values.sort();

        let max_value = unique_values.len();

        let value_to_compressed = unique_values
            .into_iter()
            .enumerate()
            .map(|(a, b)| (b, a))
            .collect::<HashMap<usize, usize>>();
        let original_to_compressed = points
            .iter()
            .map(|Position(a, b)| {
                (
                    Position(*a, *b),
                    Position(
                        *value_to_compressed.get(&a).expect("Must be here"),
                        *value_to_compressed.get(&b).expect("Must be here"),
                    ),
                )
            })
            .collect::<HashMap<Position, Position>>();

        let compressed_to_original = original_to_compressed
            .iter()
            .map(|(a, b)| (*b, *a))
            .collect::<HashMap<Position, Position>>();

        let mut grid = vec![vec![' '; max_value + 1]; max_value + 1];

        for Position(row_index, col_index) in compressed_to_original.keys() {
            grid[*row_index][*col_index] = '#';
        }

        for index in 0..points.len() {
            let first_index = index;
            let second_index = (index + 1) % points.len();

            let first = original_to_compressed
                .get(&points[first_index])
                .expect("Must be here");
            let second = original_to_compressed
                .get(&points[second_index])
                .expect("Must be here");

            if first.0 == second.0 {
                let min_val = usize::min(first.1, second.1);
                let max_val = usize::max(first.1, second.1);

                for tile_index in min_val..max_val {
                    grid[first.0][tile_index] = 'X';
                }
            } else {
                let min_val = usize::min(first.0, second.0);
                let max_val = usize::max(first.0, second.0);

                for tile_index in min_val..max_val {
                    grid[tile_index][first.1] = 'X';
                }
            }
        }

        // Fill outside, fill inside

        let mut next_point = VecDeque::new();

        next_point.push_back(Position(0, 0));

        while let Some(Position(row_index, col_index)) = next_point.pop_front() {
            if grid[row_index][col_index] == ' ' {
                grid[row_index][col_index] = '.';
                if row_index < grid.len() - 1 && grid[row_index + 1][col_index] == ' ' {
                    next_point.push_back(Position(row_index + 1, col_index));
                }
                if row_index > 0 && grid[row_index - 1][col_index] == ' ' {
                    next_point.push_back(Position(row_index - 1, col_index));
                }
                if col_index < grid[0].len() - 1 && grid[row_index][col_index + 1] == ' ' {
                    next_point.push_back(Position(row_index, col_index + 1));
                }
                if col_index > 0 && grid[row_index][col_index - 1] == ' ' {
                    next_point.push_back(Position(row_index, col_index - 1));
                }
            }
        }

        // Now we can just do all the rectangles and reject those with ' ' inside

        Self {
            compressed_to_original,
            grid,
        }
    }

    fn calculate_real_area(
        &self,
        compressed_point_a: Position,
        compressed_point_b: Position,
    ) -> usize {
        let point_a = self
            .compressed_to_original
            .get(&compressed_point_a)
            .expect("Must have it");
        let point_b = self
            .compressed_to_original
            .get(&compressed_point_b)
            .expect("Must have it");
        point_a.area(&point_b)
    }

    fn biggest_red_green_rect(&self) -> usize {
        let positions = self
            .compressed_to_original
            .keys()
            .map(|val| *val)
            .collect::<Vec<Position>>();

        let mut largest = 0usize;

        for position_0 in &positions {
            'inner_pos_loop: for position_1 in &positions {
                let position_0 = *position_0;
                let position_1 = *position_1;
                if position_0 == position_1 {
                    continue;
                }
                let area = self.calculate_real_area(position_0, position_1);
                if area > largest {
                    // Need to check if in this rect, there is a '.' square

                    let min_x = usize::min(position_0.0, position_1.0);
                    let max_x = usize::max(position_0.0, position_1.0);
                    let min_y = usize::min(position_0.1, position_1.1);
                    let max_y = usize::max(position_0.1, position_1.1);

                    for row_index in min_x..=max_x {
                        for col_index in min_y..=max_y {
                            if self.grid[row_index][col_index] == '.' {
                                continue 'inner_pos_loop;
                            }
                        }
                    }
                    largest = area;
                }
            }
        }

        largest
    }
}

fn main() -> std::io::Result<()> {
    let raw_lines = read_from_file("input.txt")?;

    println!("Found {} lines", raw_lines.len());

    let positions = raw_lines
        .iter()
        .map(|line| Position::from_line(line))
        .collect::<Vec<Position>>();

    let p1_solution = largest_rectangle(&positions);

    println!("P1 Solution: {p1_solution}");

    let compressed_grid = CompressedGrid::from_points(positions);

    let p2_solution = compressed_grid.biggest_red_green_rect();

    println!("P2 Solution: {p2_solution}");

    Ok(())
}

use std::fs::File;
use std::io::{self, BufRead};

type RawLines = Vec<String>;

fn read_from_file(filepath: &str) -> std::io::Result<RawLines> {
    let file = File::open(filepath)?;
    io::BufReader::new(file).lines().collect()
}

type Tile = char;

const EMPTY_TILE: Tile = '.';
const ROLL_TILE: Tile = '@';

const MAX_ADJACENT: usize = 3;

type Grid = Vec<Vec<Tile>>;

fn grid_from_raw_lines(lines: RawLines) -> Grid {
    lines
        .into_iter()
        .map(|line| line.chars().collect::<Vec<Tile>>())
        .collect::<Grid>()
}

fn count_and_remove_accessible_rolls(grid: &Grid) -> (usize, Grid) {
    let mut count = 0usize;

    let mut new_grid = grid.clone();

    for (row_index, row) in grid.iter().enumerate() {
        for (col_index, tile) in row.iter().enumerate() {
            let tile = *tile;
            if tile == EMPTY_TILE {
                continue;
            }

            let mut num_adjacent = 0usize;

            let is_row_tile =
                |row_i: usize, col_i: usize| -> bool { grid[row_i][col_i] == ROLL_TILE };

            let not_top_row = row_index != 0;
            let not_bottom_row = row_index != grid.len() - 1;

            let not_top_col = col_index != 0;
            let not_bottom_col = col_index != grid[row_index].len() - 1;

            if not_top_row && is_row_tile(row_index - 1, col_index) {
                num_adjacent += 1;
            }

            if not_top_col && is_row_tile(row_index, col_index - 1) {
                num_adjacent += 1;
            }

            if not_bottom_row && is_row_tile(row_index + 1, col_index) {
                num_adjacent += 1;
            }

            if not_bottom_col && is_row_tile(row_index, col_index + 1) {
                num_adjacent += 1;
            }

            if not_top_row && not_top_col && is_row_tile(row_index - 1, col_index - 1) {
                num_adjacent += 1;
            }

            if not_top_row && not_bottom_col && is_row_tile(row_index - 1, col_index + 1) {
                num_adjacent += 1;
            }

            if not_bottom_row && not_top_col && is_row_tile(row_index + 1, col_index - 1) {
                num_adjacent += 1;
            }

            if not_bottom_row && not_bottom_col && is_row_tile(row_index + 1, col_index + 1) {
                num_adjacent += 1;
            }

            if num_adjacent <= MAX_ADJACENT {
                count += 1;
                new_grid[row_index][col_index] = EMPTY_TILE;
            }
        }
    }

    (count, new_grid)
}

fn main() -> std::io::Result<()> {
    let raw_lines = read_from_file("input.txt")?;

    println!("Found {} lines", raw_lines.len());

    let mut grid = grid_from_raw_lines(raw_lines);

    println!("{} x {} Grid", grid.len(), grid[0].len());

    let (p1_solution, new_grid) = count_and_remove_accessible_rolls(&grid);

    grid = new_grid;

    println!("P1 Solution: {p1_solution}");

    let mut count = p1_solution;

    let mut p2_solution = p1_solution;

    while count != 0 {
        (count, grid) = count_and_remove_accessible_rolls(&grid);
        p2_solution += count;
    }

    println!("P2 Solution: {p2_solution}");

    Ok(())
}

use std::collections::{HashMap, HashSet};
use std::fs::File;
use std::io::{self, BufRead};

type RawLines = Vec<String>;

fn read_from_file(filepath: &str) -> std::io::Result<RawLines> {
    let file = File::open(filepath)?;
    io::BufReader::new(file).lines().collect()
}

type Coordinates = (usize, usize);
type Tile = char;

const GALAXY_TILE: Tile = '#';

const P1_MAGNIFICATION_FACTOR: usize = 2;
const P2_MAGNIFICATION_FACTOR: usize = 1_000_000;

fn get_original_coordinates(lines: &RawLines) -> Vec<Coordinates> {
    let mut output = Vec::new();
    for (row_index, row) in lines.iter().enumerate() {
        for (col_index, character) in row.chars().enumerate() {
            if character == GALAXY_TILE {
                output.push((row_index, col_index));
            }
        }
    }

    output
}

fn main() -> std::io::Result<()> {
    let raw_lines = read_from_file("input.txt")?;

    let all_coordinates = get_original_coordinates(&raw_lines);

    println!("Found {} galaxies", all_coordinates.len());

    let non_empty_space_rows = all_coordinates
        .iter()
        .map(|(row_index, _)| *row_index)
        .collect::<HashSet<usize>>();

    let non_empty_space_cols = all_coordinates
        .iter()
        .map(|(_, col_index)| *col_index)
        .collect::<HashSet<usize>>();

    let empty_space_rows = (0..raw_lines.len())
        .filter(|element| !non_empty_space_rows.contains(element))
        .collect::<HashSet<usize>>();

    let empty_space_cols = (0..raw_lines[0].len())
        .filter(|element| !non_empty_space_cols.contains(element))
        .collect::<HashSet<usize>>();

    println!("Num rows with empty space {}", empty_space_rows.len());

    println!("Num cols with empty space {}", empty_space_cols.len());

    let mut all_pairings = HashMap::new();
    let mut total_p1_distance = 0usize;
    let mut total_p2_distance = 0usize;
    let mut total_pairings = 0usize;

    for coordinates in &all_coordinates {
        for inner_coordinates in &all_coordinates {
            if coordinates == inner_coordinates {
                continue;
            }
            if all_pairings.contains_key(&(*coordinates, *inner_coordinates)) {
                continue;
            }
            if all_pairings.contains_key(&(*inner_coordinates, *coordinates)) {
                continue;
            }
            let (row_index_a, col_index_a) = *coordinates;
            let (row_index_b, col_index_b) = *inner_coordinates;
            let (row_index_a, row_index_b) = (
                usize::min(row_index_a, row_index_b),
                usize::max(row_index_a, row_index_b),
            );
            let (col_index_a, col_index_b) = (
                usize::min(col_index_a, col_index_b),
                usize::max(col_index_a, col_index_b),
            );
            // Maybe more efficient way, but works well enought to find empty space
            let empty_row_space = (row_index_a..row_index_b)
                .filter(|element| empty_space_rows.contains(element))
                .collect::<Vec<_>>()
                .len();
            let empty_col_space = (col_index_a..col_index_b)
                .filter(|element| empty_space_cols.contains(element))
                .collect::<Vec<_>>()
                .len();
            let p1_distance = row_index_a.abs_diff(row_index_b)
                + col_index_a.abs_diff(col_index_b)
                + empty_row_space * (P1_MAGNIFICATION_FACTOR - 1)
                + empty_col_space * (P1_MAGNIFICATION_FACTOR - 1);
            let p2_distance = row_index_a.abs_diff(row_index_b)
                + col_index_a.abs_diff(col_index_b)
                + empty_row_space * (P2_MAGNIFICATION_FACTOR - 1)
                + empty_col_space * (P2_MAGNIFICATION_FACTOR - 1);
            all_pairings.insert((*coordinates, *inner_coordinates), p1_distance);
            total_p1_distance += p1_distance;
            total_p2_distance += p2_distance;
            total_pairings += 1;
        }
    }

    println!("P1 solution: {total_p1_distance} over {total_pairings} pairings");
    println!("P2 solution: {total_p2_distance} over {total_pairings} pairings");

    Ok(())
}
